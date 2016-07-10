
#include "os/compat.h"

#include "os/debug.h"
#include "os/gl.h"
#include "res/res.h"
#include "res/config.h"
#include "gfx/gfx.h"
#include "gfx/settings.h"
#include "gfx/camera.h"
#include "world/world.h"
#include "object/base.h"
#include "object/mkinfo.h"
#include "object/factory.h"
#include "object/objectgroup.h"
#include "gameif.h"
#include "res/resource.h"
#include "misc/parse.h"
#include "misc/stlhelper.h"
#include "main/types_io.h"

#include <cctype>

namespace reaper {
namespace gui {

namespace {
	debug::DebugOutput derr("gameif");
}

using namespace object;
using namespace misc;
using namespace std;

typedef object::ObjectGroup RealObjectGroup;
typedef std::vector<ObjGroupID> ObjGrpIDs;


class Reaper : public GameIF, public Renderer
{
	gfx::Camera cam;
	gfx::RendererRef rr;
	world::WorldRef wr;

	Mode mode;
	string cur_object, cur_level;

	ObjGroupID cur_group;
	
	map<ObjectID, MkInfo> obj_info;

	map<string,ObjectGroup> obj_groups;

	bool is_init, dirty, grabbed;
	ObjectID selected;
	Point cam_pos, cam_tgt;

	string errmsg;

	int next_id;
public:
	Reaper() : mode(NotInit), is_init(false),
		   dirty(false), grabbed(false), next_id(1000)
	{
		res::ConfigEnv duh("game");
		res::ConfigEnv tmp("game");
		object::init_loader("objectgroups");
		wr = world::World::create();
	}


	// GameIF

	auto_ptr<Env> get_cfg_file(string m, string n)
	{
		res::res_stream is(res::File, m + "/" + n);
		Env* r = new Env();
		misc::config_lines(is, *r);
		return auto_ptr<Env>(r);
	}

	ObjectID mk_object(string name) {
		ObjectID id = next_id++;
		MkInfo mk(name);
		mk.id = id;
		obj_info[id] = mk;
		derr << "mk_object " << id << '\n';
		return id;
	}

	ObjectInfo get_object(ObjectID id) {
		const MkInfo& mk = obj_info[id];
		ObjectInfo oi;
		oi.id = id;
		oi.name = mk.name;
		misc::copy(seq(mk.info), std::inserter(oi.info, oi.info.begin()));
		misc::copy(seq(mk.cdata), std::inserter(oi.cdata, oi.cdata.begin()));
		return oi;
	}

	LevelInfo load_level(string name) {
		next_id = 1000;
		LevelInfo li;
		li.env = *get_cfg_file("levels", name);
		misc::split(string(li.env["ObjectGroups"]), std::back_inserter(li.lists));
		for_each(seq(li.lists), misc::apply_to(this, &Reaper::load_objectgroup));
		return li;
	}

	Renderer* renderer() { return this; }

	ObjectID get_obj_at_pos(Point p) {
		world::si_iterator i = wr->find_si(world::Sphere(p, 1));
		if (i == wr->end_si()) {
			derr << "cannot find obj at " << p << '\n';
			return -1;
		} else
			return i->get_id();
	}

	void set_pos(ObjectID id, Point p) {
		derr << "set pos on " << id << " to " << p << '\n';
		obj_info[id].mtx.pos(p);
		world::si_iterator i = wr->find_si(id);
		if (i == wr->end_si())
			derr << "cannot find " << id << '\n';
		else {
			SillyPtr pt = *i;
			wr->erase(pt);
			pt->get_mtx().pos() = p;
			wr->add_object(pt);
		}
	}

	Point get_pos(ObjectID id) {
		derr << "get pos " << id << " " << obj_info[id].mtx.pos() << '\n';
		return obj_info[id].mtx.pos();
	}

	ObjGroupID load_objectgroup(string s) {
		derr << "load_objectgroup " << s << '\n';
		try {
			RealObjectGroup rlst = res::resource<RealObjectGroup>(res::Path("objectgroups", s));
			ObjectGroup lst;
			for (int i = 0; i < rlst.objs.size(); ++i) {
				ObjectID id = rlst.objs[i].id = next_id++;
				printf("load obj %d\n", id);
				lst.push_back(id);
				obj_info[id] = rlst.objs[i];
			}
			obj_groups[s] = lst;
			
		} catch (error_base& e) {
			printf("Err (load_objectgroup): %s\n", e.what());
		}
		derr << "objectgroup " << s << " loaded\n";
		return s;
	}

	ObjectGroup get_group(ObjGroupID id) {
		derr << "get_group " << id << '\n';
		return obj_groups[id];
	}

	void add_to_group(ObjGroupID lst, ObjectID id) {
		derr << "add_to_group " << lst << ' ' << id << '\n';
		obj_groups[lst].push_back(id);
		SillyPtr p = factory::inst().create(obj_info[id], TMap<SillyBase>());
		wr->add_object(p);
	}

	void del_from_group(ObjGroupID lst, ObjectID id) {
		derr << "del_to_group " << lst << ' ' << id << '\n';
		misc::erase(obj_groups[lst], id);
	}

