#include "os/compat.h"
#include "quadrics.h"
#include "vfrelations.h"
#include "main/types_io.h"

#include <float.h>

#include <iostream>

namespace reaper 
{
namespace gfx
{
namespace mesh
{

	using std::cout;
	using std::cin;

Quadric::Quadric(const SRMesh& srmesh, const VFRelations* vfrel, int vid):
	mesh(srmesh),
	vfrelations(vfrel),
	on_edge(false)
{
	bool cond = false;//(vid == 628);
	if(cond)cout << "Quadric::Quadric caught trap. (" << vid << ")\n";
	clear();
	Point here = mesh.vertices[vfrelations->vertex_parent(vid)].point;

	std::set<int> faces;
	std::set<int>::const_iterator si;
	vfrelations->get_vertex_faces(vid, faces);

	// Construct walls for edgenodes
	std::vector<int> edgvec;
	edgvec.clear();
	get_connected_edge_vertices(vid, edgvec);
	if(cond)cout << "edge vertices: " << edgvec.size() << "\n";
	while(!edgvec.empty())
	{
		if(cond)cout << "edge:\n";

		Point p_inside = mesh.vertices[pop(edgvec)].point;
		Point p_onedge = mesh.vertices[pop(edgvec)].point;

		Vector kv1 = (p_inside - here);
		Vector kv2 = (p_onedge - here);

		Point p_up = (cross(kv1,kv2) + here);

		if(cond)cout << "here/p_onedge/p_up: " << here << "/" << p_onedge << "/" << p_up << "\n";

		Quadric quad(here, p_onedge, p_up, mesh, vfrelations);

		if(cond)quad.report();

		if(cond)cout << "===========================\n";

		quad *= 100.0;
		(*this) += quad;
	}
	
	// Continue for normal nodes
	for(si = faces.begin(); si != faces.end(); si++)
	{
		add_face(*si, vid);
	}
}
	
Quadric::Quadric(const Point& p0,const Point& p1,const Point& p2, const SRMesh& srmesh, const VFRelations* vfrel):
		mesh(srmesh),
		vfrelations(vfrel),
		on_edge(false)
{
	/*
	cout << "\np0: " << p0 << "\n";
	cout << "p1: " << p1 << "\n";
	cout << "p2: " << p2 << "\n";
	cout << "p:  " << p << "\n";
	*/
	Vector rv1 = (p1 - p0);
	Vector rv2 = (p2 - p0);
	/*
	cout << "rv1: " << rv1 << "\n";
	cout << "rv2: " << rv2 << "\n";
	*/
	Point n = cross(rv1,rv2).to_pt();

	normd(n);

	/*
	cout << "n: " << n << "\n";
	*/
	A = mul(n, n);

	Vector Ap = A*p0.to_vec();

	b = -Ap;

	c = dot(p0, Ap);
	/*
	cout << "\nA:\n" << A.get_col(0) << "\n" << A.get_col(1) << "\n" << A.get_col(2) << "\n\n";
	cout << "b: " << b << "\n";
	cout << "c: " << c << "\n";

	cin.get();
	*/
	if(isnan(c))clear();

}
	
void Quadric::add_face(int fid, int vid)
{
	bool cond = false;// (vid == 628);

	Point here = mesh.vertices[vfrelations->vertex_parent(vid)].point;

	int vi0 = vfrelations->vertex_parent(mesh.faces[fid].vertices[0]);
	int vi1 = vfrelations->vertex_parent(mesh.faces[fid].vertices[1]);
	int vi2 = vfrelations->vertex_parent(mesh.faces[fid].vertices[2]);

	Point p0 = mesh.vertices[vi0].point;
	Point p1 = mesh.vertices[vi1].point;
	Point p2 = mesh.vertices[vi2].point;

	if(cond){
		cout << "\n-----------------------------------------\n";
		cout << "face: " << fid << "\n";

		cout << "p0: " << p0;
		cout << " p1: " << p1;
		cout << " p2: " << p2;
	}
	Quadric tq(p0, p1, p2, mesh, vfrelations);
	if(cond)tq.report();
	(*this) += tq;
}


// @IMPORTANT: This is a SMART (tm) function!
void Quadric::get_connected_edge_vertices(int vid, std::vector<int>& target)
{
	std::set<int>::const_iterator si;
	std::set<int>::const_iterator sj;
	std::set<int> faces;

	vfrelations->get_vertex_faces(vid, faces);

	for(si = faces.begin(); si != faces.end(); si++){

		int i1 = vfrelations->vertex_parent(mesh.faces[*si].vertices[0]);
		int i2 = vfrelations->vertex_parent(mesh.faces[*si].vertices[1]);

		if(i1 == vid)i1 = vfrelations->vertex_parent(mesh.faces[*si].vertices[2]);
		if(i2 == vid)i2 = vfrelations->vertex_parent(mesh.faces[*si].vertices[2]);
		bool b1 = false;
		bool b2 = false;

		// i1 & i2 hold the "other ones"

		for(sj = faces.begin(); sj != faces.end(); sj++){
			if(*si == *sj)continue;

			int j1 = vfrelations->vertex_parent(mesh.faces[*sj].vertices[0]);
			int j2 = vfrelations->vertex_parent(mesh.faces[*sj].vertices[1]);
			int j3 = vfrelations->vertex_parent(mesh.faces[*sj].vertices[2]);

			if(!b1)if(i1 == j1 || i1 == j2 || i1 == j3) b1 = true;
			if(!b2)if(i2 == j1 || i2 == j2 || i2 == j3) b2 = true;
		}

		if(!b1){
			target.push_back(i1);
			target.push_back(i2);
		}
		if(!b2){
			target.push_back(i2);
			target.push_back(i1);
		}

	}

	if(target.size() > 0)on_edge = true;
}

float Quadric::cost(const Vector& v)
{

	Vector t(190,190,40);
	bool cond = (v == t);

	if(cond){
		cout << "\n----------------------------------------\n";
		cout << "cost cond fullfilled.\n";
		report();
	}

	

	//A*v
	Vector Av = A*v;
	//if(cond)cout << "Av: " << Av << "\n";

	//v[transponat]*(A*v)
	float vtAv = dot(v, Av);
	if(cond)cout << "vtAv: " << vtAv << "\n";

	//2*b[transponat]*v
	float btv = 2*dot(b, v);
	if(cond)cout << "2btv: " << btv << "\n";
	if(cond)cout << "c: " << c << "\n";

	float res = vtAv + btv + c;
	if(cond)cout << "res: " << res << "\n";

	//if(on_edge)res+=5;
/*
	if( (res < 0) && (res*res > 0.001)){
		cout << "\n\nv: (" << v[0] << ", " << v[1] << ", " << v[2] << ")\n";
		report();
		char error[128];
		sprintf(error, "Quadric::cost Subzero. %f", res);
		throw MeshException(error);
	}
*/
	return res;
}

float Quadric::report_cost(const Vector& v)
{

	Vector t(190,190,40);
	bool cond = (v == t);

	cout << "\n----------------------------------------\n";
	cout << "cost calculating.\n";


	//A*v
	Vector Av = A*v;

	//v[transponat]*(A*v)
	float vtAv = dot(v, Av);
	cout << "vtAv: " << vtAv << "\n";

	//2*b[transponat]*v
	float btv = 2*dot(b, v);
	cout << "2btv: " << btv << "\n";
	cout << "c: " << c << "\n";

	float res = vtAv + btv + c;
	cout << "res: " << res << "\n";
/*
	if( (res < 0) && (res*res > 0.001)){
		cout << "\n\nv: (" << v[0] << ", " << v[1] << ", " << v[2] << ")\n";
		report();
		char error[128];
		sprintf(error, "Quadric::cost Subzero. %f", res);
		throw MeshException(error);
	}
*/
	return res;
}





void Quadric::clear()
{
	for(int i = 0; i < 3; i++)A.col(i, Vector(0.0, 0.0, 0.0));
	b.x = b.y = b.z = c = 0.0;
	
}



void Quadric::report()
{
	cout << "\n\n";
	cout.precision(4);
	int i,j;
	cout << "A\n";
	for(i=0;i<3;++i){
		for(j=0;j<3;++j){
			cout << A[i][j] << '\t';
		}
		cout << "\n";
	}
	cout << "b\n";
	for(i=0;i<3;++i)
			cout << b[i] << '\n';

	cout << "c\n";

	cout << c;

	cout << "\n\n";
}

}
}
}


