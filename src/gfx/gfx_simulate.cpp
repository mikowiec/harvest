#include "os/compat.h"

#include "gfx/renderer.h"
#include "gfx/interfaces.h"
#include "misc/stlhelper.h"
#include "misc/sequence.h"

namespace reaper {
namespace gfx {
namespace lowlevel {

using namespace reaper::misc;
using namespace std;

namespace {

template<typename T, typename It>
class collect_dead_t
{
	It i;
	float dt;
public:
	collect_dead_t(It i_, float dt_) : i(i_), dt(dt_) {}
	void operator()(T *t) {
		if(!t->simulate_death(dt)) {
			i = t;
		}
	}
};

template<typename T, typename C>
class remove_dead_t
{
	C &c;
public:
	remove_dead_t(C &c_) : c(c_) {}
	void operator()(T *t) {
		using namespace reaper::misc;
		c.erase(find(seq(c),t));
		delete t;
	}
};

template<typename It>
collect_dead_t<SimEffect, It> collect_dead(It i, float dt) {
	return collect_dead_t<SimEffect, It>(i, dt);
}

template<typename C>
remove_dead_t<SimEffect, C> remove_dead(C &c) {
	return remove_dead_t<SimEffect, C>(c);
}

} // end anonymous namespace

void Renderer::simulate(float dt)
{
	reaper::misc::time::Profile p(stats[Simul]);
	deque<SimEffect*> eff;

	for_each(seq(lakes),          apply_val(&Lake::simulate, dt));
	for_each(seq(simul_effects),  apply_val(&SimEffect::simulate, dt));
	for_each(seq(orphan_effects), collect_dead(back_inserter(eff), dt));
	for_each(seq(eff),            remove_dead(orphan_effects));
	sky.simulate(dt);	

	orphan_effects_size -= eff.size();
}

}
}
}

/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:01 $
 * $Log: gfx_simulate.cpp,v $
 * Revision 1.6  2003/01/06 12:42:01  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.5  2002/12/29 17:04:33  pstrand
 * hw/ moved to os/
 *
 * Revision 1.4  2002/05/31 14:38:40  pstrand
 * const. ref, fix..
 *
 * Revision 1.3  2002/05/10 14:12:50  fizzgig
 * shadow code cleanup
 *
 * Revision 1.2  2002/04/29 20:33:26  fizzgig
 * Added quadtree for particle-systems, plus shot->silly fix
 *
 * Revision 1.1  2002/01/28 00:50:39  macke
 * gfx reorg
 *
 */
