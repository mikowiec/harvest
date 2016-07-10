#include "os/compat.h"
#include "vfrelations.h"
#include "shared.h"
#include "main/types_ops.h"
#include "main/types_io.h"
#include "contractionref.h"
#include <iostream>
#include <algorithm>

namespace reaper 
{
namespace gfx
{
namespace mesh
{

	using std::cout;
	using std::cin;

	VFRelations::VFRelations(SRMesh& srmesh):mesh(srmesh)
	{
		int i;
		verts.resize(mesh.vertices.size()*2); 
		conts.resize(mesh.vertices.size());
		quad.resize(mesh.vertices.size()*2);
		face_status.resize(mesh.faces.size());
		for(i = 0; i < face_status.size(); ++i)face_status[i] = true; // all faces active at first
		for(i = 0; i < mesh.faces.size(); ++i) add_face_data(i, mesh.faces[i].vertices[0], 
																mesh.faces[i].vertices[1], 
																mesh.faces[i].vertices[2]);

		for(i = 0; i < mesh.vertices.size(); ++i) update_vertex_quadric(i);
	}


void VFRelations::update_vertex_quadric(int vid)
{
	delete quad[vid];
	quad[vid] = new Quadric(mesh, this, vid);
}

void VFRelations::check_neighbours(int fi)
{
	int i, j;

	if( (mesh.faces[fi].nf[0] == -1) && (mesh.faces[fi].nf[1] == -1) && (mesh.faces[fi].nf[2] == -1) )
		throw MeshException("VFRelations::check_neighbours: I'm all alone!");



	for(i = 0; i < 3; i++){
			j = (i + 1) % 3;
			int n0 = mesh.faces[fi].nf[i];
			int n1 = mesh.faces[fi].nf[j];
			if( (n0 != -1) && (n0 == n1) ){
				mesh.faces[fi].report(fi);
				throw MeshException("VFRelations::check_neigbours: two of a kind");
			}
		}
}

int VFRelations::get_exclusive_vertex_ii(int src, int match)
{
	if(src == -1)return -1;

	bool found = false;
	
	for(int si = 0; si < 3; si++){
		found = false;
		for(int mi = 0; mi < 3; mi++){
			if(mesh.get_active_vertex_relative(mesh.faces[src].vertices[si]) == mesh.get_active_vertex_relative(mesh.faces[match].vertices[mi]))
				found = true;
		}
		if(!found)return si;
	}

	throw MeshException("VFRelations::get_exclusive_vertex_ii: No exclusive match...");
}

int VFRelations::face_neighbour_count(int fi)
{
	int cnt=0;
	for(int i = 0; i < 3; i++)if( mesh.faces[fi].nf[i] != -1)++cnt;

	return cnt;
}

bool VFRelations::test_face_neighbour_switch(int src, int old_dest, int new_dest)
{
		if(src == -1)return false;
		for(int i = 0; i < 3; i++){
			if( (mesh.faces[src].nf[i] == new_dest) && (new_dest != -1)){
				return true;
			}
		}

		return false;
}


bool VFRelations::dangerous_neighbours(PMVertex& pmv)
{
		int i, j;

		int fnnf[4][3];

		for(i = 0; i < 4; i++)if(pmv.fn[i] != -1)for(j = 0; j < 3; j++)fnnf[i][j] = mesh.faces[pmv.fn[i]].nf[j];
		


		int f[4][3] = { {0,0,1}, {1,0,0}, {2,1,3}, {3,1,2}};
		int g[4][3] = { {0,1,0}, {1,0,0}, {2,3,1}, {3,2,1}};


		int k = 0;

		bool bad = false;
		
		//cout << "danger 0";
		//mesh.faces[2].report(2);

		for(k = 0; k < 4; k++){
			if(!test_face_neighbour_switch(pmv.fn[f[k][0]], pmv.flr[f[k][1]], pmv.fn[f[k][2]]) ) {
				mesh.switch_face_neighbour(pmv.fn[f[k][0]], pmv.flr[f[k][1]], pmv.fn[f[k][2]]);
			} else { 
				/*
				if(k > 0)for(i = (k - 1); i >= 0; i--)
					mesh.switch_face_neighbour(pmv.fn[g[i][0]], pmv.fn[g[i][1]], pmv.flr[g[i][2]]);
				*/
				//Restore
				for(i = 0; i < 4; i++)if(pmv.fn[i] != -1)for(j = 0; j < 3; j++)mesh.faces[pmv.fn[i]].nf[j] = fnnf[i][j];
				//cout << "danger 1";
				//mesh.faces[2].report(2);
				return true;

			}
		}

				//cout << "danger 1.5";
				//mesh.faces[2].report(2);

/*
			for(k = 0; k < 4; k++)if(pmv.fn[k] != -1){

				int fi = pmv.fn[k];

				
				for(int i = 0; i < 3; i++){
						int j = (i + 1) % 3;
						int n0 = mesh.faces[fi].nf[i];
						int n1 = mesh.faces[fi].nf[j];
						if( (n0 != -1) && (n0 == n1) ){
							bad = true;
						}
				}
			}
*/
		// Restore		
		for(i = 0; i < 4; i++)if(pmv.fn[i] != -1)for(j = 0; j < 3; j++)mesh.faces[pmv.fn[i]].nf[j] = fnnf[i][j];

		/*
		for(i = 0; i < 4; i++)
			mesh.switch_face_neighbour(pmv.fn[g[i][0]], pmv.fn[g[i][1]], pmv.flr[g[i][2]]);
		*/
		
		//cout << "danger 2";
		//mesh.faces[2].report(2);
		return false;
					
				
/*

		try {
			if(test_face_neighbour_switch(pmv.fn[0], pmv.flr[0], pmv.fn[1]))
				mesh.switch_face_neighbour(pmv.fn[0], pmv.flr[0], pmv.fn[1], "dangerous 0");
			else return true;
			mesh.switch_face_neighbour(pmv.fn[1], pmv.flr[0], pmv.fn[0], "dangerous 1");
			mesh.switch_face_neighbour(pmv.fn[2], pmv.flr[1], pmv.fn[3], "dangerous 2");
			mesh.switch_face_neighbour(pmv.fn[3], pmv.flr[1], pmv.fn[2], "dangerous 3");

			bool bad = false;

			for(int k = 0; k < 4; k++)if(pmv.fn[k] != -1){

				int fi = pmv.fn[k];

				
				for(int i = 0; i < 3; i++){
						int j = (i + 1) % 3;
						int n0 = mesh.faces[fi].nf[i];
						int n1 = mesh.faces[fi].nf[j];
						if( (n0 != -1) && (n0 == n1) ){
							bad = true;
						}
				}
			}

			mesh.switch_face_neighbour(pmv.fn[0], pmv.fn[1], pmv.flr[0], "dangerous 4");
			mesh.switch_face_neighbour(pmv.fn[1], pmv.fn[0], pmv.flr[0], "dangerous 5");
			mesh.switch_face_neighbour(pmv.fn[2], pmv.fn[3], pmv.flr[1], "dangerous 6");
			mesh.switch_face_neighbour(pmv.fn[3], pmv.fn[2], pmv.flr[1], "dangerous 7");

			return bad;

		} catch (MeshException e) {
			
			return true;
		}
*/
/*
		mesh.switch_face_neighbour(pmv.fn[0], pmv.flr[0], pmv.fn[1], "dangerous 0");
		mesh.switch_face_neighbour(pmv.fn[1], pmv.flr[0], pmv.fn[0], "dangerous 1");
		mesh.switch_face_neighbour(pmv.fn[2], pmv.flr[1], pmv.fn[3], "dangerous 2");
		mesh.switch_face_neighbour(pmv.fn[3], pmv.flr[1], pmv.fn[2], "dangerous 3");

		bool bad = false;

		for(int k = 0; k < 4; k++)if(pmv.fn[k] != -1){

			int fi = pmv.fn[k];

			
			for(int i = 0; i < 3; i++){
					int j = (i + 1) % 3;
					int n0 = mesh.faces[fi].nf[i];
					int n1 = mesh.faces[fi].nf[j];
					if( (n0 != -1) && (n0 == n1) ){
						bad = true;
					}
			}
		}


		mesh.switch_face_neighbour(pmv.fn[0], pmv.fn[1], pmv.flr[0], "dangerous 4");
		mesh.switch_face_neighbour(pmv.fn[1], pmv.fn[0], pmv.flr[0], "dangerous 5");
		mesh.switch_face_neighbour(pmv.fn[2], pmv.fn[3], pmv.flr[1], "dangerous 6");
		mesh.switch_face_neighbour(pmv.fn[3], pmv.fn[2], pmv.flr[1], "dangerous 7");
*/

}

/* Normals are considered dangerous if they flip during the upcoming (given) contraction.
*/
bool VFRelations::dangerous_normals(PMVertex& pmv)
{
	int v[3];
	Point p[3];
	for(int fi = 0; fi < 4; fi++){
		if(pmv.fn[fi] != -1){

			for(int vi = 0; vi < 3; vi ++){
				v[vi] = mesh.get_active_vertex_relative( mesh.faces[pmv.fn[fi]].vertices[vi] );
				if( (v[vi] == pmv.vu) || (v[vi] == pmv.vt) )
					p[vi] = pmv.point;
				else
					p[vi] = mesh.vertices[v[vi]].point;
			}

			Vector kv1 = p[1] - p[0];
			Vector kv2 = p[2] - p[0];

			Vector newn = norm(cross(kv1, kv2));
			Vector oldn = mesh.faces[pmv.fn[fi]].normal;

			float dp = dot(oldn, newn);

			if(dp < 0.0)return true;

		}
	}

	return false;
}

				

// BEWARE: Not actually storing the new value anymore
bool VFRelations::update_face_normal(int fi)
{
	
	Vector oldn = mesh.faces[fi].normal;

	Vector kv1 = mesh.vertices[mesh.get_active_vertex_relative(mesh.faces[fi].vertices[1])].point - 
				 mesh.vertices[mesh.get_active_vertex_relative(mesh.faces[fi].vertices[0])].point;

	Vector kv2 = mesh.vertices[mesh.get_active_vertex_relative(mesh.faces[fi].vertices[2])].point - 
				 mesh.vertices[mesh.get_active_vertex_relative(mesh.faces[fi].vertices[0])].point;

	Vector newn = norm(cross(kv1, kv2));

	float dp = dot(oldn, newn);
	
/*
	cout << "\noldn: " << oldn.x << ", " << oldn.y << ", " << oldn.z;
	cout << "\nnewn: " << newn.x << ", " << newn.y << ", " << newn.z;
	cout << "\ndp: " << dp;
*/

	if(dp > -0.0001);//mesh.faces[fi].normal = newn;
	else {
		cout << "\noldn: " << oldn.x << ", " << oldn.y << ", " << oldn.z;
		cout << "\nnewn: " << newn.x << ", " << newn.y << ", " << newn.z;
		cout << "\ndp: " << dp;
		throw MeshException("VFRelations::update_face_normal: normals flipped");
	}

	return (dp < 0.0);

}

bool VFRelations::faces_right_fan_connected(int src, int vi, int dst)
{
	if( src == dst)return true;

	if( (src == -1) || (dst == -1) ) return false;

	if(vi == -1)throw MeshException("VFRelations::faces_right_fan_connected: vi invalid...");

	int lvi;
	lvi = find_face_vertex_index(src, vi);


	int next = mesh.faces[src].nf[lvi];
	if(next == -1) return false;

	lvi = (lvi + 2) % 3;

	int next_vi = mesh.get_active_vertex_relative(mesh.faces[src].vertices[lvi]);

	return faces_right_fan_connected(next, next_vi, dst);
}

void VFRelations::face_assure_vertex_zero(int fi, int vi)
{
	// DON'T FORGET TO SWAP NEIGBOURS ASWELL, GODDAMN!!!


	if(fi == -1)return;

	int i,j;

	for(i = 0; i < 3; i++){
			j = (i + 1) % 3;
			int n0 = mesh.faces[fi].nf[i];
			int n1 = mesh.faces[fi].nf[j];
			if( (n0 != -1) && (n0 == n1) ){
				mesh.faces[fi].report(fi);
				throw MeshException("VFRelations::face_assure_vertex_zero: BEFORE SWAP two of a kind");
			}
		}


	/*
	int cnt = 0;
	for(int i = 0; i < 3; i++)if(mesh.faces[fi].nf[i] == vi)cnt++;
	if(cnt != 1){
		cout << "\ncnt: " << cnt;
		throw MeshException("VFRelations::face_assure_vertex_zero: Counting Error.");
	}
	*/
	if(mesh.get_active_vertex_relative(mesh.faces[fi].vertices[0]) == vi)return;

	if(mesh.get_active_vertex_relative(mesh.faces[fi].vertices[1]) == vi){

		std::swap(mesh.faces[fi].vertices[1], mesh.faces[fi].vertices[0]);
		std::swap(mesh.faces[fi].nf[1], mesh.faces[fi].nf[0]);

		std::swap(mesh.faces[fi].vertices[1], mesh.faces[fi].vertices[2]);
		std::swap(mesh.faces[fi].nf[1], mesh.faces[fi].nf[2]);

		for(i = 0; i < 3; i++){
			j = (i + 1) % 3;
			int n0 = mesh.faces[fi].nf[i];
			int n1 = mesh.faces[fi].nf[j];
			if( (n0 != -1) && (n0 == n1) ){
				throw MeshException("VFRelations::face_assure_vertex_zero: 0 two of a kind");
			}
		}

		return;
	}

	if(mesh.get_active_vertex_relative(mesh.faces[fi].vertices[2]) == vi){

		std::swap(mesh.faces[fi].vertices[2], mesh.faces[fi].vertices[0]);
		std::swap(mesh.faces[fi].nf[2], mesh.faces[fi].nf[0]);

		std::swap(mesh.faces[fi].vertices[2], mesh.faces[fi].vertices[1]);
		std::swap(mesh.faces[fi].nf[2], mesh.faces[fi].nf[1]);

		for(i = 0; i < 3; i++){
			j = (i + 1) % 3;
			int n0 = mesh.faces[fi].nf[i];
			int n1 = mesh.faces[fi].nf[j];
			if( (n0 != -1) && (n0 == n1) ){
				throw MeshException("VFRelations::face_assure_vertex_zero: 1 two of a kind");
			}
		}


		return;
	}

	cout << "face#" << fi << ": " << mesh.faces[fi].vertices[0] << ", " << mesh.faces[fi].vertices[1] << ", " << mesh.faces[fi].vertices[2] << "\n";
	cout << "vertex: " << vi << "\n";
	throw MeshException("Cannot assure vertex zero if no appropriate exists!");

}


Quadric* VFRelations::get_quad(int vid) const
{
	return quad[vid];
}

void VFRelations::add_face_data(int fid, int v0, int v1, int v2)
{
	verts[v0].push_back(fid);
	verts[v1].push_back(fid);
	verts[v2].push_back(fid);
}

void VFRelations::insert_contraction(ContractionRef c_ref)
{
	conts[(*c_ref.ref).vu].push_back(c_ref);
	conts[(*c_ref.ref).vt].push_back(c_ref);
}

void VFRelations::remove_contraction(ContractionRef c_ref, int vid)
{
	//cout << "\nvid: " << vid << ", has_children: " << mesh.vertices[vid].has_children() << ", max: " << mesh.vertices.size() << "\n";
	std::list<ContractionRef>::iterator li;
	std::list<ContractionRef>::iterator dli;

	bool check = false;

	int c_vt = (*c_ref.ref).vt;
	int c_vu = (*c_ref.ref).vu;

	int i = 0;

	if(vid == -1)throw MeshException("VFRelations::remove_contraction - vid == -1");
	if(vid > mesh.vertices.size())throw MeshException("VFRelations::remove_contraction - vid too large");

	if(!mesh.vertices[vid].has_children()){

		li = conts[vid].begin();
		//cout << "bu!\n";		
		while(li != conts[vid].end()){

			if( (*(*li).ref).refused() ){
			//	cout << "\n----- " << i << " ------------\n";
			//	cout << "This one said he was refused:\n";
			//	(*(*li).ref).report();
			//	cout << "But I thought it was more this one:\n";
			//	(*c_ref.ref).report();
				dli = li;
				li++;
				conts[vid].erase(dli);
				check = true;
			} else {
				li++;
			}
			i++;
		}

		
	} else {
		remove_contraction(c_ref, mesh.vertices[vid].vt);
		remove_contraction(c_ref, mesh.vertices[vid].vu);
	}
}


void VFRelations::remove_contraction(ContractionRef c_ref)
{
	//cout << "\nfoo\n";
	int vui = (*c_ref.ref).vu;
	int vti = (*c_ref.ref).vt;

	if((vui == -1) || (vti == -1))throw MeshException("VFRelations::remove_contraction - this is no contraction!");
//	vertex_report(vui, true);
//	vertex_report(vti, true);


	remove_contraction(c_ref, vui);
	remove_contraction(c_ref, vti);
	//cout << "\nbar\n";

	//verify_no_cont_references(c_ref);
}

void VFRelations::update_conts_at_vertex(int vid, PMVertex& pmv)
{
	
	std::list<ContractionRef>::iterator li;
	std::list<ContractionRef>::iterator dli;
	
	/*
	int tvu = (*(*li).ref).vu;
	int tvt = (*(*li).ref).vt;
	if( (tvt > mesh.vertices.size()) || (tvu > mesh.vertices.size()))
		throw MeshException("update_conts_at_vertex: out of bound bigtime");
	*/
	bool cond = false; //(vid == 23);// && (pmv.vt == 1779);

	if(cond){
		cout << "\nupdate_conts_at_vertex enter...\n";
		pmv.report(-1);
		cout << "vid: " << vid << "\n";
		cout << "mesh.vertices.size: " << mesh.vertices.size() << "\n";
		cout << "conts.size: " << conts.size() << "\n";
		cout << "conts[vid].size: " << conts[vid].size() << "\n";
	}
	
	if(cond){
		vertex_report(1772, true);
		vertex_report(1778, true);
		vertex_report(vid, true);
	}
	
	if(cond)cin.get();

	if(cond) cout << "enter conts_at_vertex: " << vid << "\tconts.size(): " << conts[vid].size() << "\n";


	//cout << "alfa..." << vid;
	li = conts[vid].begin();
	while(li != conts[vid].end()){

		if( (*(*li).ref).get_mesh() == NULL) throw MeshException("VFRelations::update_conts_at_vertex:  NULL Pointer Exception");
		//cout << (*(*li).ref).get_mesh() << "\n";
		Contraction c( (*(*li).ref) );
		(*(*li).ref).update(pmv.vt, pmv.vu, mesh.vertices.size() - 1);
		if( (*(*li).ref).invalid() ){
			dli = li;
			li++;
			conts[vid].erase(dli);
		} else {
			li++;
		}
	}
	//cout << " beta\n";

	if(cond) cout << "\nexiting conts_at_vertex\n";

}

void VFRelations::verify_no_cont_references(ContractionRef cr)
{
	if(mesh.active_faces.size() < 3)return;
	std::list<ContractionRef>::iterator li;
	char error[128] = "";

	for(int i = 0; i < conts.size(); i++)
	{
		for(li = conts[i].begin(); li != conts[i].end(); li++)
			if( ((*li).ref) == (cr.ref)){
				cout << "verify_no_cont_references...\n";
				cout << "Attempting to pop: \n";
				(*cr.ref).report();
				vertex_report(i, true);	
				mesh.vertices.back().report(mesh.vertices.size()-1);
				sprintf(error, "verify_no_cont_references %d still have references to: %d/%d", i, (*cr.ref).vt, (*cr.ref).vu);
				throw MeshException(error);
			} 

	}

}

void VFRelations::verify_vertices_have_conts() const
{
	moli::const_iterator li;

	for(li = mesh.active_vertices.begin(); li != mesh.active_vertices.end(); li++)
	{
		if(vertex_cont_count( (*li).id) == 0)throw MeshException("verify_vertices_have_conts.");
	}

}	

int VFRelations::vertex_cont_count(int vid) const
{
	if(mesh.vertices[vid].vt == -1)
		return conts[vid].size();
	else
		return (vertex_cont_count(mesh.vertices[vid].vt) + vertex_cont_count(mesh.vertices[vid].vu));
}

int VFRelations::one_on_edge(int v1, int v2) const
{
	bool bv1 = vertex_on_edge(v1);
	bool bv2 = vertex_on_edge(v2);

	if(bv1 && !bv2)return v1;
	if(bv2 && !bv1)return v2;

	return -1;
}

bool VFRelations::vertex_on_edge(int vid) const
{
	std::set<int> neighbours;
	std::set<int> faces;
	std::set<int>::iterator si;

	get_vertex_neighbours(vid, neighbours);

	for(si = neighbours.begin(); si != neighbours.end(); si++){

		faces.clear();

		get_common_vertex_faces(vid, *si, faces);
		
		if(faces.size() == 0)throw MeshException("VFRelations::vertex_on_edge neighbours have no common faces.");

		remove_currently_equal_faces(*faces.begin(), faces);

		if(faces.empty())return true;
	}

	return false;
}

void VFRelations::get_vertex_neighbours(int vid, std::set<int>& target) const
{
	std::set<int> faces;
	std::set<int>::iterator si;

	get_vertex_faces(vid, faces);

	for(si = faces.begin(); si != faces.end(); si++){

		target.insert(vertex_parent(mesh.faces[*si].vertices[0]));
		target.insert(vertex_parent(mesh.faces[*si].vertices[1]));
		target.insert(vertex_parent(mesh.faces[*si].vertices[2]));

	}

	exclude_one_from_set(vid, target);
}

int VFRelations::find_face_vertex_index(int fi, int vi)
{
	for(int i = 0; i < 3; i++)
		if( mesh.get_active_vertex_relative(mesh.faces[fi].vertices[i]) == vi) return i;

	throw MeshException("VFRelations::find_face_vertex_index found nothing");
}


int VFRelations::get_right_face(int up, int down)
{
	if( (up == -1) || (down == -1) ) 
		throw MeshException("VFRelations::get_right_face: up == down == -1");

	std::set<int> target;

	get_common_vertex_faces(up, down, target);
	if( target.size() == 0)return -1;

	std::set<int>::iterator si = target.begin();

	bool cond = false;// (*si == 159);
	if(cond){
		cout << "\nYo! up/down: " << up << "/" << down << "\n";
		face_report(159, true);
	}

	int right = -1;

	for(int i = 0; i < 3; i++){
		int j = (i + 1) % 3;
		int x   = mesh.get_active_vertex_relative(mesh.faces[(*si)].vertices[i]);
		int xpp = mesh.get_active_vertex_relative(mesh.faces[(*si)].vertices[j]);

		if( (x == up) && (xpp == down) )
			return (*si);
	}

	si++;
	if(si != target.end())return (*si);


	//if(target.size() > 0 && (get_left_face(up, down) == -1))
	//	throw MeshException("get_right_face: didn't find what i should've...");

	return -1;
}

int VFRelations::get_left_face(int up, int down)
{

	if( (up == -1) && (down == -1) ) 
		throw MeshException("VFRelations::get_left_face: up == down == -1");

	std::set<int> target;

	get_common_vertex_faces(up, down, target);
	if( target.size() == 0)return -1;

	std::set<int>::iterator si = target.begin();

	for(int i = 0; i < 3; i++){
		int j = (i + 1) % 3;
		int x   = mesh.get_active_vertex_relative(mesh.faces[(*si)].vertices[i]);
		int xpp = mesh.get_active_vertex_relative(mesh.faces[(*si)].vertices[j]);
		if( (x == down) && (xpp == up) )
			return (*si);
	}

	si++;
	if(si != target.end())return (*si);

	//if((target.size() > 0) && (get_right_face(up, down) == -1))
	//	throw MeshException("get_left_face: didn't find what i should've...");

	return -1;
}

void VFRelations::get_vertex_faces(int vid, std::set<int>& target) const
{
	if(vid == -1)return;


	std::vector<int> v;

	v.push_back(vid);

	int current;

	while(!v.empty()){
		
		std::vector<int>::const_iterator i;

		current = pop(v);

		if(mesh.vertices[current].vt == -1){ // current is in M^
			for(i = verts[current].begin(); i != verts[current].end(); i++)
				if(mesh.faces[*i].is_active)target.insert(*i);
		} else { // current has children. Let's get their faces.
			v.push_back(mesh.vertices[current].vt);
			v.push_back(mesh.vertices[current].vu);
		}
	}


/*
	if(target.size() == 0){
		char txt[128];
		sprintf(txt, "VFRelations::get_vertex_faces: vertex %d contains no faces.", vid);
		throw MeshException(txt);
	}
*/
	std::set<int>::const_iterator si;
	for(si = target.begin(); si != target.end(); si++)if(!mesh.faces[*si].is_active)
		throw MeshException("get_vertex_faces: returning inactive elements...");


}

void VFRelations::common(const std::set<int>& s1, const std::set<int>& s2, std::set<int>& target) const
{
	std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(target, target.begin()));
}

bool VFRelations::common_but_not(const std::set<int>& s1, const std::set<int>& s2, int exclude, int& res) const
{


	if(s1.empty() || s2.empty()){
		res = -1;
		return true;
	}

	std::set<int> target;
	std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(target, target.begin()));

