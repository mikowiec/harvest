#ifndef REAPER_OBJECT_GEN_SHIP_H
#define REAPER_OBJECT_GEN_SHIP_H


#include "object/base.h"
#include "object/base_data.h"
#include "object/controls.h"
#include "object/hull.h"
#include "object/renderer.h"
#include "object/ai.h"
#include "object/player_ai.h"
#include "object/phys.h"
#include "object/current_data.h"
#include "object/factory.h"
#include "object/reg.h"
#include "object/accessor.h"
#include "object/object_impl.h"
#include "object/accessor.h"

#include "main/types_param.h"

#include <vector>


namespace reaper {
namespace object {

template<class B, class C>
void set_weapon(misc::Switch* sw, B& b, C c)
{
	sw->set(misc::mk_cmd(b, &weapon::Weapon::fire, c));
}




class GenShip : public ObjImpl<PlayerBase>,
	        public Simulate,
		public Collide
{
protected:
	BasicDyn* acc;
public:
	ShipInfo phys_data;
protected:
	current_data::Ship current;
        hull::Shielded hull;
	phys::Ship phys;
	renderer::Ship rd;
	controls::Ship ctrl;
	sound::Ship* snd;

	weapon::Weapon* laser;
	weapon::Missiles* missiles;

	DynPhysWrap<ShipInfo, reaper::phys::DynAccessor>* phys_acc;

public:
	GenShip(MkInfo);
	~GenShip();

	void initialize();

	void collide(CollisionInfo cI);
	const gfx::RenderInfo* render(bool effects) const;
	void simulate(double dt);

	/* DynamicBase */
	const GetDyn& dyn() const { return *acc; }
};

}
}

#endif

