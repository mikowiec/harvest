#include "os/compat.h"

#include <algorithm>
#include <string>
#include <cfloat>

#include "gfx/light.h"
#include "gfx/exceptions.h"

#include "os/gl.h"
#include "os/debug.h"

#include "res/config.h"
#include "res/res.h"

#include "world/world.h"

#include "main/types_ops.h"
#include "main/types_io.h"

namespace reaper {
namespace gfx {
namespace {
debug::DebugOutput dout("gfx::light",0);
}

namespace light
{
const float Light::cutoff = 10; // light radius is at 1/10 intensity

Light::Light(const Vector4 &p, const Vector &dir, float exp, float ang, 
	     const Color &d, const Color &s, const Color &a,
	     float ca, float la, float qa) :
	pos(p),
	ambient(a), diffuse(d), specular(s),
	spot_dir(dir), 
	exponent(exp), angle(ang),
	const_att(ca), linear_att(la), quad_att(qa),
	radius(0)
{
	calc_radius();
}

Light::Light(const Point &p, const Color &c, float r) :
	pos(p),
	ambient(0,0,0), diffuse(c), specular(c),
	spot_dir(0,0,0), 
	exponent(0), angle(180), const_att(0), linear_att(0), quad_att(cutoff/(r*r)),
	radius(0)
{	
	calc_radius();
}

Light::Light(const Vector &v, const Color &c) :
	pos(v.x, v.y, v.z, 0),
	ambient(0,0,0), diffuse(c), specular(c),
	spot_dir(0,0,0), 
	exponent(0), angle(180), const_att(0), linear_att(0), quad_att(0),
	radius(FLT_MAX)
{
}

void Light::calc_radius()
{
	if(pos.w == 0 || ambient != Color(0,0,0,1) || (linear_att==0 && quad_att==0)) {
		radius = 0;
	} else {
		if(quad_att != 0) {
			float b = linear_att/(2*quad_att);
			float c = (cutoff-const_att)/quad_att;
			float z = sqrt((b*b)+c);

			float x1 = b + z;
			float x2 = b - z;

			radius = std::max(x1,x2);
		} else {
			radius = (cutoff-const_att)/linear_att;
		}
	}
}


void Light::activate(int light) const
{
	light += GL_LIGHT0;

	glLight(light, GL_POSITION, pos);
	
	glLight(light, GL_AMBIENT, ambient);
	glLight(light, GL_DIFFUSE, diffuse);
	glLight(light, GL_SPECULAR, specular);

	glLight(light, GL_SPOT_CUTOFF, angle);

	if(angle != 180) {
		glLight(light, GL_SPOT_DIRECTION, spot_dir);
		glLight(light, GL_SPOT_EXPONENT, exponent);
	}

	glLight(light, GL_CONSTANT_ATTENUATION, const_att);
	glLight(light, GL_LINEAR_ATTENUATION, linear_att);
	glLight(light, GL_QUADRATIC_ATTENUATION, quad_att);
}

Color Light::calc_diffuse_light(const Point &p, const Vector &n, const Material &m) const
{
	// FIXME: Update to handle spot lights

	const Vector dir = pos.as_point() -p;
	const float dist = length(dir);
	float dist_factor, dir_factor;

	if(pos.w == 1) { // positional light
		dist_factor = 1/(const_att + linear_att * dist + quad_att * dist * dist);
		dir_factor = dot(n,dir);
	} else { // directional light
		dist_factor = 1;
		dir_factor = dot(n,pos.as_point());
	}

	Color c = ambient * m.ambient_color +
		  diffuse * dist_factor * dir_factor * m.diffuse_color +
		  m.emission_color;

	return clamp(c);
}

void Light::build_triangles(world::WorldRef &wr)
{
	using namespace std;
	using namespace reaper::world;

	points.clear();
	texcoords.clear();

	back_insert_iterator<vector<Point> >     pi(back_inserter(points));
	back_insert_iterator<vector<TexCoord> >  ti(back_inserter(texcoords));

//	const float rad = radius / 3;
	const float inv_r = 1/radius;
	const float x = pos.x - radius/2;
	const float z = pos.z - radius/2;

	tri_iterator tri = wr->find_tri(Sphere(pos.as_point(), radius));
	const tri_iterator tri_end = wr->end_tri();

	while(tri != tri_end) {
		const Triangle& t = **tri;
		*pi     = t.a;
		*(++pi) = t.b;
		*(++pi) = t.c;
		++pi;

		*ti     = TexCoord((t.a.x - x)*inv_r, (t.a.z - z)*inv_r);
		*(++ti) = TexCoord((t.b.x - x)*inv_r, (t.b.z - z)*inv_r);
		*(++ti) = TexCoord((t.c.x - x)*inv_r, (t.c.z - z)*inv_r);
		++ti;

		++tri;
	}
}

void Light::render()
{
	if(!points.empty()) {
		glColor4(diffuse);
		glVertexPointer(3, GL_FLOAT, sizeof(Point), &points[0]);
		glTexCoordPointer(2, GL_FLOAT, sizeof(TexCoord), &texcoords[0]);
		glDrawArrays(GL_TRIANGLES,0,points.size());
	}
}

} // end light namespace
} // end gfx namespace
} // end reaper namespace

/*
 * $Author: pstrand $
 * $Date: 2003/06/11 15:16:42 $
 * $Log: light.cpp,v $
 * Revision 1.38  2003/06/11 15:16:42  pstrand
 * småfixar..
 *
 * Revision 1.37  2003/06/04 16:29:58  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.36  2002/12/29 17:04:33  pstrand
 * hw/ moved to os/
 *
 * Revision 1.35  2002/05/21 10:18:43  pstrand
 * *** empty log message ***
 *
 * Revision 1.34  2002/01/23 04:42:47  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.33  2002/01/22 23:44:01  peter
 * reversed last two revs
 *
 * Revision 1.31  2002/01/16 00:25:26  picon
 * terrängbelysning
 *
 * Revision 1.30  2002/01/10 23:09:07  macke
 * massa bök
 *
 * Revision 1.29  2001/11/27 04:29:39  peter
 * intelfixar
 *
 * Revision 1.28  2001/11/26 10:46:03  peter
 * kompilatorgnäll... ;)
 *
 * Revision 1.27  2001/09/24 02:33:24  macke
 * Meckat lite med fulbuggen i grafikmotorn.. verkar funka att deallokera nu.. ?
 *
 * Revision 1.26  2001/08/09 18:11:39  macke
 * Fyllefix.. lite här å där..
 *
 * Revision 1.25  2001/08/06 12:16:13  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.24.2.1  2001/07/31 17:34:04  peter
 * testgren...
 *
 * Revision 1.24  2001/07/30 23:43:16  macke
 * Häpp, då var det kört.
 *
 * Revision 1.23  2001/07/09 13:47:52  peter
 * gcc-3.0 fixar
 *
 * Revision 1.22  2001/07/06 01:47:11  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.21  2001/06/09 01:58:48  macke
 * Grafikmotor reorg
 *
 * Revision 1.20  2001/06/07 05:14:17  macke
 * Reaper v0.9
 *
 * Revision 1.19  2001/05/15 01:42:25  peter
 * minskade ner på debugutskrifterna
 *
 * Revision 1.18  2001/05/10 11:40:12  macke
 * häpp
 *
 */

