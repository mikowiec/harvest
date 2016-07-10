
#include "os/compat.h"
#include "os/debug.h"
#include "main/types_io.h"

#include "world/geometry.h"


namespace reaper {
namespace world {

debug::DebugOutput derr("geometry");

float angle_loc(const Vector& v1, const Vector& v2, float l1, float l2) {
	float r = dot(v1, v2) / (l1 * l2);
	return (r < 1.0 && r > -1.0) ? rad2deg(acos(r)) : 0 ;
}

Point proj_on_tri(const Triangle& tri, const Point2D& p)
{
	Vector v1 = tri.edges[0];
	Vector v2 = tri.edges[2];
	Vector norm = cross(v1, v2);
	float y_part = (-norm.x*(p.x - tri.a.x) - norm.z*(p.y - tri.a.z));
	float y = y_part / norm.y + tri.a.y;
//	debug::std_debug << tri.a << tri.b << tri.c << " - "
//			 << v1 << v2 << norm << " - "
//			 << y_part << ' ' << Point(p.x, y, p.y) << '\n';
	return Point(p.x, y, p.y);
}


bool intersect(const Triangle& tri, const Point& pt)
{
	Vector n_a = tri.norms[0];
	Vector n_b = tri.norms[1];
	Vector n_c = tri.norms[2];
	
	Vector v_a = pt - tri.a;
	Vector v_b = pt - tri.b;
	Vector v_c = pt - tri.c;

	bool r = dot(v_a, n_a) < 0
	    && dot(v_b, n_b) < 0
	    && dot(v_c, n_c) < 0;
	return r;
}

bool intersect(const Point& ll, const Point& ur, const world::Line& l)
{
//	derr << "box vs line  " << ll << ' ' << ur << ' ' << l.p1 << ' ' << l.p2 << '\n';
	if (! intersect(ll, ur, minpt(l.p1, l.p2), maxpt(l.p1, l.p2))) {
//		derr << "reject box\n";
		return false;
	}
	const Point box = midpoint(ll, ur);
	const Vector line_dir = l.p2 - l.p1;
	const Vector box_dir = box - l.p1;

	const float c_angle = rangle(line_dir, box_dir);
	const float s_dist = length(box_dir) * sin(c_angle);
	bool r = s_dist < length((ll - ur) / 2);
//	derr << box << ' ' << line_dir << ' ' << box_dir << ' ' << c_angle << ' ' << s_dist << "  -> " << r << '\n';
//	derr << r << '\n';
	return r;
}

bool intersect(const Point& pos, float radius, const world::Line& l)
{
	return intersect(pos, radius, l.p1, l.p2);
}

bool intersect(const Point& pos, float radius, const Point& p1, const Point& p2)
{
	const Vector line_dir = p2 - p1;
	const Vector e_l = norm(line_dir);
	const Vector p_loc = pos - p1;

	float e_proj = dot(p_loc, e_l);
	if (e_proj < -radius || e_proj > radius+length(line_dir))
		return false;
	const Vector p_proj = e_proj * e_l;
	float kat = length(p_loc - p_proj);
	return (kat < radius);
}

bool intersect(const Point2D& ll, const Point2D& ur,
	       const Point2D& center, float radius)
{
	const Point2D ll2(center.x - radius, center.y - radius);
	const Point2D ur2(center.x + radius, center.y + radius);
	return world::intersect(ll, ur, ll2, ur2)
	    && world::intersect(Point2D(ll.x + (ur.x - ll.x)/2,
	    				ll.y + (ur.y - ll.y)/2),
				sqrt(dist_squared(ll, ur))/2, center, radius);
}

bool intersect(const Point& ll, const Point& ur,
	       const Point& center, float radius)
{
	const Vector diag_2 = (ur - ll) / 2;
	const Point mid = ll + diag_2;
	const float rad = length(diag_2);
	const Point ll2 = center - radius;
	const Point ur2 = center + radius;
	return world::intersect(ll, ur, ll2, ur2)
	    && world::intersect(mid, rad, center, radius);
}

bool intersect(const Point& pos, float radius, const world::Frustum& fru)
{
	if(intersect(pos,radius,fru.pos(),0)) {
		return true;
	}

	const Vector& o = pos - fru.pos();

	float dir_len = fru.cutoff();
	float o_proj = dot(o, (fru.dir() / dir_len));

	if (fru.fov_height() < 180 && fru.fov_width() < 180) {
		if (o_proj < 0 || o_proj - radius > dir_len) {
			return false;
		}
	} else {
		if (fabs(o_proj - radius) > dir_len)
			return false;
	}

	const Vector& o_up = o - fru.up() * (dot(o, fru.up()));
	float dir_ang = fabs(angle(o_up, fru.dir()));
	float w_corr = rad2deg(atan2(radius, length(o_up)));

	if (dir_ang - w_corr > fru.fov_width()/2) {
		return false;
	}

	Vector left = fru.left() * (dot(o, fru.left()));
	Vector o_left = o - left;
	float o_left_len = length(o_left);
	float up_ang  = fabs(angle_loc(o_left, fru.dir(), o_left_len, dir_len));

	float h_corr = rad2deg(atan2(radius, o_left_len));

	if (up_ang - h_corr > fru.fov_height()/2) {
		return false;
	}
	return true;
}

bool intersect(const Sphere& s, const Frustum& fru)
{
	return intersect(s.p, s.r, fru);
}


bool intersect(const world::Triangle& t, const world::Frustum& fru)
{
	const world::Sphere sph(tri2sph(t));
	return intersect(sph.p, sph.r, fru);
}



}

}
