#include "os/compat.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <functional>

#include "main/types_ops.h"
#include "main/types4_ops.h"

#include "gfx/renderer.h"
#include "gfx/hud.h"
#include "gfx/exceptions.h"
#include "gfx/settings.h"
#include "gfx/interfaces.h"
#include "gfx/camera.h"

#include "misc/sequence.h"
#include "misc/skipiter.h"
#include "misc/stlhelper.h"
#include "gfx/misc.h"

#include "object/accessor.h"
#include "world/world.h"

namespace reaper {
namespace gfx {
namespace lowlevel {

using reaper::misc::time::Profile;

namespace  {	

/// Rendering command compare
class IDcomp {
public:
	inline bool operator()(const IDable *a, const IDable *b) const {
		if(a->get_texture() < b->get_texture()) 
			return true;
		if(b->get_texture() < a->get_texture()) 
			return false;

		return a->get_id() < b->get_id();
	}
};

class RenderInfoComp {
public:
	inline bool operator()(const RenderInfo *a, const RenderInfo *b) const {
		if(a->texture < b->texture) 
			return true;
		if(b->texture < a->texture) 
			return false;

		return a->mesh < b->mesh;
	}
};

using namespace reaper::misc;
using namespace world;

inline Seq<tri_iterator> wseq(const tri_iterator &i) {
	return Seq<tri_iterator>(i, WorldRef()->end_tri()); 
}
inline Seq<si_iterator> wseq(const si_iterator &i) { 
	return Seq<si_iterator>(i, WorldRef()->end_si()); 
}
inline Seq<st_iterator> wseq(const st_iterator &i) { 
	return Seq<st_iterator>(i, WorldRef()->end_st()); 
}
inline Seq<dyn_iterator> wseq(const dyn_iterator &i) { 
	return Seq<dyn_iterator>(i, WorldRef()->end_dyn()); 
}
inline Seq<shot_iterator> wseq(const shot_iterator &i) {
	return Seq<shot_iterator>(i, WorldRef()->end_shot()); 
}

} // end anonymous namespace

void Renderer::render(const Camera &cam)
{
	AssertGL agl("IntRndr::render() - entering");

	stats.reset();

	if(!envmapper.get()) {
		envmapper = std::auto_ptr<EnvMapper>(new EnvMapper);
	}

	if(Settings::current.use_fog) {	glEnable(GL_FOG); }	

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	gen_envmap();
	frustum = cam.setup_view();
	{ Profile p(stats[Clear]); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	{ Profile p(stats[Sky_prf]); sky.render(cam); }
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	{ Profile p(stats[Terrain_prf]); terrain.render(cam); }
	
	render_objects();

	glDepthMask(GL_FALSE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	
	// shadows/lights are drawn only for half of current frustm
	world::Frustum short_frustum(frustum.pos(), frustum.dir() / 2, frustum.up(), 
	                             frustum.fov_width(), frustum.fov_height());	

	{ Profile p(stats[Shadow]); render_shadows(short_frustum); }
	{ Profile p(stats[Light]); lr->render(short_frustum); }

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_CULL_FACE);
	
	{ Profile p(stats[Water]); render_water(); }
	{ Profile p(stats[EffectRender]); render_effects(); }

	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);

	if(Settings::current.use_fog) {	glDisable(GL_FOG); }
	
	{ Profile p(stats[HUDprf]);
	  stats.render();
	  render_hud();
	}

	glDisable(GL_ALPHA_TEST);	
	glDisable(GL_BLEND);
}	

namespace {

template<typename InIt>
struct Gather
{
	InIt mi, bmi;
	int cnt;
	
	Gather(const InIt& m, const InIt& bm) : mi(m), bmi(bm), cnt(0) {}

	template<typename T>
	void operator()(const T &i)
	{
		const RenderInfo* ri = i->render(true);
		for(RenderInfo::iterator rii = ri->begin(); rii != ri->end(); ++rii) {
			(rii->blend ? bmi : mi) = &(*rii);
		}
		cnt++;
	}
};

template<typename S, typename InIt>
inline int gather_meshes(const InIt& mi, const InIt& bmi, const S& seq)
{
	Gather<InIt> g(mi, bmi);
	for_each(seq, g);
	return g.cnt;
}


struct Refs
{
	TextureRef tr;
	MeshRef mr; 

	Refs() : tr(TextureMgr::get_ref()), mr(MeshMgr::get_ref()) {}
};

struct Render : protected Refs
{
	void operator()(const RenderInfo *i) {
		glPushMatrix();
		glMultMatrix(*i->mtx);	
		tr->use(i->texture);
		mr->render(*i, 0);
		glPopMatrix();
	}		
};

class RenderLight : public Render
{
	LightRef lr;
public:
	RenderLight() : lr(LightMgr::get_ref()) {}