	if(target.empty()){ // With no common elements -1 is returned
		res = -1;
		return true;
	}


	std::set<int>::iterator si;
	si = std::remove(target.begin(), target.end(), exclude);
	if(si != target.end())target.erase(si);
	else throw MeshException("common_but_not: could not remove exclude parameter"); 


	remove_currently_equal_faces(exclude, target);

	if(target.size() > 1){
		if(!faces_currently_equal(target)){

			res = get_smallest_face(target);

			return true;
			/*
			cout << "\n\n";
			printset(s1, "s1");
			printset(s2, "s2");
			printset(target, "target");
			face_report(exclude, true);
			for(si = target.begin(); si != target.end(); si++)
			{
				face_report(*si, true);
			}


			throw MeshException("common_but_not result is larger than one. Cannot be stored.");
			*/
		}
	}

	if(target.size() > 0)res = *target.begin();
	else res = -1;

	if((res != -1) && !mesh.faces[res].is_active){
		cout << "res: " << res << "\n";
		throw MeshException("common_but_not: returning inactive face...");
	}

	return true;
}

bool VFRelations::common_but_not(const std::set<int>& s1, const std::set<int>& s2, int exclude1, int exclude2, int& res) const
{
	throw MeshException("common_but_not should not be called with two parameters");

	std::set<int> target;
	std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(target, target.begin()));

