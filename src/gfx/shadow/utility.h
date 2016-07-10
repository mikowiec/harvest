#ifndef REAPER_GFX_SHADOW_UTILITY_H
#define REAPER_GFX_SHADOW_UTILITY_H

#include "gfx/shadow/types.h"
#include "gfx/shadow/main.h"

#include "object/accessor.h"

#include <algorithm>

namespace reaper {
namespace gfx {
namespace shadow {

using object::SillyBase;
using namespace world;

void render_shadow(const SillyBase &o);	

template<typename ItP, typename ItT, typename ItC>
class GenTriVertices
{
	const ShadowData &sd;
	const float inv_d, x, z;
	ItP pi;
	ItT ti;
	ItC ci;
public:
	GenTriVertices(const ShadowData &s, ItP p, ItT t, ItC c) : 
		sd(s), inv_d(1.0f / sd.diameter),
		x(sd.p.x - sd.radius), z(sd.p.z - sd.radius),
		pi(p), ti(t), ci(c) 
	{}

	template<typename T>
	void operator()(T *t) 
	{
		pi = t->a;
		pi = t->b;
		pi = t->c;

		ti = TexCoord((t->a.x - x)*inv_d, (t->a.z - z)*inv_d);
		ti = TexCoord((t->b.x - x)*inv_d, (t->b.z - z)*inv_d);
		ti = TexCoord((t->c.x - x)*inv_d, (t->c.z - z)*inv_d);

		ci = sd.color;
		ci = sd.color;
		ci = sd.color;
	}
};


template<typename ItP, typename ItT, typename ItC>
inline GenTriVertices<ItP, ItT, ItC> vert_generator(const ShadowData &s, ItP p, ItT t, ItC c)
{
	return GenTriVertices<ItP, ItT, ItC>(s,p,t,c);
}

/////////////////////////////////////////////////////////////////////////////////

template<typename ItP, typename ItT, typename ItC>
bool gen_shadow_triangles_if(const SillyBase &o, const world::Frustum &frustum, float radius_mult,
			       ItP pi, ItT ti, ItC ci)
{
	const ShadowData sd(o, radius_mult);
	bool visible = sd.visible(frustum);
	if(visible) {
		WorldRef wr;
		std::for_each(wr->find_tri(sd.shadow_vol), wr->end_tri(), vert_generator(sd,pi,ti,ci));
	}
	return visible;
}


namespace {
/// remove shadows older than threshold value (silly_shadow_lifetime)
template<typename Cont>
void purge_old_shadows(Cont &shadows, const os::time::RelTime &now)
{
	typedef typename Cont::iterator iterator;
        const int lifetime = SilhouetteShadowAll::silly_shadow_lifetime;

	for(iterator i = shadows.begin(); i != shadows.end();) {
                if((now - i->second->last_used) > lifetime) {			
			delete i->second;
			iterator j = i++;
			shadows.erase(j);
		} else {
			++i;
		}
	}
}

} // end anonymous namespace

template<typename It, typename IIt, typename Cont>
void gen_silly_shadows(It i, const It &end, IIt render_ii, 
		       Cont &shadows, const world::Frustum &frustum)
{
	using namespace os::time;
	typedef typename Cont::iterator iterator;
	const RelTime now   = os::time::get_rel_time();
	bool purge_attempted = false; 

	while(i != end) {
		iterator si = shadows.find((*i)->stat().id());

		if(si == shadows.end()) {
			ShadowData sd(*(*i).get(), 1.1f);

			if(sd.visible(frustum)) {
				if(shadows.size() >= SilhouetteShadowAll::max_static_shadows &&
				   !purge_attempted) {
					// if shadow buffer is full, try to purge it
					purge_attempted = true;
					purge_old_shadows(shadows, now);
					if(shadows.size() >= SilhouetteShadowAll::max_static_shadows) {
						++i;
						continue; // still full, then give up with this shadow
					}
				}

				SillyShadow* shadow = new SillyShadow(sd, SilhouetteShadowAll::silly_shadow_size);
				render_shadow(*(*i).get());
				shadow->tex.copy_to_texture();
				si = shadows.insert(std::make_pair((*i)->stat().id(), shadow)).first;			
				render_ii = si->second;
			}
		} 
		else if(si->second->visible(frustum)) {
			si->second->last_used = now;
			render_ii = si->second;
		}
		++i;
	}
}

}
}
}

#endif