	void operator()(const RenderInfo *i) {
		lr->setup_locals(i->mtx->pos(), i->radius);
		Render::operator()(i);
	}		
};

// std::equal_to<> takes only one type
template <typename A, typename B>
struct Compare : public std::binary_function<A, B, bool>
{
	bool operator()(const A &a, const B &b) const {
		return a == b;
	}
};

typedef Compare<object::DynamicPtr, object::DynamicPtr> ComparePtr;

template<typename C, typename R>
inline void render_meshes(C &meshes, C &blend_meshes, const R &rend)
{
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);		
	for_each(seq(meshes), rend);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);		
	for_each(seq(blend_meshes), rend);
}

} // end anonymous namespace

void Renderer::render_objects()
{
	AssertGL agl("IntRndr::render_objects()");

	using namespace std;
	using namespace world;
	using namespace reaper::misc;

	const bool env_map = (Settings::current.environment_mapping != 0) &&
	                      Settings::current.hud_type != Internal_HUD;

	back_insert_iterator<MeshCont> mi(meshes);
	back_insert_iterator<MeshCont> bmi(blend_meshes);

	object::DynamicPtr player = wr->get_local_player();
	// determine visible meshes
	{
	Profile p(stats[ObjectPrepare]);

	stats[Shots] = gather_meshes(mi, bmi, wseq(wr->find_shot(frustum)));
	stats[Sillys] = gather_meshes(mi, bmi, wseq(wr->find_si(frustum)));
	stats[Statics] = gather_meshes(mi, bmi, wseq(wr->find_st(frustum)));
	
	if (player.valid() && env_map) {
		stats[Dynamics] = gather_meshes(mi, bmi, skip_seq(wseq(wr->find_dyn(frustum)),
			reaper::misc::bind2nd(ComparePtr(), player)));
	} else {
		stats[Dynamics] = gather_meshes(mi, bmi, wseq(wr->find_dyn(frustum)));
	}

	sort(seq(meshes), RenderInfoComp());
	sort(seq(blend_meshes), RenderInfoComp());
	}

	// render visible meshes
	Profile p(stats[ObjectRender]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(Settings::current.use_lighting) {
		lr->setup_global();
		glEnable(GL_LIGHTING);
		render_meshes(meshes, blend_meshes, RenderLight());
	} else {
		glColor3f(1,1,1);
		render_meshes(meshes, blend_meshes, Render());
	}

	meshes.clear();
	blend_meshes.clear(); 

	if (player.valid() && env_map) {
		AssertGL agl("IntRndr - envmap render");

		Gather<back_insert_iterator<MeshCont> > g(mi, bmi);
		g(wr->get_local_player());

		glActiveTextureARB(GL_TEXTURE1_ARB);
		envmapper->setup();
		glMatrixMode(GL_TEXTURE);
		Matrix m;
		m.row(0) = norm(frustum.left());
		m.row(1) = norm(-frustum.up());
		m.row(2) = norm(-frustum.dir());
		m.pos() = Point(0,0,0);
		glLoadMatrix(fake_inverse3(m));
		glMatrixMode(GL_MODELVIEW);
		glActiveTextureARB(GL_TEXTURE0_ARB);		

		if(Settings::current.use_lighting) {
			render_meshes(meshes, blend_meshes, RenderLight());
		} else {
			render_meshes(meshes, blend_meshes, Render());
		}

		glActiveTextureARB(GL_TEXTURE1_ARB);
		envmapper->restore();
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glActiveTextureARB(GL_TEXTURE0_ARB);

		meshes.clear();
		blend_meshes.clear(); 
	}

	if(Settings::current.use_lighting) {
		glDisable(GL_LIGHTING);	
	}
}

void Renderer::gen_envmap()
{
	if(Settings::current.environment_mapping != 0 &&
	   Settings::current.hud_type != Internal_HUD) {
		AssertGL agl("IntRndr - envmap generate");		
		Profile p(stats[Cubemap]);

		object::DynamicPtr ptr = wr->get_local_player();
		if (ptr.valid())
			envmapper->generate(ptr->stat().pos(), frustum, sky, terrain);
	}
}

void Renderer::render_effects()
{
	using namespace reaper::misc;

	if(!Settings::current.draw_effects) {	
		temp_effects.clear();
		return;
	}

	AssertGL agl("IntRndr::render_effects()");

	simul_effects.update();
	orphan_effects.update();
	
	// gather all effects in one container
	std::back_insert_iterator<EffectCont> ii(temp_effects);		

	std::copy(simul_effects.find(frustum),  simul_effects.end(),  ii);
	std::copy(orphan_effects.find(frustum), orphan_effects.end(), ii);
	sort(seq(temp_effects), IDcomp());
	
	typedef EffectCont::value_type     value_type;
	typedef EffectCont::const_iterator iterator;

	const iterator end = temp_effects.end();
	iterator i = temp_effects.begin(); 

	while(i != end) {
		bool has_rendered    = false;
		const value_type e   = *i;
		const iterator upper = std::upper_bound(i, end, e, IDcomp());

		do {
			const value_type e = *i;
			if(!has_rendered) {
				e->setup();
				has_rendered = true;
			}
			e->render();
			stats[Effects]++;
			++i;
		} while(i != upper);

		if(has_rendered) {
			e->restore();
		}
	}
	temp_effects.clear();
}


void Renderer::render_water()
{
	AssertGL agl("IntRndr::render_water()");

	if(!Settings::current.draw_water) 
		return;

	for(LakeCont::const_iterator i = lakes.begin(); i != lakes.end(); ++i) {
		(*i)->render(frustum);
	}
}


void Renderer::render_hud()
{
	AssertGL agl("IntRndr::render_hud()");

	if (*HUD::hud_type == None_HUD) 
		return;

	/*
	 * This should be done a bit more cunning...
	 * 1. Keep track of current id, the game engine
	 *    should provide it somehow.
	 * 2. Remove "dead" huds.
	 */
	object::DynamicPtr pb = wr->get_local_player();
	if (pb != 0) {
		object::ID cur = pb->stat().id();
		if (huds.find(cur) != huds.end())
			huds[cur]->render();
	}
}

void Renderer::render_shadows(const world::Frustum &frustum)
{		
	static int shadow_type = 0;

	if(Settings::current.texture_level == 0 || !Settings::current.draw_shadows) {
		return; 
	}

	AssertGL agl("IntRnr::render_shadows");
	
	if((shadow.get() == 0 && shadow_type > 0) ||
	    shadow_type != Settings::current.shadow_type ) {
		shadow_type = Settings::current.shadow_type;
		shadow = std::auto_ptr<ShadowRenderer>(ShadowRenderer::get(shadow_type));
		if(shadow.get() == 0) {
			stats[Shadows] = 0;
		}
	}

	if(shadow.get()) {
		stats[Shadows] = shadow->render(frustum);
	}
}
}
}
}