	if(target.empty()){ // With no common elements -1 is returned
		res = -1;
		return true;
	}

	std::set<int>::iterator si;

	si = std::remove(target.begin(), target.end(), exclude1);
	if(si != target.end())target.erase(si);
	else throw MeshException("common_but_not: could not remove first exclude parameter"); 
	
	if(target.size() > 1){
		si = std::remove(target.begin(), target.end(), exclude2);
		if(si != target.end())target.erase(si);
		else throw MeshException("common_but_not: could not remove seccond exclude parameter"); 
	}


	if(target.size() > 1){

		if(!faces_currently_equal(target))
			throw MeshException("common_but_not result is larger than one. Cannot be stored.");

		/*
		cout << "\n\n";
		printset(s1, "s1");
		printset(s2, "s2");
		printset(target, "target");
		for(si = target.begin(); si != target.end(); si++)
		{
			face_report(*si, true);
		}

		cout << "exclude 1/2: " << exclude1 << "/" << exclude2 << "\n";
		throw MeshException("common_but_not result is larger than one. Cannot be stored.");
		*/
	}

	res = *target.begin();
}

void VFRelations::remove_currently_equal_faces(int fid, std::set<int>& target) const
{
	std::set<int>::iterator si, tmp;

	si = target.begin();

	while(si != target.end())
	{
		if(faces_currently_equal(fid, *si)){
			tmp = si;
			si++;
			target.erase(tmp);
		} else {
			si++;
		}
	}

}
			