	void save_group(ObjGroupID lst) {
		derr << "save_group " << lst << '\n';
		const ObjectGroup& og = obj_groups[lst];
		RealObjectGroup& rl = res::resource<RealObjectGroup>(res::Path("objectgroups", lst));
		rl.objs.clear();
		for (int i = 0; i < og.size(); i++) {
			rl.objs.push_back(obj_info[og[i]]);
			printf("save %d %d\n", obj_info[i].id, rl.objs.back().id);
		}
		res::save<RealObjectGroup>(res::Path("objectgroups", lst));
	}

	// Renderer

	void init() {
		try {
			rr.destroy();
			wr->clear();
			if (mode == Object) {
				wr->load("empty_level");
				rr = gfx::Renderer::create();
				rr->settings().draw_sky = false;
				rr->settings().draw_water = false;
				rr->settings().use_fog = false;
				rr->settings().draw_shadows = false;
			} else {
				wr->load(cur_level);
				rr = gfx::Renderer::create();
				rr->settings().draw_shadows = false;
			}
			rr->load();
			rr->start();
			if (mode == Object) {
				SillyPtr p = factory::inst().create(
					MkInfo(cur_object, Matrix(Point(0, 0, 0))), TMap<SillyBase>());
				wr->add_object(p);
			}
		} catch (error_base& e) {
			errmsg = e.what();
			printf("ERROR! %s\n", e.what());
			mode = NotInit;
		}
	}

	void reinit() {
		is_init = false;
	}

	void insert_group(ObjGroupID id) {
		derr << "insert_group " << id << '\n';
		ObjectGroup& ol = obj_groups[id];
		for (int i = 0; i < ol.size()-1; ++i) {
			SillyPtr p = factory::inst().create(obj_info[ol[i]], TMap<SillyBase>());
			wr->add_object(p);
		}
	}

	void update_olists() {
		if (mode == Object)
			return;
		wr->clear(true);
		derr << "update_olists " << mode << " " << cur_group << " " << obj_groups.size() << '\n';
		if (mode == Level) {
			insert_group(cur_group);
		} else if (mode == LevelAll) {
			map<string, ObjectGroup>::iterator c, e = obj_groups.end();
			for (c = obj_groups.begin(); c != e; ++c) {
				insert_group(c->first);
			}
		}
	}


	void render() {
		derr << "Render in mode: " << mode << '\n';
		if (mode == NotInit)
			return;
		if (!is_init) {
			init();
			is_init = true;
		}
		if (dirty) {
			update_olists();
			dirty = false;
		}
		int vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		gfx::Camera cam(cam_pos, cam_tgt, Vector(0,1,0), 100, (100.0 * vp[3]) / vp[2]);
		glClear(GL_COLOR_BUFFER_BIT);
		rr->render(cam);
		switch (mode) {
		case Renderer::Object:
			break;
		case Renderer::LevelAll:
		case Renderer::Level: {
		      glColor3f(1,1,1);
		      glDisable(GL_TEXTURE_2D);
		      glBegin(GL_LINES);
		      float x = cam_tgt.x, y = cam_tgt.y, z = cam_tgt.z;
		      glVertex3f(x-50, y, z);
		      glVertex3f(x+50, y, z);
		      glVertex3f(x, y-50, z);
		      glVertex3f(x, y+50, z);
		      glVertex3f(x, y, z-50);
		      glVertex3f(x, y, z+50);
		      float a = get_altitude(cam_tgt);
		      printf("alt %f\n", a);
		      glVertex3f(x-30, a, z-30);
		      glVertex3f(x+30, a, z+30);
		      glVertex3f(x-30, a, z+30);
		      glVertex3f(x+30, a, z-30);
		      glEnd();
		      glEnable(GL_TEXTURE_2D);
		     }
		     break;
		case Renderer::NotInit:
		     glClearColor(0,0,0,0);
		     glClear(GL_COLOR_BUFFER_BIT);
		     break;
		}

	}

	void set_camera(const Point& eye, const Point& tgt) {
		cam_pos = eye;
		cam_tgt = tgt;
	}

	Point get_cam_pos() { return cam_pos; }
	Point get_tgt_pos() { return cam_tgt; }

	void set_mode(Mode m) { mode = m; dirty = true; }
	Mode get_mode() { return mode; }

	void set_cur_object(string o) { cur_object = o; }
	string get_cur_object() { return cur_object; }

	void set_cur_level(string l) { cur_level = l; }
	string get_cur_level() { return cur_level; }


	void clear_objectgroups() {
		obj_groups.clear();
		dirty = true;
	}

	void sel_objectgroup(ObjGroupID id) {
		cur_group = id;
		dirty = true;
	}

	ObjGroupID get_current_group() {
		return cur_group;
	}

	void add_objectgroup(ObjGroupID id) {
		obj_groups[id] = ObjectGroup();
		dirty = true;
	}

	float get_altitude(Point p) {
		float a = wr->get_altitude(Point2D(p.x, p.z));
		printf("get_alt (%f,%f,%f) -> %f\n", p.x, p.y, p.z, a);
		return a;
	}

	string get_errmsg() { return errmsg; }
};


GameIF* get_game_interface()
{
	return new Reaper();
}


}
}


