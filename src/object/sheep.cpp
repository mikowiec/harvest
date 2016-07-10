
#include "os/compat.h"

#include "main/types_io.h"
#include "main/types_ops.h"
#include "misc/utility.h"
#include "object/base.h"
#include "object/factory.h"
#include "object/hull.h"
#include "object/object_impl.h"
#include "object/phys_acc.h"
#include "object/reg.h"
#include "os/debug.h"
#include "res/config.h"

namespace reaper {
namespace object {
namespace {
	reaper::debug::DebugOutput dout("object::Sheep",0);
}



class Sheep : public DynamicBase, public Simulate, public Collide
{
	Matrix m;
	std::auto_ptr<BasicDyn> acc;
	
	world::WorldRef wr;
	Vector velocity;

	hull::Standard hull;		
	gfx::RenderInfo ri;

	event::Events evts;

public:
	Sheep(const Matrix &mtx, CompanyID c, ID i);

	void initialize();
	event::Events& events() { return evts; }

	const GetStat& stat() const { return *acc; }
	const GetDyn& dyn() const { return *acc; }

	const gfx::RenderInfo* render(bool effects) const {
		return &ri;
	}

	void collide(CollisionInfo cI);
	void simulate(double dt);
};

Sheep::Sheep(const Matrix &mi, CompanyID c, ID i)
 : m(mi), acc(basic_accessor_ext_mtx(i, c, m, 1.1)), 
   hull(1), ri("sheep", m, false)
{
	using reaper::misc::frand;
	velocity = Matrix(frand()*360, Vector(0,1,0)) * Vector(3 + 3*frand(), 0, 0);	
}

struct SheepInfo
{
	float max_acc;
	float max_vel;
	float mass;
	float damage;

	SheepInfo()
	 : max_acc(1), max_vel(1), mass(1), damage(1)
	{ }
};

void Sheep::initialize()
{
	DynamicPtr ptr(this);
	typedef DynPhysWrap<SheepInfo, reaper::phys::DynAccessor> Wrap;
	Wrap* w = new Wrap(acc->id(), acc.get(), SheepInfo());
	w->add_sim(this);
	w->add_col(this);
	init_reg(ptr, w);
}

void Sheep::collide(CollisionInfo ci)
{
	if(hull.damage(ci) == hull::Fatality) {
		events().kill.ping_all();
		gfx::blood_n_guts(acc->pos(),1).insert_release();
	} else {
		gfx::blood_n_guts(acc->pos(),.1).insert_release();
	}
	velocity *= -1;
}

void Sheep::simulate(double dt)
{
	using namespace ::reaper::world;		

	float dt2 = dt;
	while(dt2>.02) {
		velocity = Matrix(dt2*360/50,Vector(0,1,0)) * velocity;
		m.pos().x += velocity.x * dt2;
		m.pos().z += velocity.z * dt2;		
		dt2 -= .02;
	}
	velocity = Matrix(dt2*360/50,Vector(0,1,0)) * velocity;
	m.pos().x += velocity.x * dt2;
	m.pos().z += velocity.z * dt2;		

	m.pos().y = wr->get_altitude(Point2D(m.pos().x, m.pos().z)) + 1.6f; 
}

SillyBase* sheep(MkInfo mk)
{
	return new Sheep(mk.mtx, mk.cid, mk.id);
}



namespace {
	struct Foo {
		Foo() {
			factory::inst().register_object("sheep", sheep);
		}
	} bar;
}

void sheep() { }

}
}