bool VFRelations::faces_currently_equal(std::set<int>& source) const
{
	if(source.size() < 2)return true;

	std::set<int>::iterator si1;
	std::set<int>::iterator si2;

	si1 = si2 = source.begin();
	si2++;
	

	while(si2 != source.end())
	{
		if(!faces_currently_equal(*si1, *si2))return false;
		si1 = si2;
		si2++;
	}

	return true;
}

bool VFRelations::faces_currently_equal(int f1, int f2) const
{
	std::set<int> f1s;
	std::set<int> f2s;
	std::set<int> target;
	
	int i;
	for(i = 0; i < 3; i++)f1s.insert(vertex_parent(mesh.faces[f1].vertices[i]));
	for(i = 0; i < 3; i++)f2s.insert(vertex_parent(mesh.faces[f2].vertices[i]));

	std::set_difference(f1s.begin(), f1s.end(), f2s.begin(), f2s.end(), std::inserter(target, target.begin()));

	if(target.size() != 0)return false;
	
	return true;
}


int VFRelations::three_exclude_two(int t1, int t2, int t3, int e1, int e2) const
{
	if(t1 != e1 && t1 != e2)return t1;
	if(t2 != e1 && t2 != e2)return t2;
	if(t3 != e1 && t3 != e2)return t3;

	throw MalFormedInput();
}

