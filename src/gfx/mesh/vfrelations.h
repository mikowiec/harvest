#ifndef REAPER_MESH_VFRELATIONS_H
#define REAPER_MESH_VFRELATIONS_H


#include <vector>
#include <set>
#include "shared.h"
#include "srmesh.h"
#include "contractionref.h"
#include "quadrics.h"

namespace reaper 
{
namespace gfx
{
namespace mesh
{

/************************************************************************
*					        CLASS: VFRelations
*
* Intended to keep track of which faces belongs to which vertices
* during PMFactorization. Also sports some handy helpers.
*
*************************************************************************/
class VFRelations {
private:
	SRMesh& mesh;

	std::vector< std::list<ContractionRef> > conts;

	std::vector<Quadric*> quad;

	std::vector<vint> verts;
	std::vector<bool> face_status;

	

	void add_face_data(int fid, int v0, int v1, int v2);

public:
	VFRelations(SRMesh& srmesh);
	~VFRelations(){};

	void insert_contraction(ContractionRef c_ref);

	void remove_contraction(ContractionRef c_ref, int vid);
	void remove_contraction(ContractionRef c_ref);

	void update_conts_at_vertex(int vid, PMVertex& pmv);

	void verify_no_cont_references(ContractionRef cr);


	void common(const std::set<int>& s1, const std::set<int>& s2, std::set<int>& target) const;
	bool common_but_not(const std::set<int>& s1, const std::set<int>& s2, int exclude, int& res) const;
	bool common_but_not(const std::set<int>& s1, const std::set<int>& s2, int exclude1, int exclude2, int& res) const;
     
	int get_smallest_face(std::set<int>& source) const;

	int get_left_face(int up, int down);
	int get_right_face(int up, int down);

	int three_exclude_two(int t1, int t2, int t3, int e1, int e2) const;

	void get_vertex_faces(int vid, std::set<int>& target) const;

	void get_common_vertex_faces(int v1, int v2, std::set<int>& target) const;

	void exclude_one_from_set(int exclude, std::set<int>& target) const;

	bool faces_currently_equal(std::set<int>& source) const;

	bool faces_currently_equal(int f1, int f2) const;

	float face_area(int fid) const;

	void face_assure_vertex_zero(int fi, int vi);

	int find_face_vertex_index(int fi, int vi);

	void remove_currently_equal_faces(int fid, std::set<int>& target) const;

	void verify_vertices_have_conts() const;

	int one_on_edge(int v1, int v2) const;

	bool vertex_on_edge(int vid) const;

	void get_vertex_neighbours(int vid, std::set<int>& target) const;

	int vertex_cont_count(int vid) const;

	void update_vertex_quadric(int vid);

	Quadric* get_quad(int vid) const;

	void setup_face_neighbours();

	void report();

	void vertex_report(int vid, bool verbose);

	void printset(const std::set<int>& s, char* info) const;
	void print_vertex_rel(int vid) const;

	int vertex_parent(int vid) const;

	void face_report(int fid, bool verbose) const;

	void check_neighbours(int fi);

	bool update_face_normal(int fi);

	bool dangerous_normals(PMVertex& pmv);

	bool dangerous_neighbours(PMVertex& pmv);

	bool test_face_neighbour_switch(int src, int old_dest, int new_dest);

	bool faces_right_fan_connected(int src, int vi, int dst);

	int face_neighbour_count(int fi);

	int get_exclusive_vertex_ii(int src, int match);

	void create_nice_normals(int vi);
	void create_nice_normals();

};

}
}
}

#endif
