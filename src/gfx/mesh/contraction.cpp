#include "os/compat.h"
#include <iostream>

#include <string.h>

#include "contraction.h"
#include "vfrelations.h"

#include "main/types.h"
#include "main/types_ops.h"
#include "main/types_io.h"

namespace reaper 
{
namespace gfx
{
namespace mesh
{

	using std::cout;
	using std::cin;
/*************************************************************
/                 CLASS: Contraction
/*************************************************************/


	Contraction::Contraction(const Pair& pair,const SRMesh* srmesh, const VFRelations* vfrel):
		mesh(srmesh), 
		vfrelations(vfrel),
		refused_explicitly(false),
		subdued(0)
	 {
		if(mesh == NULL)throw MeshException("Contraction::Contraction: NPE");

		 vu = pair.a;
		 vt = pair.b;

		 set_optimal_point_and_cost();


	 }


	Contraction::Contraction(const Contraction& arg):
		mesh(arg.mesh), 
	    vfrelations(arg.vfrelations),
		point(arg.point),
		cost(arg.cost),
		affected(arg.affected),
		vu(arg.vu),
		vt(arg.vt),
		refused_explicitly(arg.refused_explicitly),
		subdued(arg.subdued),
		color(arg.color),
		normal(arg.normal)
	{
		if(mesh == NULL)throw MeshException("Contraction::Contraction2: NPE");

		tc = (TexCoord*) malloc( sizeof(TexCoord) * mesh->tex.cnt() );
		for(int i = 0; i < mesh->tex.cnt(); i++)
			tc[i] = arg.tc[i];

	}

void Contraction::order()
{
	if(vt>vu)std::swap(vt,vu);
}

bool Contraction::invalid()
{
	return ((vt == vu));
}

bool Contraction::refused()
{
	return refused_explicitly;
}



void Contraction::set_optimal_point_and_cost()
{
	int i;
	if( (vu < 0) || (vt < 0) ){
		report();
		throw MeshException("Contraction::set_optimal_point_and_cost - set_optimal_point_and_cost vt/vu cannot be subzero");
	}
		
	if(mesh == NULL){

		throw MeshException("Contraction::set_optimal_point_and_cost: Null Pointer Exception!");
	}

	 if( vu > (mesh->vertices.size() - 1)) throw MeshException("set_optimal_point_and_cost vu too large");
	 if( vt > (mesh->vertices.size() - 1)) throw MeshException("set_optimal_point_and_cost vt too large");

	 Point pu = mesh->vertices[vu].point;
	 Point pt = mesh->vertices[vt].point;

	 Point parent_point;
	 int least_cost_param = 0;
	 float least_cost = 10E30;
	 float tmp_cost;

	 Quadric* qvu = vfrelations->get_quad(vu);
	 Quadric* qvt = vfrelations->get_quad(vt);
	 
	 Quadric q(*qvu);
	 q.clear();
	 q += *qvu;
	 q += *qvt;



	 for(int i = 0; i <= 10; ++i){
			 
		 parent_point = ( pu * (i*0.1) ) + ( pt * (1 - i*0.1) );
		 tmp_cost = q.cost(parent_point.to_vec());

		 if(tmp_cost < 0)tmp_cost = 0;
		 //float other = qvu->cost(parent_point.to_vec()) + qvt->cost(parent_point.to_vec());

		 //float diff = abs(tmp_cost - other);

		 if( false )//(tmp_cost !=  other) && (abs(tmp_cost-other) > 0.01))
		 {
			 cout << "cost: " << tmp_cost << " when i = " << i << "\n";
			 cout << "vt/vu: " << vt << "/" << vu << "\n";

			 q.report_cost(parent_point.to_vec());

			 qvt->report();
			 qvu->report();
			 q.report();
			 cout << "\nparent point: " << parent_point << "\n";

			 cout.precision(30);
			 cout << "\n";
			 //cout << "tmp: " << tmp_cost << "\noth: " << other;
		//	 cout << "\ndif: " << diff << "\n";
			 throw MeshException("Contraction::set_optimal_point_and_cost inconsistency.");
		 }
			 
		 if(tmp_cost < least_cost){
			 least_cost = tmp_cost;
			 least_cost_param = i;
		 }
	 }

	 cost = least_cost;

	 float lcf = ((float) least_cost_param) * 0.1f;
	 float lcf_comp = 1.0f - lcf;

	 point = ( pu * (least_cost_param*0.1) ) + ( pt * (1 - least_cost_param*0.1) );

	 Point cu = mesh->vertices[vu].color;
	 Point ct = mesh->vertices[vt].color;

	 color = ( cu * (least_cost_param*0.1) ) + ( ct * (1 - least_cost_param*0.1) );

	 Vector nu = mesh->vertices[vu].normal;
	 Vector nt = mesh->vertices[vt].normal;

	 normal = nu + nt;
	 normal = normal / length(normal);

	 //cout << "texcnt: " << mesh->tex.cnt() << "\n";
	 tc = (TexCoord*) malloc( sizeof(TexCoord) * mesh->tex.cnt() );
	 for(i = 0; i < mesh->tex.cnt(); i++){
		 TexCoord tcu = mesh->vertices[vu].tc[i];
		 TexCoord tct = mesh->vertices[vt].tc[i];
		 tc[i] = (tcu * lcf)  + (tct * lcf_comp);
		 //cout << tcu.u << ", " << tc[i].u << "\n";
		 //cout << tcu.a[3] << ", " << tct.a[3] << ", " << tc[i].a[3] << "\n"; cin.get();
		 //tc[i].u = (tcu.u * (least_cost_param*0.1) ) + (tct.u * (1 - least_cost_param*0.1) );
		 //tc[i].v = (tcu.v * (least_cost_param*0.1) ) + (tct.v * (1 - least_cost_param*0.1) );
	 }



}

void Contraction::verify_optimal_point_and_cost()
{

	 Point ref_point = point;
	 float ref_cost = cost;

	 Point pu = mesh->vertices[vu].point;
	 Point pt = mesh->vertices[vt].point;

	 Point parent_point;
	 int least_cost_param = 0;
	 float least_cost = 10E30;
	 float tmp_cost;

	 Quadric* qvu = vfrelations->get_quad(vu);
	 Quadric* qvt = vfrelations->get_quad(vt);
	 
	 Quadric q(*qvu);
	 q.clear();
	 q += *qvu;
	 q += *qvt;

	 for(int i = 0; i <= 10; ++i){
			 
		 parent_point = ( pu * (i*0.1) ) + ( pt * (1 - i*0.1) );
		 tmp_cost = q.cost(parent_point.to_vec());

		 if(tmp_cost < 0)tmp_cost = 0;
			 
		 if(tmp_cost < least_cost){
			 least_cost = tmp_cost;
			 least_cost_param = i;
		 }
	 }

	 /*
	 for(int i = 0; i <= 10; ++i){
	
		 
		 parent_point = ( pu * (i*0.1) ) + ( pt * (1 - i*0.1) );
		 tmp_cost = calc_cost(vu, vt, parent_point);
		 
		 if(tmp_cost < least_cost){
			 least_cost = tmp_cost;
			 least_cost_param = i;
		 }
	 }
	*/
	 Point np = ( (least_cost_param*0.1) * pu) + ( (1 - least_cost_param*0.1) * pt);
	 /*
	 if(point != np){
		 sprintf(error, "Point not updated \n(%f,%f,%f) should be \n(%f,%f,%f)", ref_point.x, ref_point.y, ref_point.z, np.x, np.y, np.z); 
		 throw MeshException(error);
	 }
	*/
	 if(cost != least_cost){
		 cout << "\ncost/least_cost: " << cost << " / " << least_cost;
		 throw MeshException("Contraction::verify_optimal_point_and_cost - Cost not updated");
	 }


}


void Contraction::update(int s1, int s2, int d)
{
	bool cond = false;//(d == 136);

	if(cond) cout << "\nupdating...\ts1: " << s1 << "\ts2: " << s2 << "\tto: " << d << "\n";
	if(cond) cout << "vt: " << vt << "\nvu: " << vu << "\n";
	//cout << "Now updating: (" << vu << ", " << vt << ") to (";

	if((vu == s1) || (vu == s2))vu = d;
	if((vt == s1) || (vt == s2))vt = d;

	if(vu == vt){
		if(cond)cout << "done updating...(Collapsed!)\n";
		return;
	}
/*
	int size = mesh->vertices.size();
	if((vt > size) || (vu > size)){
		report();
		throw MeshException("verify_integrity is out of bound");
	}

*/	
	//cout << vu << ", " << vt << ")\t";

	//cout << "supposed to take (" << s1 << ", " << s2 << ") -> " << d << "\n";

	//if(d == 45)cout << "foo"  << "s1/s2: " << s1 << "/" << s2 << "\t" << d << "\tmesh: " << mesh << "\n";
	set_optimal_point_and_cost();
	//if(d == 45)cout << "bar " << d << "\n";

	if(cond)cout << "done updating.\n";


}

void Contraction::verify_update(int s1, int s2, int d)
{
	char error[128] = "";

	if(vu != vt){
		if((vu == s1) || (vu == s2)){
			sprintf(error, "verify_update: vu is still %d, should be %d\n", vu, d);
			throw MeshException(error);
		}

		if((vt == s1) || (vt == s2)){
			sprintf(error, "verify_update: vt is still %d, should be %d\n", vt, d);
			throw MeshException("verify_update: vt-change not updated");
		}
	}


	if(vu == vt){
		return;
	}
	
	verify_optimal_point_and_cost();

}


float Contraction::calc_cost(int tmp_vu, int tmp_vt, const Point& parent)
{
	Point cpu = (mesh->vertices[tmp_vu].point - parent);
	Point cpt = (mesh->vertices[tmp_vt].point - parent);

	return (std::min(length(cpu) , length(cpt)) );

	/*
	std::set<int> vu_faces;
	std::set<int> vt_faces;

	vfrelations->get_vertex_faces(tmp_vu, vu_faces);
	vfrelations->get_vertex_faces(tmp_vt, vt_faces);

	// Temporary sollution...

	Point cpu = (mesh->vertices[tmp_vu].point - parent).to_pt();
	Point cpt = (mesh->vertices[tmp_vt].point - parent).to_pt();

	return (length(cpu) * vu_faces.size()) + (length(cpt) * vt_faces.size());
	*/
}

int Contraction::estimate_face_collapse()
{
	std::set<int> s;
	vfrelations->get_common_vertex_faces(vu, vt, s);
	return s.size();
}

void Contraction::report()
{
	cout << "--------------------------\n";
	cout << "| Contraction reporting |" << "\n";
	cout << "--------------------------" << "\n";
	cout << "vu:\t" << vu << "\n";
	cout << "vt:\t" << vt << "\n";
	cout << "cost:\t" << cost << "\n";
	cout << "mesh: " << mesh << "\n";
	cout << "refused: " << refused_explicitly << "\n";
}


}
}
}
