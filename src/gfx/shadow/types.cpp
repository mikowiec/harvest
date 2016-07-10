#include "os/compat.h"

#include <iterator>

#include "object/base.h"
#include "object/accessor.h"
#include "world/world.h"
#include "gfx/shadow/types.h"
#include "gfx/shadow/utility.h"
#include "world/geometry.h"

using namespace std;

namespace reaper {
namespace gfx {
namespace shadow {

using object::SillyBase;
using namespace reaper::world;

ShadowData::ShadowData(const SillyBase &o, float radius_mult)
{
	WorldRef wr;
	
	p          = o.stat().pos();
	radius     = o.stat().radius() * radius_mult;
	diameter   = radius * 2;
	shadow_pos = Point(p.x, wr->get_altitude(Point2D(p.x, p.z)), p.z);
	alt        = p.y - shadow_pos.y;	
	color      = ByteColor(0, 0, 0, static_cast<int>(255 * (1.0 - alt / (diameter * 20))));
	shadow_vol = Sphere(shadow_pos, radius);			
}

bool ShadowData::visible(const Frustum &frustum) const
{
	return color.a > 0 && intersect(shadow_pos, radius, frustum);
}


DynamicShadow::DynamicShadow(int size, bool bw) : 
	tex(size, size, bw) 
{ 
	points.reserve(3);
	texcoords.reserve(3);
	colors.reserve(3);
}

//FIXME: Stupid VC not allowing 'return void;' in void funcs as needed in mem_fun()
int DynamicShadow::render() 
{
	tex.use();
	glVertexPointer(  3, GL_FLOAT,         sizeof(Point),     &points[0]);
	glColorPointer(   4, GL_UNSIGNED_BYTE, sizeof(ByteColor), &colors[0]);
	glTexCoordPointer(2, GL_FLOAT,         sizeof(TexCoord),  &texcoords[0]);
	glDrawArrays(GL_TRIANGLES,0,points.size());
	return 0;
}

void DynamicShadow::clear() 
{
	points.clear();
	texcoords.clear();
	colors.clear();
}

void DynamicShadow::gen_tris(const ShadowData &sd)
{
	WorldRef wr;

	std::for_each(wr->find_tri(sd.shadow_vol),
	              wr->end_tri(), 
	              vert_generator(sd,
		                     std::back_inserter(points), 
		                     std::back_inserter(texcoords),
		                     std::back_inserter(colors)));
}


SillyShadow::SillyShadow(const ShadowData &sd, int size) : 
	DynamicShadow(size), ShadowData(sd), last_used(os::time::get_rel_time())
{
	gen_tris(*this);
}

}
}
}
