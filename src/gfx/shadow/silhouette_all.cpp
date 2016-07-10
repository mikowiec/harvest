#include "os/compat.h"

#include "misc/sequence.h"
#include "gfx/shadow/main.h"
#include "gfx/shadow/utility.h"

namespace reaper {
namespace gfx {
namespace shadow {

const int SilhouetteShadowAll::max_static_shadows = 5000;
const int SilhouetteShadowAll::silly_shadow_lifetime = 5000000;
const int SilhouetteShadowAll::silly_shadow_size = 128;

SilhouetteShadowAll::~SilhouetteShadowAll() 
{
	using namespace reaper::misc;
	for_each(seq(shadows), delete_it);
}

int SilhouetteShadowAll::render(const Frustum &frustum)
{
	using namespace reaper::misc;
	using namespace std;

	set_renderstate();
	int n_dyn_shadows = gen_dyn_shadows(frustum);

	ShadowVolumeType sv = calc_shadow_volume(frustum);
	deque<SillyShadow*> st_shadows;
	
	texture::setup_viewport(silly_shadow_size,silly_shadow_size);
	glClear(GL_COLOR_BUFFER_BIT);
	gen_silly_shadows(wr->find_si(sv), wr->end_si(), back_inserter(st_shadows), shadows, frustum);
	gen_silly_shadows(wr->find_st(sv), wr->end_st(), back_inserter(st_shadows), shadows, frustum);
	texture::restore_viewport();

	restore_renderstate();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
	ClientStateKeeper s1(GL_COLOR_ARRAY, true);
	ClientStateKeeper s2(GL_TEXTURE_COORD_ARRAY, true);

	render_dyn_shadows(n_dyn_shadows);
	for_each(seq(st_shadows), mem_fun(&SillyShadow::render));
	return n_dyn_shadows + st_shadows.size();
}

}
}
}

