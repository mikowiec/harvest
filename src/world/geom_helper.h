#ifndef WORLD_GEOM_HELPER_H
#define WORLD_GEOM_HELPER_H

#include <map>
#include "world/triangle.h"

namespace reaper
{
namespace world
{

using std::min;
using std::max;

inline
float max(float a, float b, float c)
{
	return (a > c) ? max(a,b) : max(b,c);
}

inline
Point maxtri(Point a, Point b, Point c)
{
	return Point(max(a.x, b.x, c.x),
		     max(a.y, b.y, c.y),
		     max(a.z, b.z, c.z));
}

inline
Point2D maxtri2(Point a, Point b, Point c)
{
	return Point2D(max(a.x, b.x, c.x), max(a.z, b.z, c.z));
}

inline
float min(float a, float b, float c)
{
	return (a < c) ? min(a,b) : min(b,c);
}

inline
Point mintri(Point a, Point b, Point c)
{
	return Point(min(a.x, b.x, c.x),
		     min(a.y, b.y, c.y),
		     min(a.z, b.z, c.z));
}

inline
Point2D mintri2(Point a, Point b, Point c)
{
	return Point2D(min(a.x, b.x, c.x), min(a.z, b.z, c.z));
}


inline
const Point minpt(const Point& p1, const Point& p2)
{
	return Point(min(p1.x, p2.x), min(p1.y, p2.y), min(p1.z, p2.z));
}

inline
const Point maxpt(const Point& p1, const Point& p2)
{
	return Point(max(p1.x, p2.x), max(p1.y, p2.y), max(p1.z, p2.z));
}

inline
const Point2D minpt2(const Point& p1, const Point& p2)
{
	return Point2D(min(p1.x, p2.x), min(p1.z, p2.z));
}

inline
const Point2D maxpt2(const Point& p1, const Point& p2)
{
	return Point2D(max(p1.x, p2.x), max(p1.z, p2.z));
}

inline
float mid(float a, float b, float c)
{
	return (a+b+c)/3;
}


inline
Point midtri(Point a, Point b, Point c)
{
	return Point(mid(a.x, b.x, c.x),
		     mid(a.y, b.y, c.y),
		     mid(a.z, b.z, c.z));
}

inline
Point midpoint(const Point& ll, const Point& ur)
{
	return ll + (ur - ll) / 2;	
}


inline bool operator<(const Point& p1 , const Point& p2) {
	return p1.x < p2.x && p1.y < p2.y && p1.z < p2.z;
}

inline bool operator>(const Point& p1 , const Point& p2) {
	return p1.x > p2.x && p1.y > p2.y && p1.z > p2.z;
}

inline bool operator<(const Point2D& p1 , const Point2D& p2) {
	return p1.x < p2.x && p1.y < p2.y;
}

inline bool operator>(const Point2D& p1 , const Point2D& p2) {
	return p1.x > p2.x && p1.y > p2.y;
}

inline
const Sphere tri2sph(const Triangle& tri)
{
	Point mx(maxtri(tri.a, tri.b, tri.c));
	Point mn(mintri(tri.a, tri.b, tri.c));
	float r = length(mx-mn) / 2;
	return Sphere(Point(mn+r), r);
}

typedef std::pair<Point, Point> PtBox;

inline
const PtBox tri2box(const Triangle& tri)
{
	return std::make_pair(mintri(tri.a, tri.b, tri.c),
			      maxtri(tri.a, tri.b, tri.c));
}


inline
float dist_squared(const Point2D& ll, const Point2D& ur)
{
	const float d_x = ur.x - ll.x;
	const float d_y = ur.y - ll.y;
	return d_x*d_x + d_y*d_y;
}

inline
float dist_squared(const Point& p1, const Point& p2)
{
	return length_sq(p1 - p2);
}

inline
float rangle(const Vector& a, const Vector& b)
{
	float l2 = length(a) * length(b);
	if (l2 == 0)
		return 0;
	float r = dot(a,b) / l2;
	return (r < 1.0 && r > -1.0) ? acos(r) : 0;
}

inline
float rad2deg(float r)
{
	return 180/3.141159 * r;
}

inline
float angle(const Vector& a, const Vector& b)
{
	return rad2deg(rangle(a,b));
}



}

}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/06/12 08:17:35 $
 * $Log: geom_helper.h,v $
 * Revision 1.10  2003/06/12 08:17:35  pstrand
 * switch to loose, non-sharing quadtree
 *
 * Revision 1.9  2002/09/21 17:40:21  pstrand
 * uninit.vars.
 *
 * Revision 1.8  2002/04/08 21:27:47  pstrand
 * *** empty log message ***
 *
 * Revision 1.7  2001/08/06 12:16:45  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.6.2.2  2001/08/03 13:44:16  peter
 * pragma once obsolete...
 *
 * Revision 1.6.2.1  2001/08/01 00:48:31  peter
 * visual fix...
 *
 * Revision 1.6  2001/07/30 23:43:34  macke
 * Häpp, då var det kört.
 *
 * Revision 1.5  2001/07/06 01:47:38  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.4  2001/04/29 13:32:30  peter
 * no message
 *
 * Revision 1.3  2001/04/25 01:09:16  peter
 * no message
 *
 * Revision 1.2  2001/04/25 00:58:16  peter
 * no message
 *
 * Revision 1.1  2001/04/25 00:49:04  peter
 * omorganisering...
 *
 *
 */

