
#include "os/compat.h"

#include <string>
#include "misc/plugin.h"
#include "world/world.h"
#include "main/types_io.h"

#include "misc/test_main.h"

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

}

using namespace reaper;

typedef misc::Plugin<AI, Game> AIPlugin;

class GameImpl
 : public Game
{
public:
	WorldRef world() { return world::World::get_ref(); }
	TimeSpan get_time() { return os::time::get_time(); }
};

extern "C" {
typedef reaper::AI* (*createAI)(reaper::Game*);
createAI create_test_plugin_ai0();
}

int test_main()
{
	AIPlugin ai_plugin("test_plugin_ai0");
	GameImpl game;

	AI* ai = ai_plugin.create(&game);

	derr << "old pos: " << ai->get_pos() << '\n';
	ai->think();
	derr << "new pos: " << ai->get_pos() << '\n';
	return 0;
}




