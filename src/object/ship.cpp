

#include "os/compat.h"

#include <string>

#include "object/base.h"
#include "object/factory.h"
#include "object/hull.h"
#include "object/ai.h"
#include "object/player_ai.h"
#include "object/sound.h"
#include "object/collide.h"
#include "object/weapon.h"
#include "object/current_data.h"
#include "object/phys.h"
#include "object/phys_data.h"
#include "object/renderer.h"
#include "object/controls.h"
#include "object/object_impl.h"
#include "object/sync.h"
#include "res/config.h"
#include "main/types_io.h"
#include "msg/event.h"
#include "net/sync.h"

#include "object/gen_ship.h"
#include "object/object.h"

namespace reaper {
namespace object {



class ShipFighter : public GenShip
{
	std::auto_ptr<ai::ShipFighter> ai;
public:
	ShipFighter(MkInfo mk)
	 : GenShip(mk),
	   ai(new ai::ShipFighter(mk.cid, acc, ctrl))
	{ }
	void initialize() {
		events().remove.add_listener( reg(ai.get()) );
		GenShip::initialize();
	}
};

class ShipBomber : public GenShip
{
	std::auto_ptr<ai::ShipBomber> ai;
public:
	ShipBomber(MkInfo mk)
	 : GenShip(mk),
	   ai(new ai::ShipBomber(mk.cid, acc, ctrl))
	{ }
	void initialize() {
		events().remove.add_listener( reg(ai.get()) );
		GenShip::initialize();
	}
};

class ShipPlayer : public GenShip
{
	std::auto_ptr<ai::PlayerControl> ai;
public:
	ShipPlayer(MkInfo mk)
	 : GenShip(mk),
	   ai(new ai::PlayerControl(ctrl, mk.id))
	{ }
	void initialize()
	{
		net::Sync()->add(data.get_id(),
				new object::sync::GenericSync(acc));
		events().remove.add_listener( reg(ai.get()) );
		GenShip::initialize();
	}
};



SillyBase* ship(MkInfo mk)
{
	char t = (mk.name.size() == 5) ? 'n' : mk.name[5];
	std::string name = mk.name.substr(0, 5);
	MkInfo mk2(name, mk.mtx, mk.id, mk.cid, factory::inst().info(mk.name, name), mk.cdata);
	switch (t) {
	case 'n': return new ShipFighter(mk2);
	case 'b': return new ShipBomber(mk2);
	case 'P': return new ShipPlayer(mk2);
	default: return 0;
	}
}


namespace {
struct Foo {
	Foo() {
		factory::inst().register_object("ship2", ship);
		factory::inst().register_object("ship3", ship);
		factory::inst().register_object("ship4", ship);
		factory::inst().register_object("shipX", ship);

		factory::inst().register_object("ship2b", ship);
		factory::inst().register_object("ship3b", ship);
		factory::inst().register_object("ship4b", ship);
		factory::inst().register_object("shipXb", ship);

		factory::inst().register_object("ship2P", ship);
		factory::inst().register_object("ship3P", ship);
		factory::inst().register_object("ship4P", ship);
		factory::inst().register_object("shipXP", ship);

	}
} bar;
}

void ship() { }

}
}

