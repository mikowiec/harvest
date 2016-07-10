#ifndef REAPER_GFX_RENDERER_H
#define REAPER_GFX_RENDERER_H

#include <memory>
#include <set>
#include <deque>

#include "gfx/gfx.h"
#include "gfx/abstracts.h"
#include "gfx/managers.h"
#include "gfx/statistics.h"
#include "gfx/interfaces.h"

#include "object/id.h"

#include "world/worldref.h"
#include "world/query_obj.h"
#include "world/quadtree.h"

#include "misc/sequence.h"

namespace reaper {
namespace game { class LevelInfo; }
namespace gfx {
namespace pm { class Pmd; }
class Lake;
class Camera;
class ShadowRenderer;

namespace lowlevel {

/// Internal renderer data
class Renderer
{
public: 
        Renderer(const game::LevelInfo &li);
	~Renderer();
	void start();

	void simulate(float dt);
	void render(const Camera &);

	inline void draw(Effect *e);
	inline void insert(SimEffect *e);
	inline void insert_orphan(SimEffect *e); 
	inline void transfer(SimEffect *e);
	inline void remove(SimEffect *e);

	void add_hud(object::ID, Hud*);

	pm::Pmd* get_pmd();

	inline int simeff_size();
	inline int orpeff_size();

	typedef std::deque<Lake*>    LakeCont;
	typedef std::deque<Effect*>  EffectCont;
	typedef quadtree::QuadTree<SimEffect*> SimEffectCont;
	typedef std::deque<const RenderInfo*> MeshCont;

	//-----------------------------------------------------------------
private:	
	world::WorldRef wr;
	TextureRef      tr;
	MeshRef         mr;
	LightRef        lr;

	Terrain  terrain;
        Sky      sky;
	LakeCont lakes;

	std::auto_ptr<ShadowRenderer> shadow;
	std::auto_ptr<EnvMapper> envmapper;

	MeshCont      meshes;
	MeshCont      blend_meshes;
	EffectCont    temp_effects;   ///< temporary effects, renders one frame
	SimEffectCont simul_effects;  ///< simulated effects, renders/simulates forever
	SimEffectCont orphan_effects; ///< orphaned efects, renders/simulates until 'death'

	std::map<object::ID, Hud*> huds; ///< Available user displays.

	world::Frustum frustum;       ///< current camera frustum
	Statistics     stats;	

	int simul_effects_size;
	int orphan_effects_size;

	//-----------------------------------------------------------------
	void render_hud();
	void render_water();
	void render_stats();
	void render_objects();
	void render_effects();
	void render_shadows(const world::Frustum &);

	void gen_envmap();

	void simulate_water(float dt);
	void simulate_effects(float dt);
};

inline void Renderer::draw(Effect *e) 
{
	temp_effects.push_back(e); 
}

inline void Renderer::insert(SimEffect *e) 
{
	simul_effects.insert(e); 
	++simul_effects_size;
}

inline void Renderer::insert_orphan(SimEffect *e) 
{
	if (e) {
		orphan_effects.insert(e); 
		++orphan_effects_size;
	}
}

inline void Renderer::transfer(SimEffect *e) 
{
	using namespace reaper::misc;
	if (e) {
		simul_effects.erase(find(seq(simul_effects), e)); 
		orphan_effects.insert(e); 
		--simul_effects_size;
		++orphan_effects_size;
	}
}

inline void Renderer::remove(SimEffect *e) 
{
	using namespace reaper::misc;
	simul_effects.erase(find(seq(simul_effects), e)); 
	--simul_effects_size;
}

inline int Renderer::simeff_size() 
{ 
	return simul_effects_size; 
}

inline int Renderer::orpeff_size() 
{ 
	return orphan_effects_size; 
}

}
}
}
#endif

/*
 * $Author: pstrand $
 * $Date: 2003/06/04 16:29:58 $
 *
 */

