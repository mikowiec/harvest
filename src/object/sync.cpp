
#include "os/compat.h"

#include "os/debug.h"

#include "main/types_ops.h"
#include "main/types_io.h"
#include "misc/parse.h"
#include "misc/iostream_helper.h"

#include "object/sync.h"

#include <sstream>

namespace reaper {
namespace object {
namespace sync {

namespace { debug::DebugOutput derr("object::sync"); }

using namespace misc;

GenericState::GenericState(GenericSync* s, const Matrix& m, const Vector& v,
			   const Vector& rv, bool d)
 : sync(s), mtx(m), vel(v), rot_vel(rv), is_dead(d)
{ }

void diff(Matrix& d, const Matrix& m1, const Matrix& m2)
{	
	d.col(0, m1.col(0) - m2.col(0));
	d.col(1, m1.col(1) - m2.col(1));
	d.col(2, m1.col(2) - m2.col(2));
	d.pos() = m1.pos() - m2.pos();
}

bool diff_to_big(const Matrix& m)
{
	Vector c_x = m.col(0);
	Vector c_y = m.col(1);
	Vector c_z = m.col(2);
	Point p = m.pos();
	return dot(c_x, c_x) > 0.5
	    || dot(c_y, c_y) > 0.5
	    || dot(c_z, c_z) > 0.5
	    || dot(p, p) > 100;
}

bool diff_to_big(const Vector& v)
{
	return dot(v, v) > 10; 
}

void GenericState::merge(const std::string& str)
{
	std::istringstream is(str);
	Matrix oth_mtx;
	Vector oth_vel, oth_rot_vel;
	bool oth_is_dead;
	is >> oth_mtx >> spaces >> oth_vel >> spaces
	   >> oth_rot_vel >> spaces >> oth_is_dead;

	Matrix dm;
	diff(dm, mtx, oth_mtx);
	Vector dvv = vel - oth_vel;
	Vector dvr = rot_vel - oth_rot_vel;
	if (oth_is_dead)
		sync->dead = true;
	if (diff_to_big(dm)) {
		derr << "too big diff, forcing mtx: " << dm << "\n";
		sync->acc->set_mtx(oth_mtx);
	} else {
		Matrix m(sync->acc->mtx());
		m += dm;
		orthonormalize3(m);
		sync->acc->set_mtx(m);
	}

	Vector nvel = sync->acc->velocity();
	nvel = diff_to_big(dvv) ? oth_vel : Vector(nvel + dvv);
	sync->acc->set_velocity(nvel);

	Vector nrot_vel = sync->acc->rot_velocity();
	nrot_vel = diff_to_big(dvr) ? oth_rot_vel : Vector(nrot_vel + dvr);
	sync->acc->set_rot_velocity(nrot_vel);

}

std::string GenericState::serialize()
{
	std::ostringstream os;
	os << mtx << ' ' << vel << ' '
	   << rot_vel << ' ' << is_dead;
	return os.str();
}

GenericSync::GenericSync(BasicDyn* a)
 : acc(a), dead(false)
{

}

net::State* GenericSync::get_current_state()
{
	return new GenericState(this, acc->mtx(), acc->velocity(),
			        acc->rot_velocity(), dead);
}

void GenericSync::kill()
{
	dead = true;
}

bool GenericSync::is_dead()
{
	return dead;
}


}
}
}

