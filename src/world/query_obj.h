#ifndef WORLD_QUERY_OBJ_H
#define WORLD_QUERY_OBJ_H

#include "main/types.h"

namespace reaper
{
namespace world
{


/// Cylinder with infinite height
class Cylinder
{
public:
	Point2D p;
	float r;

	Cylinder(const Point2D& pp, float rr)
	 : p(pp),r(rr) { }
	Cylinder(const Cylinder& c) : p(c.p), r(c.r) { }
	Cylinder& operator=(const Cylinder& cyl) {
		p = cyl.p;
		r = cyl.r;
		return *this;
	}
};

class Frustum
{
protected:
	Point pos_;
	Vector dir_;
	Vector up_;
	Vector left_;
	float fov_w, fov_h;
	float cut, radius;
public:
	Frustum()
	 : pos_(0,0,0), dir_(0,0,0), up_(0,0,0), left_(0,0,0),
	   fov_w(0), fov_h(0), cut(0), radius(0)
	{ }
	/** Frustum
	    \param p Eye-position
	    \param d Look-at direction
	    \param u Up vector (normalized, shold be be perpendicular to direction)
	    \param fw field of view, width, degrees
	    \param fh field of view, height, degrees
	 */
	Frustum(const Point& p, const Vector& d, const Vector& u, float fw, float fh);

	Frustum(const Frustum& f)
	 : pos_(f.pos_), dir_(f.dir_), up_(f.up_), left_(f.left_), 
	   fov_w(f.fov_w), fov_h(f.fov_h), cut(f.cut), radius(f.radius)
	{ }
	Frustum& operator=(const Frustum& fru) {
		pos_ = fru.pos_; dir_ = fru.dir_;
		up_ = fru.up_; left_ = fru.left_;
		fov_w = fru.fov_w; fov_h = fru.fov_h;
		cut = fru.cut; radius = fru.radius;
		return *this;
	}

	const Point& pos() const { return pos_; }
	const Vector& dir() const { return dir_; }
	const Vector& up() const { return up_; }
	const Vector& left() const { return left_; }
	float fov_width() const { return fov_w; }
	float fov_height() const { return fov_h; }
	float cutoff() const { return cut; }
	float b_radius() const { return radius; }
};

class Sphere
{
public:
	Point p;
	float r;
	Sphere() : p(0,0,0), r(0) { }
	Sphere(const Point& c, float rr) : p(c), r(rr) { }
	Sphere& operator=(const Sphere& sph) {
		p = sph.p;
		r = sph.r;
		return *this;
	}
};

class Line
{
public:
	Point p1, p2;
	Line(const Point& pp1, const Point& pp2)
	 : p1(pp1), p2(pp2) { }
	Line& operator=(const Line& line) {
		p1 = line.p1; p2 = line.p2;
		return *this;
	}
};

class Rect
{
public:
	Point2D ll, ur;
	Rect() { }
	Rect(const Point2D& x, const Point2D& y)
	: ll(x), ur(y) { }
};

}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/03/31 15:14:28 $
 * $Log: query_obj.h,v $
 * Revision 1.14  2003/03/31 15:14:28  pstrand
 * uninit. vars
 *
 * Revision 1.13  2002/04/14 09:22:37  pstrand
 * quadtree redesign and Sphere::radius -> Sphere::r
 *
 * Revision 1.12  2002/03/18 13:34:08  pstrand
 * rect search & level fix
 *
 * Revision 1.11  2001/08/20 17:11:45  peter
 * obj.ptr
 *
 * Revision 1.10  2001/08/06 12:16:46  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.9.4.1  2001/08/03 13:44:17  peter
 * pragma once obsolete...
 *
 * Revision 1.9  2001/07/21 23:10:41  peter
 * intel fix..
 *
 * Revision 1.8  2001/07/06 01:47:39  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.7  2001/05/06 09:25:33  peter
 * frustum, const.. (förebygga senilitet hos undertecknad..)
 *
 * Revision 1.6  2001/04/24 13:49:24  peter
 * flyttat runt lite...
 *
 * Revision 1.5  2001/04/16 14:52:38  peter
 * dyn. search
 *
 * Revision 1.4  2001/04/10 17:11:07  peter
 * fix..
 *
 * Revision 1.3  2001/04/08 18:02:42  peter
 * *** empty log message ***
 *
 * Revision 1.2  2001/04/05 10:14:56  peter
 * picon vill ha mat, så det blir inga kommentarer... ;)
 *
 * Revision 1.1  2001/02/19 23:40:41  peter
 * altitude..
 *
 */


