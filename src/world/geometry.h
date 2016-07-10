#ifndef WORLD_GEOMETRY_H
#define WORLD_GEOMETRY_H

#include "main/types_ops.h"
#include "world/triangle.h"
#include "world/query_obj.h"
#include "world/geom_helper.h"

#include <cmath>

namespace reaper
{
namespace world
{


// Just approximations.. Make more fancy stuff later...

/** Circle <-> Circle */
inline
bool intersect(const Point2D& c1, float r1, const Point2D& c2, float r2)
{
	return dist_squared(c1, c2) < pow(r1 + r2, 2);
}


/** Sphere <-> Sphere */
inline
bool intersect(const Point& c1, float r1, const Point& c2, float r2)
{
	return dist_squared(c1, c2) < pow(r1 + r2, 2);
}


/** Rectangle <-> Rectangle */
inline
bool intersect(const Point2D& r1_ll, const Point2D& r1_ur,
	       const Point2D& r2_ll, const Point2D& r2_ur)
{
	return r1_ur > r2_ll && r1_ll < r2_ur;
}

/** Box <-> Box */
inline
bool intersect(const Point& r1_ll, const Point& r1_ur,
	       const Point& r2_ll, const Point& r2_ur)
{
	return r1_ur > r2_ll && r1_ll < r2_ur;
}

/** Box <-> Line */
bool intersect(const Point& ll, const Point& ur, const Line& l);

/** Sphere <-> Line */
bool intersect(const Point& pos, float radius, const Line& l);
bool intersect(const Point& pos, float radius, const Point& p1, const Point& p2);

/** Rect <-> Circle */
bool intersect(const Point2D& ll, const Point2D& ur,
	       const Point2D& center, float radius);

/** Box <-> Sphere */
bool intersect(const Point& ll, const Point& ur,
	       const Point& center, float radius);

bool intersect(const Point& pos, float radius, const Frustum& fru);
bool intersect(const Sphere& s, const Frustum& fru);
bool intersect(const Triangle& t, const Frustum& fru);


/** Triangle <-> Point */
bool intersect(const Triangle& tri, const Point& pt);

/** Get the point on a triangle corresponding to a point in the xz-plane */
Point proj_on_tri(const Triangle& tri, const Point2D& p);


}
}


#endif

/*
 * $Author: pstrand $
 * $Date: 2003/06/12 08:17:36 $
 * $Log: geometry.h,v $
 * Revision 1.26  2003/06/12 08:17:36  pstrand
 * switch to loose, non-sharing quadtree
 *
 * Revision 1.25  2001/08/06 12:16:45  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.24.2.2  2001/08/03 13:44:16  peter
 * pragma once obsolete...
 *
 * Revision 1.24.2.1  2001/07/31 17:34:10  peter
 * testgren...
 *
 * Revision 1.24  2001/07/30 23:43:34  macke
 * Häpp, då var det kört.
 *
 * Revision 1.23  2001/07/06 01:47:38  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.22  2001/05/03 18:17:53  macke
 * huga..
 *
 * Revision 1.21  2001/04/26 15:24:37  peter
 * *** empty log message ***
 *
 * Revision 1.20  2001/04/25 00:48:43  peter
 * omorganisering...
 *
 * Revision 1.19  2001/04/25 00:29:24  peter
 * no message
 *
 * Revision 1.18  2001/04/24 13:49:23  peter
 * flyttat runt lite...
 *
 * Revision 1.17  2001/04/16 22:12:53  peter
 * flyttade (en del i) quadtree -> world
 *
 */

