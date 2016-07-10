



#include "os/compat.h"


#include "misc/plugin.h"
#include "world/world.h"
#include "os/time.h"

extern "C" {
#include <caml/mlvalues.h>
#include <caml/callback.h>
#include <caml/alloc.h>
}


namespace reaper {

using world::WorldRef;
using os::time::TimeSpan;


struct WorldIF {
	virtual WorldRef world() = 0;
};

struct TimeIF {
	virtual TimeSpan get_time() = 0;
};

struct Game
 : public WorldIF, public TimeIF
{ };

class AI {
public:
	virtual const Point& get_pos() = 0;
	virtual void think() = 0;
};

class AIPlugin
 : public AI
{
	Game* game;
	Point pts;
	AIPlugin(Game* g) : game(g), pts(1,2,3) { }
public:
	static AIPlugin* create(Game* g) { return new AIPlugin(g); }
	const Point& get_pos() { return pts; }
	void think() { pts += Point(4,3,2); }
};

extern "C"
value hw_time_get_time(value) {
	os::time::TimeSpan ts = os::time::get_time();
	long u = ts.approx().upper();
	long l = ts.approx().lower();
	return copy_int64((long long)u * os::time::low_mod_us + l);
}


class Extension
 : public AI
{
	value* cm_create;
	value* cm_get_pos;
	value* cm_think;
	value me;
	Game* game;
	Point pos;
public:
	Extension(Game* g)
	 : game(g)
	{
		char* argv[] = { "test_plugin_ai0", 0 };
		caml_startup(argv);
		cm_create = caml_named_value("create");
		cm_get_pos = caml_named_value("get_pos");
		cm_think = caml_named_value("think");
		me = callback(*cm_create, Val_unit);
	}
	const Point& get_pos()
	{
		value pts = callback(*cm_get_pos, me);
		pos = Point(Double_field(pts, 0),
			    Double_field(pts, 1),
			    Double_field(pts, 2));
		return pos;
	}
	void think()
	{
		callback(*cm_think, me);
	}
	
};


}


extern "C" {

typedef reaper::AI* (*createAI)(reaper::Game*);

void* create_test_plugin_ai0(reaper::Game* g) {
	return new reaper::Extension(g);
}

}