void VFRelations::get_common_vertex_faces(int v1, int v2, std::set<int>& target) const
{
	std::set<int> v1_faces;
	std::set<int> v2_faces;

	get_vertex_faces(v1, v1_faces);
	get_vertex_faces(v2, v2_faces);

	std::set<int>::const_iterator si;

	common(v1_faces, v2_faces, target);

}

void VFRelations::exclude_one_from_set(int exclude, std::set<int>& target) const
{
	std::remove(target.begin(), target.end(), exclude);
}


int VFRelations::get_smallest_face(std::set<int>& source) const
{
	int res = -1;
	float area = 0.0;
	float tmp;

	std::set<int>::const_iterator si;

	for(si = source.begin(); si != source.end(); si++)
	{
		tmp = face_area(*si);
		if( (tmp < area) || (res == -1) )
		{
			area = tmp;
			res = *si;
		}
	}

	return res;
}


/*
void VFRelations::expand_vertex_set(std::set<int>& target) const
{
	std::set<int>::iterator si = target.begin();
	std::set<int> tmp;
	while(si != target.end()){
	
		get_cascading_vertex(*i, 
*/
void VFRelations::report()
{
	cout << "\n";
	cout << "-------------------------\n";
	cout << "| VFRelations reporting |" << "\n";
	cout << "-------------------------" << "\n";
	cout << "Vertex count:\t" << verts.size() << "\n";
	cout << "\n";
}

