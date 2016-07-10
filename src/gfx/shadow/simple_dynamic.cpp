#include "os/compat.h"
#include "gfx/shadow/main.h"

namespace reaper {
namespace gfx {
namespace shadow {

int SimpleShadowDynamic::render(const Frustum &frustum)
{ 
	return render(frustum, true); 
} 

SimpleShadowDynamic::ShadowVolumeType SimpleShadowDynamic::calc_shadow_volume(const Frustum &frustum)
{
	return Cylinder(Point2D(frustum.pos().x, frustum.pos().z), 1200);
}

int SimpleShadowDynamic::render(const Frustum &frustum, bool shadow_local_player)
{	
	ShadowVolumeType sv        = calc_shadow_volume(frustum);
	object::DynamicPtr player   = wr->get_local_player();
	const dyn_iterator dyn_end = wr->end_dyn();
	int n_shadows = 0;

	if(shadow_local_player) {
		for(dyn_iterator i = wr->find_dyn(sv); i != dyn_end; ++i)
			if(add_shadow(*(*i).get(), frustum, 0.75f))
				n_shadows++;			
	} else {
		for(dyn_iterator i = wr->find_dyn(sv); i != dyn_end; ++i)
			if((*i) != player && add_shadow(*(*i), frustum, 0.75f))
				n_shadows++;			
	}

	if(n_shadows > 0) {
		tr->use("smoke");
		render_triangles();
	}
	
	return n_shadows;
}

}
}
}