/*
 * $Author: pstrand $
 * $Date: 2003/06/11 15:16:41 $
 * $Log: gfx_render.cpp,v $
 * Revision 1.29  2003/06/11 15:16:41  pstrand
 * småfixar..
 *
 * Revision 1.28  2003/06/04 16:29:58  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.27  2003/05/05 08:51:00  pstrand
 * sound changes..
 *
 * Revision 1.26  2003/04/30 13:24:38  pstrand
 * hide dead players..
 *
 * Revision 1.25  2003/01/06 12:42:01  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.24  2002/12/29 17:04:33  pstrand
 * hw/ moved to os/
 *
 * Revision 1.23  2002/09/20 23:46:59  pstrand
 * use_texturing->texture_level, minor event fix.
 *
 * Revision 1.22  2002/06/19 15:01:28  fizzgig
 * *** empty log message ***
 *
 * Revision 1.21  2002/06/06 07:59:38  pstrand
 * gl init, settings, fix
 *
 * Revision 1.20  2002/05/31 14:38:40  pstrand
 * const. ref, fix..
 *
 * Revision 1.19  2002/05/29 00:35:12  fizzgig
 * cleanup, small fixes, improvement, etc
 *
 * Revision 1.18  2002/05/21 03:07:51  fizzgig
 * envmap update options (gfx_renderer::environment_mapping: 0->off 1->incremental 2->instant)
 * preliminary support for shadow volumes (now requests stencil buffer as well)
 *
 * Revision 1.17  2002/05/20 18:48:17  fizzgig
 * vc++ bind2nd fix
 *
 * Revision 1.16  2002/05/20 09:59:58  fizzgig
 * dynamic environment mapping!
 *
 * Revision 1.15  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.14  2002/05/10 14:12:50  fizzgig
 * shadow code cleanup
 *
 * Revision 1.13  2002/05/04 18:43:27  pstrand
 * networkfixing..
 *
 * Revision 1.12  2002/04/29 20:33:26  fizzgig
 * Added quadtree for particle-systems, plus shot->silly fix
 *
 * Revision 1.11  2002/03/21 20:17:10  fizzgig
 * minor cosmetics
 *
 * Revision 1.10  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.9  2002/02/21 11:29:47  picon
 * ''
 *
 * Revision 1.8  2002/02/18 11:52:37  peter
 * *** empty log message ***
 *
 * Revision 1.7  2002/02/11 11:12:29  macke
 * shadows für alle .. dynamics.. :)
 *
 * Revision 1.6  2002/02/10 13:58:39  macke
 * cleanup
 *
 * Revision 1.5  2002/02/08 11:27:50  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.4  2002/02/07 00:02:50  macke
 * STLPort 4.5.1 Win32-fix + shadow & render reorg
 *
 * Revision 1.3  2002/01/30 23:25:23  macke
 * do'h..
 *
 * Revision 1.2  2002/01/28 01:41:38  macke
 * bläurgh
 *
 * Revision 1.1  2002/01/28 00:50:39  macke
 * gfx reorg
 *
 */
