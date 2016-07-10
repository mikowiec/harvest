#include "os/compat.h"

#include "contractions.h"
#include "srmesh.h"


#include <algorithm>
#include <iostream>

#include <time.h>


namespace reaper 
{
namespace gfx
{
namespace mesh
{

	using std::cout;

/*************************************************************
/                 CLASS: Contractions
/*************************************************************/


Contractions::Contractions(const SRMesh& srmesh, VFRelations& vfr):
	mesh(srmesh), 
	vfrelations(vfr),
	update_time(0.0)
{
}


void Contractions::insert(Contraction* new_cont)
{
	if(new_cont == NULL)throw MeshException("Contractions::insert: NPE");
	ref_list.push_front(*new_cont);
	conts.push_back(ContractionRef(ref_list.begin()));
	vfrelations.insert_contraction(ContractionRef(ref_list.begin()));
}

void Contractions::verify_update(PMVertex& update_pmv)
{
	// Find surrounding vertices
	std::set<int> affected;
	find_affected_vertices(update_pmv, affected);

	// Update corresponding contractions
	for(int i = 0; i < conts.size(); ++i) verify_potential_update(i, affected, update_pmv, mesh.vertices.size() - 1);
}

void Contractions::update(PMVertex& update_pmv)
{
	// Find surrounding vertices
	std::set<int> affected;
	find_affected_vertices(update_pmv, affected);

	// Update corresponding contractions
	for(int i = 0; i < conts.size(); ++i)potential_update(i, affected, update_pmv, mesh.vertices.size() - 1);
}

void Contractions::fast_update(PMVertex& update_pmv)
{
	//cout << "\nfast_update: \n";
	//update_pmv.report(-1);
	// Find surrounding vertices
	int start, end;
	start = clock();
	std::set<int> affected;
	find_affected_vertices(update_pmv, affected);
	// Update corresponding contractions
	std::set<int>::iterator si;
	for(si = affected.begin(); si != affected.end(); si++) cascade_cont_update(*si, update_pmv);
	end = clock();
	update_time += ((float) (end - start)) / (float) CLOCKS_PER_SEC;

}

void Contractions::cascade_cont_update(int vid, PMVertex& pmv)
{
	/*
	if(pmv.vt == 44){
		verify_report(true);
	}
	*/

	if(mesh.vertices[vid].vt == -1) {
		//cout << "cascade_cont_update, updating: " << vid << "\n";
		//vfrelations.vertex_report(vid, false);
		vfrelations.update_conts_at_vertex(vid, pmv);
	} else {
		cascade_cont_update(mesh.vertices[vid].vt, pmv);
		cascade_cont_update(mesh.vertices[vid].vu, pmv);
	}

}

Contraction Contractions::get_top()
{
	Contraction c(*(conts.front().ref));
	return c;
}

void Contractions::pop_top()
{
	//vfrelations.verify_no_cont_references(conts.front());
	ref_list.erase(conts.front().ref);
	conts.erase(conts.begin());
}

void Contractions::remove_top()
{
	(*conts.front().ref).refuse();
	//cout << "\nsrmesh.size: " << mesh.vertices.size();
	//cout << "\nremove_top...\n";
	//(*conts.front().ref).report();
	vfrelations.remove_contraction(conts.front());
	pop_top();
}

void Contractions::subdue_top()
{
	float newcost = ((*conts.back().ref).get_cost() + (*conts.front().ref).get_cost()) / 2.0;

	(*conts.front().ref).subdued++;
	(*conts.front().ref).set_cost(newcost);
	conts.push_back(conts.front());
	conts.erase(conts.begin());
}

void Contractions::clear()
{
	conts.clear();
	ref_list.clear();
}

void Contractions::re_sort()
{
	
	if(conts.empty())throw MeshException("Contractions::re_sort, Cannot sort nothing.");
	std::sort(conts.begin(), conts.end());
	
}

void Contractions::find_affected_vertices(PMVertex& pmv, std::set<int>& target)
{
	std::set<int> vu_faces;
	std::set<int> vt_faces;
	std::set<int> faces;
	
	vfrelations.get_vertex_faces(pmv.vu, vu_faces);
	vfrelations.get_vertex_faces(pmv.vt, vt_faces);
	std::set_union(vu_faces.begin(), vu_faces.end(), vt_faces.begin(), vt_faces.end(), std::inserter(faces, faces.begin()));
	if(pmv.flr[0] != -1)faces.insert(pmv.flr[0]);
	if(pmv.flr[1] != -1)faces.insert(pmv.flr[1]);

	std::set<int>::const_iterator si;

	for(si = faces.begin(); si != faces.end(); si++)
	{
		target.insert(mesh.get_face_vertex(*si, 0));
		target.insert(mesh.get_face_vertex(*si, 1));
		target.insert(mesh.get_face_vertex(*si, 2));
	}

	target.insert(pmv.vu);
	target.insert(pmv.vt);
}

void Contractions::potential_update(int index, std::set<int> &affected, PMVertex& pmv, int pmvi)
{

	if( (std::find(affected.begin(), affected.end(), (*conts[index].ref).vu) != affected.end() ) ||
		(std::find(affected.begin(), affected.end(), (*conts[index].ref).vt) != affected.end() ) )
		(*conts[index].ref).update(pmv.vu, pmv.vt, pmvi);			
}

void Contractions::verify_potential_update(int index, std::set<int> &affected, PMVertex& pmv, int pmvi)
{

	if( (std::find(affected.begin(), affected.end(), (*conts[index].ref).vu) != affected.end() ) ||
		(std::find(affected.begin(), affected.end(), (*conts[index].ref).vt) != affected.end() ) )
		(*conts[index].ref).verify_update(pmv.vu, pmv.vt, pmvi);			
}


bool Contractions::empty(){
	return conts.empty();
}


void Contractions::render()
{
		glBegin(GL_TRIANGLES);  //tells OpenGL that we're going to start drawing triangles


			std::vector<ContractionRef>::const_iterator ci;
			ci = conts.begin(); 
			while(ci != conts.end()){

	

				Point point = (*(*ci).ref).get_point();

				glColor3f(0.0, 1.0, 0.0);
				glVertex3f(point.x, point.y+0.6, point.z);
				glVertex3f(point.x-0.26, point.y-0.26, point.z);
				glVertex3f(point.x+0.26, point.y-0.26, point.z);
		
				ci++;

			}


		glEnd();
}
	

void Contractions::report(bool verbose)
{
	cout << "\n";
	cout << "--------------------------\n";
	cout << "| Contractions reporting |" << "\n";
	cout << "--------------------------" << "\n";
	cout << "Contraction count:\t" << conts.size() << "\n";

	if(verbose){
		int i = 0;
		std::vector<ContractionRef>::const_iterator ci;
		for(ci = conts.begin(); ci != conts.end(); ci++){
			cout << i << "\t(" << (*(*ci).ref).vu << ", " << (*(*ci).ref).vt << ")   \t" << "@" << (*(*ci).ref).get_cost() << "\t";
			cout << "mesh: " << (*(*ci).ref).get_mesh() << "\n";
			++i;
		}
	} 

	cout << "\n";
}

void Contractions::verify_report(bool verbose)
{
		cout << "\n";
		cout << "---------------------------------\n";
		cout << "| Contractions Verify reporting |" << "\n";
		cout << "---------------------------------" << "\n";
		cout << "Contraction count:\t" << conts.size() << "\n";

		int i = 0;
		int m = 0;

		std::vector<ContractionRef>::const_iterator ci;
		for(ci = conts.begin(); ci != conts.end(); ci++){
			if( ( (int) (*(*ci).ref).get_mesh()) != m){
				m = (int) (*(*ci).ref).get_mesh();
				cout << std::hex << "mesh: " << m << std::dec << "\n";
			}
			++i;
		}
		cout << "\n";

		if(verbose){
			i=0;
			for(ci = conts.begin(); ci != conts.end(); ci++){
				m = (int) (*(*ci).ref).get_mesh();

				cout << "#" << i << "\t&mesh: "  << std::hex << m << std::dec << "\n";

				++i;
			}
			cout << "\n";
		}
}

void Contractions::verify_integrity()
{
	std::list<Contraction>::iterator li = ref_list.begin();
	int size = mesh.vertices.size();
	while(li != ref_list.end()){
		if(((*li).vt > size) || ((*li).vu > size)){
			(*li).report();
			throw MeshException("verify_integrity is out of bound");
		}
		li++;
	}


}


}
}
}
