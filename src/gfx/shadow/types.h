#ifndef REAPER_GFX_SHADOW_TYPES_H
#define REAPER_GFX_SHADOW_TYPES_H

#include <vector>
#include "world/query_obj.h"
#include "os/time.h"
#include "object/base.h"
#include "gfx/texture.h"

namespace reaper {
namespace gfx {
namespace shadow {

using std::vector;
using namespace world;
using namespace object;
using gfx::texture::DynamicTexture;

class ShadowData
{
public:
	Point p;
	float radius;
	float diameter;
	Point shadow_pos;
	float alt;
	ByteColor color;
	world::Sphere shadow_vol;

	ShadowData(const SillyBase &o, float radius_mult = 1.0f);
	bool visible(const Frustum &frustum) const;
};

class DynamicShadow
{
public:
	DynamicTexture tex;
	std::vector<Point>     points;
	std::vector<TexCoord>  texcoords;
	std::vector<ByteColor> colors;

	DynamicShadow(int size, bool bw = true);

	int render(); // render shadow on ground
	void clear();  // clear
	void gen_tris(const ShadowData &sd);
};

class SillyShadow : public DynamicShadow, public ShadowData
{
public:
	os::time::RelTime last_used;

	SillyShadow(const ShadowData &sd, int size);
};

}
}
}

#endif

