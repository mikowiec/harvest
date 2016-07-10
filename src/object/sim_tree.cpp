
#include "os/compat.h"

#include "os/debug.h"
#include "os/gl.h"

#include "main/types_io.h"

#include "gfx/misc.h"

#include "object/base.h"
#include "object/hull.h"
#include "object/object_impl.h"

#include "object/sim_plant_mgr.h"


namespace reaper {
namespace object {

debug::DebugOutput derr("sim_tree");

class SimTree : public ObjImpl<StaticBase>
{
	Matrix mtx;
	hull::Standard hull;
	std::auto_ptr<plants::SimPlant> plant;
	double step;
public:
	SimTree(MkInfo);

	void collide(const CollisionInfo& cI);
	const gfx::RenderInfo* render(bool effects) const;
	void simulate(double dt);		
	void think();
//	void receive(const reaper::ai::Message&) { }
};

SimTree::SimTree(MkInfo mk)
 : ObjImpl<StaticBase>(mk.mtx, mk.cid, "sheep", mk.id),
   mtx(mk.mtx),
   hull(data, 1),
   plant(plants::mk_plant()),
   step(0)
{
	derr << "create: " << mtx << '\n';
}

const gfx::RenderInfo* SimTree::render(bool effects) 
{
	glPushMatrix();
	glTranslate(mtx.pos());
	plant->render();
	glPopMatrix();
	return 0;
}

void SimTree::collide(const CollisionInfo& ci)
{
	damage(hull.damage(ci),ci,get_pos());
}

void SimTree::think()
{

}

void SimTree::simulate(double dt)
{
	step += dt;
	if (step > 1000) {
		step -= 1000;
		plant->simulate();
	}
}


SillyBase* sim_tree(MkInfo mk)
{
	return new SimTree(mk);
}


namespace {
struct Foo {
	Foo() {
		factory::inst().register_object("sim_tree", sim_tree);
	}
} bar;
}

void sim_tree() { }

}
}





