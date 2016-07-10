#ifndef REAPER_GFX_SHADOW_MAIN_H
#define REAPER_GFX_SHADOW_MAIN_H

#include <map>

#include "main/types.h"
#include "gfx/gfx_types.h"
#include "gfx/interfaces.h"
#include "gfx/managers.h"
#include "world/world.h"
#include "object/base.h"

#include "gfx/shadow/types.h"

namespace reaper {
namespace gfx {
namespace shadow {

using namespace world;
using object::SillyBase;

class SimpleShadow : public ShadowRenderer
{
public:
	SimpleShadow();
	virtual int render(const Frustum &frustum);
protected:
	WorldRef wr;
	TextureRef tr;
	
	bool add_shadow(const SillyBase &o, const Frustum &f, float r = 1.0f);	
	void render_triangles(); 
private:
	std::vector<Point>     points;
	std::vector<TexCoord>  texcoords;
	std::vector<ByteColor> colors;
};

class SimpleShadowDynamic : public SimpleShadow
{
public:
	SimpleShadowDynamic() {}
	virtual int render(const Frustum &frustum);
protected:
	// generates and renders shadows for all dynamic objects
	int render(const Frustum &frustum, bool shadow_local_player);
	typedef Cylinder ShadowVolumeType;
	ShadowVolumeType calc_shadow_volume(const Frustum &frustum);
};

class SilhouetteShadow : public SimpleShadowDynamic
{
public:
	SilhouetteShadow();
	virtual int render(const Frustum &frustum);
protected:
	static const int texture_size;

	void set_renderstate();
	void restore_renderstate();
private:
	DynamicTexture texture;
	Color clearcolor;
};

class SilhouetteShadowDynamic : public SilhouetteShadow
{
public:
	SilhouetteShadowDynamic();
	~SilhouetteShadowDynamic();
	virtual int render(const Frustum &frustum);
protected:
	typedef std::deque<DynamicShadow*> ShadowCont;
	bool generate_shadow(const SillyBase &obj, DynamicShadow &s, const Frustum &f);
	int gen_dyn_shadows(const Frustum &frustum);
	void render_dyn_shadows(int n_shadows);
private:
	static const int max_shadows;
	ShadowCont shadows;		
};

class SilhouetteShadowAll : public SilhouetteShadowDynamic
{
private:
	typedef std::map<object::ID, SillyShadow*> ShadowCont;
	ShadowCont shadows;
public:
	static const int max_static_shadows;   
	static const int silly_shadow_lifetime;    // milliseconds
	static const int silly_shadow_size;

	SilhouetteShadowAll() {}
	~SilhouetteShadowAll();

	virtual int render(const Frustum &frustum);
};

}
}
}

#endif

