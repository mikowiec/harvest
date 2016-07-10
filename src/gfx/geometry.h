#ifndef REAPER_GFX_GEOMETRY_H
#define REAPER_GFX_GEOMETRY_H

#include "main/types.h"
#include "main/types_ops.h"
#include <vector>

namespace reaper {
namespace gfx {
namespace mesh {

using std::vector;

struct Triangle : public PtrTriangle
{
	Triangle(const IdxTriangle &t, Point *pts) : PtrTriangle(&pts[t.v1], &pts[t.v2], &pts[t.v3]) {
		n = norm(cross(*v2-*v1,*v3-*v1));
	}

	Vector n;
	mutable bool lit; // temporary value, used for determining silhouette edges
};

struct Edge 
{
	Point *p1, *p2;
	Triangle *t1, *t2;
	
	Edge(Point *P1, Point *P2) : t1(0), t2(0) {
		p1 = P1;
		p2 = P2;
	}

	bool operator==(const Edge &e) const {
		return (p1 == e.p1 && p2 == e.p2) || (p1 == e.p2 && p2 == e.p1);
	}
};

class Geometry;

class ShadowVolume
{
public:
	Matrix mtx;
	vector<Point> points;
	vector<IdxTriangle> triangles;
	const Geometry *geometry;

	ShadowVolume() {}
	void reset();
	void render() const;

	friend class Geometry;
};

class Geometry 
{
public:
	vector<Point> points;
	vector<Triangle> triangles;
	vector<Edge> edges;

	void add_edge(Point *p1, Point *p2, Triangle *tri);

	Geometry() {}
	Geometry(const vector<Point> &pts, const vector<IdxTriangle> &tris);
	
	void init(const vector<Point> &pts, const vector<IdxTriangle> &tris);

	//FIXME: assumes directional light and extends shadows down to y=0
	void generate_shadow_volume(const Vector &light, const Matrix &mtx, ShadowVolume &sv) const; 
};

}
}
}

#endif

