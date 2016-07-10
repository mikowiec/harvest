#ifndef REAPER_GFX_LIGHT_H
#define REAPER_GFX_LIGHT_H

#include "main/types.h"
#include "main/types4.h"
#include "gfx/gfx_types.h"

#include "world/world.h"
#include <list>
#include <vector>

namespace reaper {
namespace gfx {
namespace light {

class Light
{
public:
	Vector4 pos;

	Color ambient;
	Color diffuse;
	Color specular;
	
	Vector spot_dir;  ///< Spot direction
	float exponent;   ///< Spot focusing
	float angle;      ///< Spot cutoff angle
	
	/// Directional light, diffuse/specular equal
	Light(const Vector &v, const Color &c);

	/// Positional light with quadratic dropoff, diffuse/specular equal
	Light(const Point &p, const Color &c, float radius);

	/// General light
	Light(const Vector4 &p, const Vector &spot_dir, float exp, float ang, 
	      const Color &d, const Color &s, const Color &a,
	      float ca, float la, float qa);

	void  activate(int light) const;	

	float get_radius()     const { return radius; }
	float get_const_att()  const { return const_att; }
	float get_linear_att() const { return linear_att; }
	float get_quad_att()   const { return quad_att; }
	Point get_pos() const        { return pos.as_point(); }   // quadtree needs this

	Color calc_diffuse_light(const Point &p, const Vector &n, const Material &m) const; 	
	
	void build_triangles(world::WorldRef &wr);
	void render();

protected:
	float const_att;  ///< Constant attenuation factor
	float linear_att; ///< Linear attenuation factor
	float quad_att;   ///< Quadratic attentuation factor	
	float radius;     ///< Light radius (light is at 10% intensity)

	std::vector<Point>     points;
	std::vector<TexCoord>  texcoords;

	static const float cutoff; // intensity at with light source has radius (reciprocal value)
	void calc_radius();
};

}
}
}

#endif

/*
 * $Author: picon $
 * $Date: 2002/01/16 00:25:26 $
 * $Log: light.h,v $
 * Revision 1.17  2002/01/16 00:25:26  picon
 * terrängbelysning
 *
 * Revision 1.16  2002/01/11 20:27:35  peter
 * .
 *
 * Revision 1.15  2002/01/10 23:09:07  macke
 * massa bök
 *
 * Revision 1.14  2001/08/09 18:11:40  macke
 * Fyllefix.. lite här å där..
 *
 * Revision 1.13  2001/08/06 12:16:13  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.12.2.2  2001/08/03 13:43:52  peter
 * pragma once obsolete...
 *
 * Revision 1.12.2.1  2001/07/31 17:34:04  peter
 * testgren...
 *
 * Revision 1.12  2001/07/30 23:43:16  macke
 * Häpp, då var det kört.
 *
 * Revision 1.11  2001/07/06 01:47:11  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.10  2001/06/09 01:58:48  macke
 * Grafikmotor reorg
 *
 * Revision 1.9  2001/05/10 11:40:12  macke
 * häpp
 *
 */