void VFRelations::vertex_report(int vid, bool verbose)
{
	cout << "\n";
	cout << "--------------------------------\n";
	cout << "| VFRelations Vertex reporting |" << "\n";
	cout << "--------------------------------" << "\n";
	cout << "#: " << vid << "\tvt/vu: " << mesh.vertices[vid].vt << "/" << mesh.vertices[vid].vu;
	cout << "\tparent:" << mesh.vertices[vid].parent;
	cout << "\t@ " << "(" << mesh.vertices[vid].point.x << ", " << mesh.vertices[vid].point.y << ", " << mesh.vertices[vid].point.z << ")\n";

	if(verbose){

		if(vid < conts.size()){
			std::list<ContractionRef>::const_iterator li;
			cout << "Contractions:\n";
			for(li = conts[vid].begin(); li != conts[vid].end(); li++)
				cout << "\t" << (((*(*li).ref)).vt) << " <-> " << (((*(*li).ref)).vu) << "\t" << (((*(*li).ref)).get_mesh()) << "\n";
			cout << "\n";
		} else {
			cout << "No contractions available.\n";
		}

		std::set<int> tmps;
		std::set<int>::const_iterator si;
		get_vertex_faces(vid, tmps);

		cout << "Faces: \n";
		for(si = tmps.begin(); si != tmps.end(); si++){
				cout << "\t" << (*si) << "\t";
				if(mesh.faces[*si].is_active)
					cout << "ACTIVE\n";
				else
					cout << "NOT active\n";
		}
	
		cout << "\n";

	}
}

