#ifndef REAPER_MESH_PMREALTIMESYS_H
#define REAPER_MESH_PMREALTIMESYS_H

#include "shared.h"
#include "srmesh.h"
#include "gfx/camera.h"
#include "texture.h"

namespace reaper 
{
namespace gfx
{
namespace mesh
{

class PMRealTimeSys 
{
protected:
	SRMesh& mesh;

	gfx::Camera cam;
	float cam_fov_cos;

	int lower_bound;
	int upper_bound;
	int max_stack_depth;
	int max_tex_pass;
	float tau;

	bool qrefine(int vi);
	bool ecol_legal(PMVertex& v);
	bool vsplit_legal(PMVertex& v);

	bool outside_view_frustum(PMVertex& v);
	inline bool oriented_away(PMVertex& v);
	inline float screen_space_error(PMVertex& v);

	virtual void force_vsplit(moli::iterator& li) =0;
	virtual void vsplit(int vi, moli::iterator* li) =0;

	void relocate_vertex(int vi, moli::iterator& li);


public:

	Timer frame_timer;
	Timer ref_timer;
	Timer render_timer;
	Timer test_timer;

	int render_cnt;
	int qrefine_cnt;
	int split_cnt;
	int stack_depth;

	SRMesh* get_mesh(){return &mesh;};

	PMRealTimeSys(SRMesh& srmesh):mesh(srmesh),tau(0.008), render_cnt(0), lower_bound(0), upper_bound(500), max_tex_pass(5){}

	void set_max_stack_depth(int param){max_stack_depth = param;};
	void set_max_tex_pass(int param){max_tex_pass = param;};
	void set_lower_bound(int param){lower_bound = param;};
	void set_upper_bound(int param){upper_bound = param;};
	void set_cam(Point origin, Point dir, float fov);

	void setup();

	int get_face_count(){return mesh.active_vertices.size();};

	void report(bool verbose);
	void report_texture();

	virtual void refine() =0;
	virtual void spec_report(bool verbose) =0;
	virtual void render();


};

}
}
}

#endif
