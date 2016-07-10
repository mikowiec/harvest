#include "os/compat.h"
#include <time.h>
#include "pmfactory.h"
#include "main/types_io.h"
#include "shared.h"
#include <vector>
#include <iostream>
#include <stdio.h>


namespace reaper 
{
namespace gfx
{
namespace mesh
{

using std::cout;
using std::cin;

PMFactory::PMFactory(SRMesh* m):mesh(m), subdue_threshold(4), virgin(true), lower_bound(0)
{ 
}

PMSkeleton* PMFactory::export_skeleton()
{
	if(virgin)throw MeshException("PMFactory::export_skeleton: virginity.");

	// Minimize the mesh
	// Done already (see build)

	// Store away a skeleton CCW ( always ) 
	PMSkeleton* skel = new PMSkeleton;

	lint::iterator li = mesh->active_faces.begin();

	skel->vertices = new Point[mesh->active_faces.size() * 3];
	skel->vertex_count = mesh->active_faces.size() * 3;

	for(int i = 0; i < mesh->active_faces.size(); i++){
		skel->vertices[3*i + 0] = mesh->vertices[mesh->faces[*li].vertices[0]].point;
		skel->vertices[3*i + 1] = mesh->vertices[mesh->faces[*li].vertices[1]].point;
		skel->vertices[3*i + 2] = mesh->vertices[mesh->faces[*li].vertices[2]].point;
		li++;
	}

	return skel;
}



void PMFactory::generate_pairs()
{
	std::vector<PMFace>::const_iterator cvi;
	for(cvi = (*mesh).faces.begin(); cvi != (*mesh).faces.end(); cvi++){
		pairs.insert((*cvi).vertices[0], (*cvi).vertices[1]);
		pairs.insert((*cvi).vertices[1], (*cvi).vertices[2]);
		pairs.insert((*cvi).vertices[2], (*cvi).vertices[0]);
	}
}

void PMFactory::initiate_contractions()
{
	Contraction* cont;
	int dcount = 0;
	int max = pairs.size();
	int delta = max / 20;


	while(!pairs.is_empty()){

		cont = new Contraction(pairs.pop(), &(*mesh), vfrelations);
		
		contractions->insert(cont);

	}

	contractions->re_sort();

}

void PMFactory::after_pass()
{	

	int vi = (*mesh).vertices.size() - 1;

	while(vi >= non_prog_limit){

		PMVertex& pmv = (*mesh).vertices[vi];

		if(!pmv.is_active)
			throw MeshException("PMFactory::after_pass: last vertex inactive!");

		// Split
		// Activate children
		(*mesh).add_active_vertex(pmv.vt);
		(*mesh).add_active_vertex(pmv.vu);

		// Activate fl/fr
		(*mesh).add_active_face(pmv.flr[0]);
		(*mesh).add_active_face(pmv.flr[1]);

		// Deactivate v
		(*mesh).remove_active_vertex(vi);

		// Fix neigbouring faces
		pmv.fni[0] = vfrelations->get_exclusive_vertex_ii(pmv.fn[0], pmv.flr[0]);
		pmv.fni[1] = vfrelations->get_exclusive_vertex_ii(pmv.fn[1], pmv.flr[0]);
		pmv.fni[2] = vfrelations->get_exclusive_vertex_ii(pmv.fn[2], pmv.flr[1]);
		pmv.fni[3] = vfrelations->get_exclusive_vertex_ii(pmv.fn[3], pmv.flr[1]);

		for(int i = 0; i < 4; i++)if(pmv.fn[i] != -1)
			(*mesh).faces[pmv.fn[i]].nf[pmv.fni[i]] = pmv.flr[i<2?0:1];


		vi--;

	}

	

}



void PMFactory::pop_and_update()
{
	pop_timer.start();

	Contraction* cont;

	static int invalid = 0;
	static int sing = 0;
	static int cnt = 0;
	static bool sort;
	
	//ut << "\nenter pop and update...";

	//re_sort contractions when it's size has decreased with 1 %
	sort = (cnt < 1);
	if(cnt < 1)cnt = contractions->size() * 0.01;
	cnt--;


	// Stop invalid pops
	if(contractions->empty())throw MeshException("pop_and_update::Cannot pop from empty contractions");

	// Get a contraction candidate
	cont = new Contraction(contractions->get_top());

	// Throw away invalid or overly subdued contractions
	while( ((*cont).vt == (*cont).vu) || ( (*cont).subdued > subdue_threshold) ) {
		invalid++;
		delete cont;
		contractions->remove_top();
		if(contractions->empty())return;
		cont = new Contraction(contractions->get_top());
	}

	if(cont->get_mesh() == NULL)throw MeshException("pop_and_update: NPE");

	if((*mesh).vertices[(*cont).vu].parent != -1) throw MeshException("pop_and_update::vu.parent != -1");
	if((*mesh).vertices[(*cont).vt].parent != -1) throw MeshException("pop_and_update::vt.parent != -1");

	switch(new_pmvertex(*cont)){
		case SUBDUE: // Currently contains a pesh, put it last in line and leave it for later.
			subdue_timer.start();
			contractions->subdue_top();
			subdue_timer.stop();
			break;
		case INVALID: // Potential singularity - throw away. 
			sing++;
			contractions->remove_top();
			if(sort && !contractions->empty()){
				sort_timer.start();
				contractions->re_sort();
				sort_timer.stop();
			}
			break;
		case OK:	// Vertex added
			contractions->fast_update((*mesh).vertices.back());
			contractions->pop_top();
			sort_timer.start();
			if(sort)contractions->re_sort();
			sort_timer.stop();
			break;

	}

	pop_timer.stop();
}

ContStatus PMFactory::new_pmvertex(const Contraction& cont)
{
	np_timer.start();

	bool cond = false;

	PMVertex pmv(cont);

	pmv.parent = -1;

	fn_timer.start();
	ContStatus cs = initiate_fn(pmv);
	fn_timer.stop();
	
	switch(cs){
		case INVALID:
		case SUBDUE:
				np_timer.stop();
				return cs;
	}
	
	pmv.tc.resize(mesh->tex.cnt());
	for(int i = 0; i < mesh->tex.cnt(); i++)
		pmv.tc[i] = cont.tc[i];

	// Setup a temporary pmv
	pmv.fni[0] = vfrelations->get_exclusive_vertex_ii(pmv.fn[0], pmv.flr[0]);
	pmv.fni[1] = vfrelations->get_exclusive_vertex_ii(pmv.fn[1], pmv.flr[0]);
	pmv.fni[2] = vfrelations->get_exclusive_vertex_ii(pmv.fn[2], pmv.flr[1]);
	pmv.fni[3] = vfrelations->get_exclusive_vertex_ii(pmv.fn[3], pmv.flr[1]);


	(*mesh).vertices.push_back(pmv);

	int pmv_index = (*mesh).vertices.size() - 1;

	// Update childrens parent status
	(*mesh).vertices[pmv.vt].parent = pmv_index;
	(*mesh).vertices[pmv.vu].parent = pmv_index;

	// Collapse
	(*mesh).col(pmv_index);

	// Create normal for new vertex
	vfrelations->create_nice_normals(pmv_index);
	/*
	cout << "vt: " << (*mesh).vertices[pmv.vt].normal << "\n";
	cout << "vu: " << 1(*mesh).vertices[pmv.vu].normal << "\n";
	cout << "res:" << (*mesh).vertices[pmv_index].normal << "\n";
	cin.get();
	*/

	for(int i = 0; i < 4; i++)if(pmv.fn[i] != -1)vfrelations->update_face_normal(pmv.fn[i]);

	// Update associated quadrics

	quad_timer.start();
	vfrelations->update_vertex_quadric(pmv_index);
	quad_timer.stop();

	np_timer.stop();
	return OK;

}

ContStatus PMFactory::initiate_fn(PMVertex& pmv)
{
	int i;

	bool cond = false;//(pmv.vt == 8) || (pmv.vu == 55);

	pmv.flr_area = 0.0;

	pmv.fn[0] = pmv.fn[1] = pmv.fn[2] = pmv.fn[3] = -1;


	std::set<int> vu_faces;
	vfrelations->get_vertex_faces(pmv.vu, vu_faces);

	std::set<int> vt_faces;
	vfrelations->get_vertex_faces(pmv.vt, vt_faces);

	std::set<int> collapsing;
	vfrelations->get_common_vertex_faces(pmv.vu, pmv.vt, collapsing);

	if(collapsing.size() > 2){
		return SUBDUE;
	}

	// Finding vl == 0, vr == 1
	int v[2] = {-1, -1}; 
	std::set<int> v_faces[2];

	// Initiate fl and fr
	pmv.flr[0] = vfrelations->get_left_face(pmv.vu, pmv.vt);
	pmv.flr[1] = vfrelations->get_right_face(pmv.vu, pmv.vt);

	if( (pmv.flr[0] == -1) && (pmv.flr[1] == -1) ){
		throw MeshException("initiatefn: zero flr");
	}
	//pmv.flr[0] = pmv.flr[1] = -1;

	for(i = 0; i < 2; i++){
		if(pmv.flr[i] != -1){
			v[i] = vfrelations->three_exclude_two((*mesh).get_face_vertex(pmv.flr[i], 0), 
		 										  (*mesh).get_face_vertex(pmv.flr[i], 1), 
												  (*mesh).get_face_vertex(pmv.flr[i], 2), 
												  pmv.vu, 
												  pmv.vt);

			vfrelations->get_vertex_faces(v[i], v_faces[i]);

			// we wish to avoid having fl0/fl1 or fl2/fl3 neigbouring eachother (geometry may corrupt)
			//if(v_faces[i].size() <= 3)return SUBDUE;

		} else {
			v[i] = -1;
		}
	}

	vfrelations->face_assure_vertex_zero(pmv.flr[0], pmv.vt);
	vfrelations->face_assure_vertex_zero(pmv.flr[1], pmv.vt);

	// Setup fn

	// Left Side
	if(pmv.flr[0] != -1){
		pmv.fn[0] = (*mesh).faces[pmv.flr[0]].nf[1];
		pmv.fn[1] = (*mesh).faces[pmv.flr[0]].nf[0];
	} else {
		pmv.fn[0] = pmv.fn[1] = -1;
	}

	// Right Side
	if(pmv.flr[1] != -1){
		pmv.fn[2] = (*mesh).faces[pmv.flr[1]].nf[2];
		pmv.fn[3] = (*mesh).faces[pmv.flr[1]].nf[0];
	} else {
		pmv.fn[2] = pmv.fn[3] = -1;
	}

	int fni = 0;
	i = 0;

	// fn's aren't allowed to neighbour eachother
	int fn0t_index, fn1u_index, fn2t_index, fn3u_index;
	fn0t_index = fn1u_index = fn2t_index = fn3u_index = -1;
	
	if( (pmv.fn[0] != -1) && (pmv.fn[1] != -1)){
		fn0t_index = vfrelations->find_face_vertex_index(pmv.fn[0], pmv.vt);
		if( (*mesh).faces[pmv.fn[0]].nf[fn0t_index] == pmv.fn[1]) return SUBDUE;
	}

	if( (pmv.fn[2] != -1) && (pmv.fn[3] != -1)){
		fn2t_index = vfrelations->find_face_vertex_index(pmv.fn[2], pmv.vt);
		if( (*mesh).faces[pmv.fn[2]].nf[fn2t_index] == pmv.fn[3]) return SUBDUE;
	}

	if( vfrelations->dangerous_neighbours(pmv) ){
	//	cout << "\n		         Got a loud neighbour there, Pierce";
		return SUBDUE;
	}

	if( vfrelations->dangerous_normals(pmv) ){
	//	cout << "\n              Hah! Caught a dangerous normal!";
		return SUBDUE;
	}

	// avoid singularities, that is pure edges
	if( (pmv.flr[0] != -1) && (pmv.fn[0] + pmv.fn[1] <=-2) )return INVALID;

	if( (pmv.flr[1] != -1) && (pmv.fn[2] + pmv.fn[3] <=-2) )return INVALID;

	// Avoid non-manifolds if can. It's expensive!
	if( (pmv.flr[0] != -1) && (pmv.flr[1] != -1)){
		//cout << "\nvl/vr: " << v[0] << " / " << v[1] << "\n";
		if( !vfrelations->faces_right_fan_connected(pmv.fn[1], v[0], pmv.fn[3]) && 
			!vfrelations->faces_right_fan_connected(pmv.fn[2], v[1], pmv.fn[0])){
			//cout << "\n				no fan connection...subdue!";
			return SUBDUE;
		}
	} 
	
	// Avoid fn-solitude
	if( (vfrelations->face_neighbour_count(pmv.fn[0]) == 1) && (pmv.fn[1] == -1) )return INVALID;
	if( (vfrelations->face_neighbour_count(pmv.fn[1]) == 1) && (pmv.fn[0] == -1) )return INVALID;
	if( (vfrelations->face_neighbour_count(pmv.fn[2]) == 1) && (pmv.fn[3] == -1) )return INVALID;
	if( (vfrelations->face_neighbour_count(pmv.fn[3]) == 1) && (pmv.fn[2] == -1) )return INVALID;

	/*
	for(i = 0; i < 4; i++)if( (pmv.fn[i] != -1) && ((pmv.fn[i] == pmv.flr[0]) || (pmv.fn[i] == pmv.flr[1])) ){
		pmv.report(-1);
		(*mesh).faces[pmv.flr[0]].report(1);
		(*mesh).faces[pmv.flr[1]].report(1);
		throw MeshException("initiate_fn: Incoherent neighbour.");
	}

	
	for(i = 0; i < 4; i++)if((pmv.fn[i] != -1) && !(*mesh).faces[pmv.fn[i]].is_active){
		cout << "\n\nfn" << i << " pointed at " << pmv.fn[i] << "\n";
		pmv.report(-1);
		vfrelations->face_report(pmv.fn[i], true);
		vfrelations->face_report(105, true);
		throw MeshException("initiate_fn: Added inactive fn-element");
	}

	if( (pmv.flr[0] == -1) && (pmv.flr[1] == -1) ){
		pmv.report(-1);
		(*mesh).vertices[pmv.vu].report(pmv.vu);
		(*mesh).vertices[pmv.vt].report(pmv.vt);
		throw MeshException("initiate_fn: fl and fr nonexisting.");
	}
	*/
	
	// Setup flr_area
	if(pmv.flr[0] != -1)pmv.flr_area += vfrelations->face_area(pmv.flr[0]);
	if(pmv.flr[1] != -1)pmv.flr_area += vfrelations->face_area(pmv.flr[1]);


	return OK;

}


void PMFactory::new_vertex(const Point& p, const Point& n, const TexCoord* t)
{

	int i;
	// Derive point property and normal
	PMVertex pmv;
	pmv.point = p;
	pmv.normal = n;

	pmv.tc.resize(mesh->tex.cnt());
	for(i = 0; i < mesh->tex.cnt(); i++)
		pmv.tc[i] = t[i];

	pmv.color.x = 0.25 + 0.65 * ((float) (rand() % 10)) / 10.0;
	pmv.color.y = 0.25 + 0.65 * ((float) (rand() % 10)) / 10.0;
	pmv.color.z = 0.25 + 0.65 * ((float) (rand() % 10)) / 10.0;

	// Parent -1 until collapse
	pmv.parent = -1;

	// Children will always be -1
	pmv.vu = pmv.vt = pmv.flr[0] = pmv.flr[1] = -1;

	for(i = 0; i < 4; i++)pmv.fn[i] = -1;

	// Add to SRMesh and to its active list
	(*mesh).vertices.push_back(pmv);
	(*mesh).add_active_vertex((*mesh).vertices.size() - 1);
}

void PMFactory::new_face(int a, int b, int c)
{
	PMFace pmf;

	pmf.vertices[0] = a;
	pmf.vertices[1] = b;
	pmf.vertices[2] = c;

	pmf.color.x = 0.25 + 0.65 * ((float) (rand() % 10)) / 10.0;
	pmf.color.y = 0.25 + 0.65 * ((float) (rand() % 10)) / 10.0;
	pmf.color.z = 0.25 + 0.65 * ((float) (rand() % 10)) / 10.0;


	// Add to SRMesh and to its active list
	(*mesh).faces.push_back(pmf);
	int face_index = (*mesh).faces.size() - 1;
	(*mesh).add_active_face(face_index);

	Vector kv1 = (*mesh).vertices[(*mesh).faces[face_index].vertices[1]].point - (*mesh).vertices[(*mesh).faces[face_index].vertices[0]].point;
	Vector kv2 = (*mesh).vertices[(*mesh).faces[face_index].vertices[2]].point - (*mesh).vertices[(*mesh).faces[face_index].vertices[0]].point;
		
	(*mesh).faces[face_index].normal = norm(cross(kv1, kv2));

}

void PMFactory::build()
{

	int lb = (lower_bound+2);

	build_timer.start();

	non_prog_limit = (*mesh).vertices.size();
	
	vfrelations = new VFRelations((*mesh));
	contractions = new Contractions((*mesh), *vfrelations);
	vfrelations->setup_face_neighbours();
	vfrelations->create_nice_normals();

	cout << "PMFactory::build -> Generating pairs...\n";
	generate_pairs();

	cout << "PMFactory::build -> Initiating contractions...\n";
	if( (*mesh).active_faces.size() > lb) initiate_contractions();

	cout << "PMFactory::build -> Building mesh...\n";

	int dcount = 0;
	int i;
	int max = contractions->size();
	int delta = max / 20;
	if(delta == 0)delta = 1;

	while((!contractions->empty()) && ((*mesh).active_faces.size() > lb)){

		pop_and_update();
		// show progress in console
		
		if(dcount % 100 == 0){

			cout << "\r|";
			for(i = 0; i < (max - contractions->size()); i += delta)
				cout << "*";
			for(;i < max; i+=delta)
				cout << "-";
			cout << "| " << contractions->size() << "(" << (*mesh).active_faces.size() << ")\t\t";
		}
		

	}

	cout << "\rPMFactory::build -> Sweeping afterpass...\n";

	after_pass();

	virgin = false;

	// Minimize the mesh
	for(int j = non_prog_limit; j < mesh->vertices.size(); j++){
		mesh->col(j);
	}


	build_timer.stop();

	cout << "PMFactory::build -> Completed after " << floor(build_timer.time() / 60.0) << " minutes and " << ((int) build_timer.time()) % 60 << " seconds (" << build_timer.time() << ")\n";
	cout << "PMFactory::build -> fn: " << initfn_timer.time() 
		                               << "   sorting: " << sort_timer.time() 
									   << "   quad: " << quad_timer.time() << "\n";
    cout << "PMFactory::build -> pop_update: " << update_timer.time()
									   << " subdue: " << subdue_timer.time() << " new_pmv: " << np_timer.time() << "\n";

}
/*
void PMFactory::report_integrity()
{
	cout << "\n";
	cout << "--------------------------------\n";
	cout << "| PMFactory reporting integrity|" << "\n";
	cout << "--------------------------------" << "\n";
	for(int i = 0; i < (*mesh).vertices.size(); i++){

		if((*mesh).vertices[i].vt != -1){

			int vt = (*mesh).vertices[i].vt;

			cout << "vertex: " << i << "...";

			int fl0;
			int fr0;

			int fl = (*mesh).vertices[i].flr[0];
			int fr = (*mesh).vertices[i].flr[1];

			if(fl != -1){
				fl0 = (*mesh).get_active_vertex_relative((*mesh).faces[fl].vertices[0]);
				if(vt != fl0) cout << "vertex #" << i << "\tvt (" << vt << ") differs from fl0 (" << fl0 << ") (fl: " << fl <<")\n";
				else cout << "ok\n";
			} else if(fr != -1){
				fr0 = (*mesh).get_active_vertex_relative((*mesh).faces[fr].vertices[0]);
				if(vt != fr0) cout << "vertex #" << i << "\tvt differs from fr0\n";
				else cout << "ok\n";
			} else cout << "ok\n";
		}

	}
	cout << "\n\n";
}
*/
void PMFactory::render()
{
	if(virgin)(*mesh).render(&(*mesh).vertices.back());
	else (*mesh).render();
	PMVertex& pmv = (*mesh).vertices.back();
	contractions->render();
	(*mesh).vertices.back().render();
/*
	int x, y, z;
	if( pmv.fn[0] != -1){
		int fn0t_index = vfrelations->find_face_vertex_index(pmv.fn[0], pmv.vt);
		if(fn0t_index != -1){
			int fi = (*mesh).faces[pmv.fn[0]].nf[fn0t_index];

			glBegin(GL_TRIANGLES);

			glColor3f(1.0, 1.0, 1.0);

			for(int j = 0; j < 3; j++){
				x = (*mesh).vertices[(*mesh).get_active_vertex_relative((*mesh).faces[fi].vertices[j])].point.x;
				y = (*mesh).vertices[(*mesh).get_active_vertex_relative((*mesh).faces[fi].vertices[j])].point.y;
				z = (*mesh).vertices[(*mesh).get_active_vertex_relative((*mesh).faces[fi].vertices[j])].point.z;
				glVertex3f(x, y, z);
			}

			glEnd();
		}
	}
*/			


}


void PMFactory::report_faces()
{
	cout << "\n";
	cout << "----------------------------\n";
	cout << "| PMFactory reporting faces|" << "\n";
	cout << "----------------------------" << "\n";
	cout << "#\tv0\tv1\tv2\n";
	for(int i = 0; i < (*mesh).faces.size(); i++)
		cout << i << "\t" << (*mesh).faces[i].vertices[0] << "\t" << (*mesh).faces[i].vertices[1] << "\t" << (*mesh).faces[i].vertices[2] << "\n";

}


void PMFactory::report(bool verbose)
{
	cout << "\n";
	cout << "-----------------------\n";
	cout << "| PMFactory reporting |" << "\n";
	cout << "-----------------------" << "\n";
	cout << "Vertex count:\t " << (*mesh).vertices.size() << "\n";
	cout << "Faces count:\t" << (*mesh).faces.size() << "\n";
	cout << "\n";
	cout << "Active Vertex count:\t " << (*mesh).active_vertices.size() << "\n";
	cout << "Active Faces count:\t" << (*mesh).active_faces.size() << "\n";
	cout << "\n";

	if(verbose){
		cout << "#\tactive\tvt\tvu\tparent\tx\ty\tz\tfl\tfr\n";
		for(int i = 0; i < (*mesh).vertices.size(); i++){
			cout << i << "\t" << (*mesh).vertices[i].is_active << "\t" << (*mesh).vertices[i].vt << "\t" << (*mesh).vertices[i].vu << "\t" << (*mesh).vertices[i].parent << "\t";
			cout << (*mesh).vertices[i].point.x << "\t";
			cout << (*mesh).vertices[i].point.y << "\t";
			cout << (*mesh).vertices[i].point.z << "\t";
			cout << (*mesh).vertices[i].flr[0] << "\t";
			cout << (*mesh).vertices[i].flr[1] << "\t";
			cout << (*mesh).vertices[i].fn[0] << "\t";
			cout << (*mesh).vertices[i].fn[1] << "\t";
			cout << (*mesh).vertices[i].fn[2] << "\t";
			cout << (*mesh).vertices[i].fn[3] << "\n";
		}
	}


	
}

void PMFactory::report_active_configuration()
{
	cout << "\n";
	cout << "--------------------------------------------\n";
	cout << "| PMFactory reporting active configuration |" << "\n";
	cout << "--------------------------------------------" << "\n";
	std::list<int>::const_iterator li;

	for(li = (*mesh).active_faces.begin(); li != (*mesh).active_faces.end(); li++)
		vfrelations->face_report(*li, true);

	cout << "\n";

	moli::iterator mi;
	for(mi = (*mesh).active_vertices.begin(); mi != (*mesh).active_vertices.end(); mi++)
		vfrelations->vertex_report((*mi).id, true);

	cout << "\n";

}


}
}
}