int VFRelations::vertex_parent(int vid) const
{
	if(mesh.vertices[vid].parent != -1)return vertex_parent(mesh.vertices[vid].parent);
	else return vid;
}

void VFRelations::print_vertex_rel(int vid) const
{
	if(mesh.vertices[vid].parent != -1){
		cout << vid << "->";
		print_vertex_rel(mesh.vertices[vid].parent);
	} else 
		cout << vid << "\n";
}

float VFRelations::face_area(int fid) const
{
	Point p1 = mesh.vertices[vertex_parent(mesh.faces[fid].vertices[0])].point;
	Point p2 = mesh.vertices[vertex_parent(mesh.faces[fid].vertices[1])].point;
	Point p3 = mesh.vertices[vertex_parent(mesh.faces[fid].vertices[2])].point;

	Point u = (p2 - p1);
	Point v = (p3 - p1);

	Point r = cross(u, v).to_pt();

	return (length(r) / 2.0);
}
	

void VFRelations::face_report(int fid, bool verbose) const
{
	cout << "\n";
	cout << "------------------------------\n";
	cout << "| VFRelations Face reporting |" << "\n";
	cout << "------------------------------" << "\n";
	cout << "#: " << fid << "\n";
	cout << "area: " << face_area(fid) << "\n";
	cout << "verts: \n";
	//int a = vertex_parent(mesh.faces[fid].vertices[0]);
	//int b = vertex_parent(mesh.faces[fid].vertices[1]);
	//int c = vertex_parent(mesh.faces[fid].vertices[2]);
	//mesh.vertices[a].report(true);
	//mesh.vertices[b].report(true);
	//mesh.vertices[c].report(true);
	print_vertex_rel(mesh.faces[fid].vertices[0]);
	print_vertex_rel(mesh.faces[fid].vertices[1]);
	print_vertex_rel(mesh.faces[fid].vertices[2]);

	//cout << vertex_parent(mesh.faces[fid].vertices[0]) << ", ";
	//cout << vertex_parent(mesh.faces[fid].vertices[1]) << ", ";
	//cout << vertex_parent(mesh.faces[fid].vertices[2]) << "\n";
	cout << "orig verts: ";
	cout << (mesh.faces[fid].vertices[0]) << ", ";
	cout << (mesh.faces[fid].vertices[1]) << ", ";
	cout << (mesh.faces[fid].vertices[2]) << "\n";
	cout << "\n";

	cout << "neighbours: ";
	for(int i = 0;i < 3; i++)cout << mesh.faces[fid].nf[i] << ", ";
	cout << "\n";
}

