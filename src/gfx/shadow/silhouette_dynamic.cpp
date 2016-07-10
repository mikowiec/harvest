#include "os/compat.h"

#include <iterator>

#include "gfx/shadow/main.h"
#include "gfx/shadow/utility.h"
#include "misc/sequence.h"

namespace reaper {
namespace gfx {
namespace shadow {

const int SilhouetteShadowDynamic::max_shadows = 10;

SilhouetteShadowDynamic::SilhouetteShadowDynamic() 
{
	for(int i=0; i<max_shadows; i++) {
		shadows.push_back(new DynamicShadow(SilhouetteShadow::texture_size, false));
	}
}

SilhouetteShadowDynamic::~SilhouetteShadowDynamic() 
{
	using namespace reaper::misc;
	for_each(seq(shadows), delete_it);
}

bool SilhouetteShadowDynamic::generate_shadow(const SillyBase &obj, DynamicShadow &s, const Frustum &f)
{
	bool shadow_visible = 
		gen_shadow_triangles_if(obj, f, 1.0f, 
			std::back_inserter(s.points),
			std::back_inserter(s.texcoords),
			std::back_inserter(s.colors));
	if(shadow_visible) {
		render_shadow(obj);
		s.tex.copy_to_texture();
	}
	return shadow_visible;
}

int SilhouetteShadowDynamic::gen_dyn_shadows(const Frustum &frustum)
{
	int n_shadows = 0;
	ShadowVolumeType sv        = calc_shadow_volume(frustum);
	const dyn_iterator dyn_end = wr->end_dyn();

	for(dyn_iterator i = wr->find_dyn(sv); i != dyn_end; ++i) {
		if(generate_shadow(*(*i).get(), *shadows[n_shadows], frustum)) {
			n_shadows++;
			if(n_shadows >= max_shadows) 
				break;
		}
	}

	return n_shadows;
}

void SilhouetteShadowDynamic::render_dyn_shadows(int n_shadows)
{
	for(ShadowCont::iterator i = shadows.begin(); i != shadows.begin() + n_shadows; ++i) {
		(*i)->render();
		(*i)->clear();
	}	
}

int SilhouetteShadowDynamic::render(const Frustum &frustum)
{
	set_renderstate();
	int n_shadows = gen_dyn_shadows(frustum);
	restore_renderstate();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
	ClientStateKeeper s1(GL_COLOR_ARRAY, true);
	ClientStateKeeper s2(GL_TEXTURE_COORD_ARRAY, true);
	render_dyn_shadows(n_shadows);
	return n_shadows;
}

}
}
}

