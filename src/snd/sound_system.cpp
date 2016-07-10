

#include "os/compat.h"

#include "os/debug.h"
#include "os/exceptions.h"

#include <vector>
#include <deque>
#include <map>
#include <string>
#include <queue>
#include <set>
#include <functional>
#include <algorithm>


#include "main/types_io.h"
#include "main/types_ops.h"
#include "misc/free.h"
#include "misc/sequence.h"
#include "misc/smartptr.h"
#include "misc/stlhelper.h"
#include "object/base.h"
#include "os/debug.h"
#include "os/reltime.h"
#include "os/snd.h"
#include "res/config.h"
#include "snd/accessor.h"
#include "snd/sound_system.h"
#include "world/world.h"

namespace reaper
{
namespace misc {
	UniquePtr<sound::Manager>::I UniquePtr<sound::Manager>::inst;
}
namespace sound
{

debug::DebugOutput derr("sound::sys");

typedef os::snd::Effect* EffectRPtr;


using namespace std;

typedef map<string, EffectRPtr> Cache;

struct Snd
{
	EffectRPtr eff;
	Point pos;
	float dist_sq;
	bool active, looping;
	std::string name;
};

typedef set<Snd*> SndSet;

class Internal
{
	friend class Interface;
	friend class Manager;
	os::snd::SoundSystem ss;

	Cache cache;

	deque<Snd*> loops;
	SndSet death_list;
	/* intersection(loops, death_list) == {} */

	bool sound_ok;
	Point cam_pos;

	int max_sounds, soft_max_loops, max_loops;
	float max_distance;
	int num_sounds, num_loops;

	EffectRPtr dummy;
public:
	Internal()
	 : max_sounds(0), soft_max_loops(0), max_loops(0),
	   max_distance(500), num_sounds(0), num_loops(0)
	{
		res::ConfigEnv env("sound_system");
		max_sounds     = res::withdefault(env, "max_sounds", 10);
		soft_max_loops = res::withdefault(env, "soft_max_loops", 10);
		max_loops      = res::withdefault(env, "max_loops", 10);
		max_distance   = res::withdefault(env, "max_distance", 500);
		max_distance   = max_distance*max_distance;
		sound_ok = ss.init();
		dummy = ss.dummysound().release();
	}
	~Internal()
	{
		delete dummy;
	}
	Snd* prepare(const string& id, bool looping)
	{
		Snd* snd = new Snd;
		snd->eff = dummy;
		snd->pos = Point(0,0,0);
		snd->dist_sq = 0;
		snd->active = false;
		snd->looping = looping;
		snd->name = id;
		if (snd->looping) {
			loops.push_back(snd);
		}
		return snd;
	}

	bool load(Snd* s)
	{
//		derr << "load " << s->name << '\n';
		update_dist_sq(s);
		if (s->dist_sq >= max_distance)
			return false;
		if (num_sounds >= max_sounds)
			return false;

		Cache::iterator i = cache.find(s->name);

		if (i != cache.end()) {
			s->eff = EffectRPtr(i->second->clone().release());
		} else {
			s->eff = EffectRPtr(ss.prepare_effect(s->name).release());
			cache[s->name] = s->eff->clone().release();
		}
		s->eff->set_position(s->pos);
		s->eff->looping(s->looping);
		num_sounds++;
//		derr << "loaded!\n";
		return true;
	}
	struct CmpAddr {
		Snd *snd;
		CmpAddr(Snd *s) : snd(s) { }
		bool operator()(Snd* s) { return s->eff == snd->eff; }
	};

	void del_snd(Snd* s)
	{
		try {
			if (s->eff != dummy)
				delete s->eff;
		} catch (os::hw_error) {
			derr << "(soundeffect) deallocation error!\n";
		}
		delete s;
	}
			
