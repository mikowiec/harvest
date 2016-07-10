#ifndef REAPER_MESH_PMRMORPH_H
#define REAPER_MESH_PMRMORPH_H

#include "pmrealtimesys.h"
#include "gfx/vertex_array.h"
#include "gfx/displaylist.h"

namespace reaper 
{
namespace gfx
{
namespace mesh
{

typedef std::vector< gfx::mesh::TexCoord > TexCoordVector;

class PMRMorph : public PMRealTimeSys
{
private:

	Timer alpha_timer;

        std::vector<GeomCore> vertex_array;
	std::vector<TexCoordVector> texture_array;
	std::vector<unsigned int> face_array;

	void force_vsplit(moli::iterator& li);
	bool ecol(int vi, moli::iterator& li);
	void backtrace_ecol(int vi, moli::iterator& li);
	void vsplit(int vi, moli::iterator* li);

	void morph_col(int vi);
	void morph_split(int vi, moli::iterator* li);
	void morph_pass();
	void update_alpha(float& alpha);
	void morph_col_vertex(MorphNode& mn);
	void morph_split_vertex(MorphNode& mn);

	GeomCore* morph_point(int vi);

	int find_collapsor(int fi);
	bool vertex_setup_zss(int vi);
	bool vertex_setup_iff(int vi);

	void setup_render_arrays();

	void render_debug();


	bool look_for_fff(int vi);

	gfx::misc::DisplayList reg_comb;

public:
	PMRMorph(SRMesh& srmesh) : PMRealTimeSys(srmesh)
        {}

	// ----------------
	bool col_debug;
	int mcnt;
	// ----------------

	void fast_render();
	void fancy_render();
	void refine();
	void render();

	void spec_report(bool verbose);

};
		

}
}
}

#endif
