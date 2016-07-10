#ifndef REAPER_MESH_QUADRICS_H
#define REAPER_MESH_QUADRICS_H


#include "main/types.h"
#include "srmesh.h"
#include "os/compat.h"
#include "main/types.h"
#include "main/types_ops.h"

#include <iostream>
#include <iomanip>


namespace reaper 
{
namespace gfx
{
namespace mesh
{

	using std::ostream;

class VFRelations;

/*************************************************************************
* Class Quadric: Using the (A,b,c) representation                        *
*************************************************************************/

class Quadric {

private:

	const SRMesh& mesh;
	const VFRelations* vfrelations;

	Matrix	A;
	Vector	b;
	float	c;

public:

	bool on_edge;

Quadric(const SRMesh& srmesh, const VFRelations* vfrel, int vid);
Quadric(const Quadric& q):on_edge(q.on_edge), mesh(q.mesh), vfrelations(q.vfrelations){};
Quadric(const Point& p0, const Point& p1, const Point& p2, const SRMesh& srmesh, const VFRelations* vfrel);

void get_connected_edge_vertices(int vid, std::vector<int>& target);
void add_face(int fid, int vid);

float cost(const Vector& v);
float report_cost(const Vector& v);

void clear();

void report();

inline Quadric* operator+(const Quadric& q) const
{
	Quadric* res = new Quadric(*this);
	res->clear();

	res->A += A;
	res->b += b;
	res->c += c;

	res->A += q.A;
	res->b += q.b;
	res->c += q.c;

	return res;
}

inline Quadric& operator+=(const Quadric& q)
{ 
	A += q.A;
	b += q.b;
	c += q.c;

	return *this;
}

inline Quadric& operator*=(float param)
{
	A *= param;
	b *= param;
	c *= param;

	return *this;
}

/*
inline Quadric& operator=(const Quadric& q)
{ 
	A = q.A;
	b = q.b;
	c = q.c;
}
*/

};


}
}
}

#endif
