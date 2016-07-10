#include "os/compat.h"

#include <iterator>
#include <set>

#include "os/gl.h"
#include "os/debug.h"
#include "gfx/geometry.h"
#include "misc/sequence.h"

namespace reaper {
namespace gfx {
namespace mesh {

using namespace std;
using namespace reaper::misc;
	
namespace {
	debug::DebugOutput dout("gfx::Geometry");
}

Geometry::Geometry(const vector<Point> &pts, const vector<IdxTriangle> &tris) 
{
	init(pts, tris);
}

void Geometry::init(const vector<Point> &pts, const vector<IdxTriangle> &tris)
{
	points.insert(points.begin(), pts.begin(), pts.end());

	triangles.reserve(tris.size());
	for(std::vector<IdxTriangle>::const_iterator i = tris.begin(); i != tris.end(); ++i) {
		triangles.push_back(Triangle(*i, &points[0]));
	}

	for(std::vector<Triangle>::iterator i = triangles.begin(); i != triangles.end(); ++i) {
		add_edge(i->v1, i->v2, &*i);
		add_edge(i->v2, i->v3, &*i);
		add_edge(i->v3, i->v1, &*i);
	}
}

void Geometry::add_edge(Point *p1, Point *p2, Triangle *tri)
{
	Edge e(p1, p2);
	vector<Edge>::iterator pos = std::find(edges.begin(), edges.end(), e);
	if(pos == edges.end()) {
		e.t1 = tri;
		edges.push_back(e);
	} else {
		if(pos->t1 == tri || pos->t2 == tri) {
			dout << "hrmphf\n";
		} else {
			if(pos->t2 != 0) 
				dout << "eek\n";
			pos->t2 = tri;
		}
	}
}


void Geometry::generate_shadow_volume(const Vector &light, const Matrix &mtx, ShadowVolume &sv) const
{	
	const float inv_y = 1.0f / light.y;
	const Vector inv_light = -light;

	for(vector<Triangle>::const_iterator i = triangles.begin(); i != triangles.end(); ++i) {
		i->lit = dot(light, mtx * i->n) > 0;

		const Point p1 = mtx * *i->v1;
		const Point p2 = mtx * *i->v2;
		const Point p3 = mtx * *i->v3;

		if(i->lit) {
//			int idx = sv.points.size();
			sv.points.push_back(p1);
			sv.points.push_back(p2);
			sv.points.push_back(p3);
			sv.points.push_back(p3);
//			sv.triangles.push_back(IdxTriangle(idx, idx+1, idx+2));
		} else {
			sv.points.push_back(p1 + (p1.y * inv_y) * inv_light);
			sv.points.push_back(p2 + (p2.y * inv_y) * inv_light);
			sv.points.push_back(p3 + (p3.y * inv_y) * inv_light);
			sv.points.push_back(p3 + (p3.y * inv_y) * inv_light);
		}
	}


	for(vector<Edge>::const_iterator i = edges.begin(); i != edges.end(); i++) {
		if(i->t1 && i->t2 && (i->t1->lit ^ i->t2->lit)) {
			const Point p1 = mtx * *i->p1;
			const Point p2 = mtx * *i->p2;

			if(i->t1->lit) {
				sv.points.push_back(p2);
				sv.points.push_back(p1);
				sv.points.push_back(p1 + (p1.y * inv_y) * inv_light);
				sv.points.push_back(p2 + (p2.y * inv_y) * inv_light);
			} else {
				sv.points.push_back(p1);
				sv.points.push_back(p2);
				sv.points.push_back(p2 + (p2.y * inv_y) * inv_light);
				sv.points.push_back(p1 + (p1.y * inv_y) * inv_light);
			}
/*
			int idx = sv.points.size();
			if(i->t2->lit) {
				sv.triangles.push_back(IdxTriangle(idx, idx+1, idx+2));
				sv.triangles.push_back(IdxTriangle(idx, idx+2, idx+3));
			} else {
				sv.triangles.push_back(IdxTriangle(idx, idx+2, idx+1));
				sv.triangles.push_back(IdxTriangle(idx, idx+3, idx+2));
			}
*/
		}
	}

	//sv.mtx = mtx;
	sv.geometry = this;
}

void ShadowVolume::reset()
{
	points.clear();
	triangles.clear();
}

void ShadowVolume::render() const
{
	//glPushMatrix();
	//glMultMatrix(mtx);
	glVertexPointer(3, GL_FLOAT, sizeof(Point), &points[0]);
	glDrawArrays(GL_QUADS, 0, points.size());
//	glDrawElements(GL_TRIANGLES, triangles.size()*3, GL_UNSIGNED_INT, &triangles[0]);
	//glPopMatrix();
}

}
}
}

