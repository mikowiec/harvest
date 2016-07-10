
#include "os/compat.h"

#include "os/debug.h"

#include "main/types.h"
#include "main/types_io.h"
#include "object/base.h"
#include "object/sound.h"
#include "object/object.h"
#include "misc/free.h"
 
#include "snd/sound_system.h"
#include "snd/accessor.h"

#include "os/snd.h"

namespace reaper
{
namespace object
{
namespace sound
{

Base::~Base()
{ }

debug::DebugOutput derr("object::sound");

typedef snd::EffectPtr EffectPtr;

class StaticImpl : public Static
{
	std::auto_ptr<reaper::sound::Accessor> acc;
	std::auto_ptr<reaper::sound::Effect> eff;
public:
	StaticImpl(const std::string& name, const Point& pos)
	{
		acc = std::auto_ptr<reaper::sound::Accessor>(reaper::sound::mk_accessor());
		eff = std::auto_ptr<reaper::sound::Effect>(acc->make(name, false));
		eff->set_position(pos);
		eff->set_volume(0.5);
	}
	void ping()
	{
		eff->play();
	}
};

Static* create_static(const std::string& name, const Point& pos)
{
	return new StaticImpl(name, pos);
}

typedef std::auto_ptr<reaper::sound::Accessor> AccPtr;
typedef std::auto_ptr<reaper::sound::Effect> EffPtr;

class NewShip : public Ship
{
	AccPtr acc;
	EffPtr eff;

public:
	NewShip(const std::string& name, const Matrix& m, const Vector& v)
	{
		acc = AccPtr(reaper::sound::mk_accessor());
//		eff = EffPtr(acc->make("engine_behind", true));
//		eff->set_position(m.pos());
//		eff->set_velocity(v);
//		eff->set_volume(0.5);
//		eff->play();
	}
	~NewShip()
	{
	}

	void set_location(const Matrix& m)
	{
//		eff->set_position(m.pos());
	}

	void set_velocity(const Vector& v)
	{
//		eff->set_velocity(v);
	}

	void engine_pitch(float f)
	{
//		eff->set_pitch(1 + f*2);
	}

	void kill()
	{
		if (eff.get()) {
			eff->stop();
			eff.reset();
			acc.reset();
		}
	}
};



Ship* create_ship(const std::string& name, const Matrix& mat, const Vector& vel)
{
	return new NewShip(name, mat, vel);
}


class ProjectileImpl : public Projectile
{
	std::auto_ptr<reaper::sound::Accessor> acc;
	std::auto_ptr<reaper::sound::Effect> eff;
public:
	ProjectileImpl(const std::string& name, const Matrix& mat, const Vector& v)
	{
		acc = AccPtr(reaper::sound::mk_accessor());
		eff = EffPtr(acc->make(name, false));
		eff->set_position(mat.pos());
		eff->set_velocity(v);
		eff->set_volume(0.5);
		eff->play();
	}

	~ProjectileImpl() {
	}
	void set_location(const Matrix& mat)
	{
		eff->set_position(mat.pos());
	}
	void set_velocity(const Vector& vel)
	{
		eff->set_velocity(vel);
	}
};

Projectile* create_proj(const std::string& name, const Matrix& mat, const Vector& vel)
{
	return new ProjectileImpl(name, mat, vel);
}



}
}
}