void VFRelations::printset(const std::set<int>& s, char* info) const
{
	std::set<int>::const_iterator si;
	cout << "Set Content " << info << ": ";
	for(si = s.begin(); si != s.end(); si++)
		cout << *si << ", ";

	cout << "\n";

}


void VFRelations::setup_face_neighbours()
{
	std::vector<PMFace>::iterator vi = mesh.faces.begin();
	int fi = 0;

	while(vi != mesh.faces.end()){

		(*vi).nf[0] = -1;
		for(int i0 = 0; i0 < 3; i0++){
			int i1 = (i0 + 1) % 3;
			int i2 = (i0 + 2) % 3;

			std::set<int> target;
			get_common_vertex_faces((*vi).vertices[i1], (*vi).vertices[i2], target);

			exclude_one_from_set(fi, target);
			if(target.size() > 2)
				throw MeshException("setup_face_neighbours incoherent set");
		

			int fai;
			if(target.size() < 2)fai = -1;
			else fai = (*target.begin());

			(*vi).nf[i0] = fai;
		}
	
		int i,j;

		for(i = 0; i < 3; i++){
			j = (i + 1) % 3;
			int n0 = mesh.faces[fi].nf[i];
			int n1 = mesh.faces[fi].nf[j];
			if( (n0 != -1) && (n0 == n1) ){
				throw MeshException("VFRelations::setup_face_neighbours: two of a kind");
			}
		}


		vi++;
		fi++;

	}
}

void VFRelations::create_nice_normals(int vi)
{
	std::set<int> vf;
	std::set<int>::iterator vfi;

	Vector tn(0.0, 0.0, 0.0);

	get_vertex_faces(vi, vf);

	if(vf.size() == 0){
		char err[128];
		sprintf(err, "VFRelations::create_nice_normals: %d had zero faces", vi);
		throw MeshException(err);
	}

	//cout << "\n nice: \n";
	for(vfi = vf.begin(); vfi != vf.end(); vfi++){
		//cout << mesh.faces[*vfi].normal << "\n";
		tn += mesh.faces[*vfi].normal;
	}

	tn = tn/length(tn);

	//cout << " res: " << tn << "\n";

	mesh.vertices[vi].normal = tn;
}	

void VFRelations::create_nice_normals()
{
	int vi;
	std::set<int> vf;
	std::set<int>::iterator vfi;
	Vector n;
	Vector tn;
	Point p[2];



	for(vi = 0; vi < mesh.vertices.size(); vi++)
	{
		vf.clear();

		tn.x = tn.y = tn.z = 0.0;
		
		std::vector<int>::const_iterator i;

		
		for(i = verts[vi].begin(); i != verts[vi].end(); i++)
			tn +=mesh.faces[*i].normal;
		

		/*
		get_vertex_faces(vi, vf);

		if(vf.size() == 0){
			char err[128];
			sprintf(err, "VFRelations::create_nice_normals: %d had zero faces", vi);
			throw MeshException(err);
		}
		//cout << vi << "\t" << vf.size() << "\n";

		
		for(vfi = vf.begin(); vfi != vf.end(); vfi++)
		{
			//p[0] = mesh.vertices[mesh.faces[*vfi].vertices[1]].point - mesh.vertices[mesh.faces[*vfi].vertices[0]].point;
			//p[1] = mesh.vertices[mesh.faces[*vfi].vertices[2]].point - mesh.vertices[mesh.faces[*vfi].vertices[0]].point;
			//n = cross(p[0], p[1]);
			//tn += n/length(n);

			tn += mesh.faces[*vfi].normal;
		}
		*/
		tn = tn/length(tn);

		mesh.vertices[vi].normal = tn;

		//cout << tn << "\n";
	}
}



}
}
}