	void unload(Snd* s)
	{
		if (!s->looping) {
			if (!s->eff->is_done())
				death_list.insert(s);
			else {
				num_sounds--;
				del_snd(s);
			}
		} else {
			num_sounds--;
			deque<Snd*>::iterator i = misc::find(misc::seq(loops), s);
			if (i != loops.end())
				loops.erase(i);
			del_snd(s);
		}
	}
	void update_dist_sq(Snd* s) {
		s->dist_sq = length_sq(s->pos - cam_pos);
	}
	struct CmpDist {
		bool operator()(Snd* s1, Snd* s2) { return s1->dist_sq < s2->dist_sq; }
	};
	void work()
	{
		SndSet tmp;
		tmp.swap(death_list);
		for (SndSet::iterator i = tmp.begin(); i != tmp.end(); ++i) {
			if ((*i)->eff->is_done()) {
				num_sounds--;
				delete (*i)->eff;
				delete *i;
			} else {
				death_list.insert(*i);
			}
		}

		misc::for_each(misc::seq(loops), std::bind1st(std::mem_fun(&Internal::update_dist_sq), this));
		misc::sort(misc::seq(loops), CmpDist());
		int n = loops.size();
		int lim = min(soft_max_loops, n);
		int count = 0;
		for (int i = 0; i < n; ++i) {
			Snd* s = loops[i];
			if (count < lim) {
				if (! s->active) {
					play(s);
				}
				if (s->active)
					count++;
			} else {
				if (s->active)
					stop(s);
			}
		}
//		derr << "loops: " << lim << " of " << n << "  dead: " << death_list.size() << '\n';
	}
	struct delete_eff {
		bool operator()(Snd* s) { delete s->eff; return false; }
	};
	void shutdown()
	{
		misc::for_each(misc::seq(death_list), misc::delete_it);
		death_list.clear();
		if (!loops.empty()) {
			derr << "warning, " << loops.size() << " looping sounds not properly unloaded!\n";
			misc::for_each(misc::seq(loops), delete_eff());
			misc::for_each(misc::seq(loops), misc::delete_it);
			loops.clear();
		}
//		ss.shutdown();
	}
	void play(Snd* s)
	{
		if (s->dist_sq < max_distance) {
			if (s->eff == dummy) {
				s->active = load(s);
			} else
				s->active = true;
			s->eff->play();
		} else {
			s->active = false;
		}
	}
	void stop(Snd* s)
	{
		s->eff->stop();
		s->active = false;
	}
};

void Manager::play(const string& id, const Point& pos, float v)
{
	Snd *snd = snd_int->prepare(id, false);
	snd->pos = pos;
	snd_int->load(snd);
	snd->eff->set_position(pos);
	snd->eff->play();
	snd_int->unload(snd);
}



Manager::Manager()
 : snd_int(new Internal())
{
	set_camera(Point(0,0,0), Vector(0,0,1), Vector(0,0,0));
}

Manager::~Manager()
{
	shutdown();
	delete snd_int;
//	SoundRef::force_death();
}

SoundRef Manager::create() { return SoundRef::create(); }
SoundRef Manager::get_ref()  { return SoundRef(); }



void Manager::set_camera(const Point& pos, const Vector& dir, const Vector& vel)
{
	snd_int->cam_pos = pos;
	snd_int->ss.set_listener(pos, dir, vel);
}


template<class Obj>
struct mk_snd {
	int operator()(Obj o) {
		return 0;
	}
};

void Manager::run()
{
	if (!snd_int->sound_ok)
		return;
	snd_int->work();
/*
	os::time::RelTime now = os::time::get_rel_time();
	if (now > (snd_int->last + 1e3)) {
		snd_int->count_move = snd_int->count_pos = 0;
		snd_int->last = now;
		snd_int->tick++;
	}
	while (! snd_int->death_list.empty() && snd_int->death_list.front().first < snd_int->tick - 2) {
		snd_int->death_list.pop();
	}
*/
/*
	world::WorldRef wr = world::World::get_ref();
	for_each(wr->find_st(snd_int->cyl), wr->end_st(), mk_snd<object::StaticPtr>());
	for_each(wr->find_dyn(snd_int->cyl), wr->end_dyn(), mk_snd<object::DynamicPtr>());
	for_each(wr->find_shot(snd_int->cyl), wr->end_shot(), mk_snd<object::ShotPtr>());
*/
}

void Manager::shutdown()
{
	if (snd_int)
		snd_int->shutdown();
}

class DummyEffect : public Effect
{
public:
	DummyEffect()
	{
//		derr << "DummyEffect()\n";
	}
	~DummyEffect()
	{
//		derr << "~DummyEffect()\n";
	}
	virtual void set_position(const Point& p)
	{
//		derr << "DummyEffect::set_position("<<p<<")\n";
	}
	virtual void set_direction(const Vector& v)
	{
//		derr << "DummyEffect::set_direction("<<v<<")\n";
	}
	virtual void set_velocity(const Vector& v)
	{
//		derr << "DummyEffect::set_velocity("<<v<<")\n";
	}

	virtual void set_pitch(float p)
	{
//		derr << "DummyEffect::set_pitch("<<p<<")\n";
	}
	virtual void set_volume(float vol)
	{
//		derr << "DummyEffect::set_volume("<<vol<<")\n";
	}

	virtual void play()
	{
//		derr << "DummyEffect::play()\n";
	}
	virtual void stop()
	{
//		derr << "DummyEffect::stop()\n";
	}
	
};

class EffectImpl : public Effect
{
	Internal* mgr;
	Snd* snd;
public:
	EffectImpl(Internal* m, Snd* s) : mgr(m), snd(s)
	{
//		derr << "EffectImpl()\n";
	}
	~EffectImpl()
	{
//		derr << "~EffectImpl()\n";
		mgr->unload(snd);
	}
	virtual void set_position(const Point& p)
	{
		snd->pos = p;
		snd->eff->set_position(p);
	}
	virtual void set_direction(const Vector& v)
	{
//		derr << "EffectImpl::set_direction("<<v<<")\n";
	}
	virtual void set_velocity(const Vector& v)
	{
//		derr << "EffectImpl::set_velocity("<<v<<")\n";
	}

	virtual void set_pitch(float p)
	{
//		derr << "EffectImpl::set_pitch("<<p<<")\n";
	}
	virtual void set_volume(float vol)
	{
//		derr << "EffectImpl::set_volume("<<vol<<")\n";
	}

	virtual void play()
	{
//		derr << "EffectImpl::play()\n";
		mgr->play(snd);
	}
	virtual void stop()
	{
//		derr << "EffectImpl::stop()\n";
		mgr->stop(snd);
	}
	
};

class DummyAccessor : public Accessor
{
	Internal* mgr;
public:
	DummyAccessor()
	{
		SoundRef si = Manager::get_ref();
		mgr = si->snd_int;
//		derr << "DummyAccessor()\n";
		
	}
	Effect* make(const std::string& id, bool looping)
	{
//		derr << "DummyAccessor::load("<<id<<")\n";
		Snd* snd = mgr->prepare(id, looping);
		return new EffectImpl(mgr, snd);
	}
};

Accessor* mk_accessor()
{
	return new DummyAccessor();
}

}
}



