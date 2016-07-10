/*
 * $Author: pstrand $
 * $Date: 2003/06/11 15:16:42 $
 * $Log: terrain.cpp,v $
 * Revision 1.85  2003/06/11 15:16:42  pstrand
 * småfixar..
 *
 * Revision 1.84  2003/06/04 16:29:59  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.83  2003/01/11 23:58:43  fizzgig
 * improved opengl implementation reporting
 *
 * Revision 1.82  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.81  2002/05/29 00:35:12  fizzgig
 * cleanup, small fixes, improvement, etc
 *
 * Revision 1.80  2002/05/22 10:04:46  fizzgig
 * no message
 *
 * Revision 1.79  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.78  2002/02/18 11:51:22  peter
 * no need to set dim..
 *
 * Revision 1.77  2002/02/10 13:58:40  macke
 * cleanup
 *
 * Revision 1.76  2002/01/31 05:35:05  peter
 * *** empty log message ***
 *
 * Revision 1.75  2002/01/27 19:46:03  peter
 * *** empty log message ***
 *
 * Revision 1.74  2002/01/23 04:42:49  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.73  2002/01/22 23:44:03  peter
 * reversed last two revs
 *
 * Revision 1.71  2002/01/16 00:25:28  picon
 * terrängbelysning
 *
 * Revision 1.70  2002/01/10 23:09:09  macke
 * massa bök
 *
 * Revision 1.69  2001/12/14 16:31:25  macke
 * meck å pul
 *
 * Revision 1.68  2001/10/01 17:36:32  macke
 * Hmm.. grafikmotorn äger objekt som håller en ref-ptr till grafikmotorn.. fel!
 *
 * Revision 1.67  2001/08/20 16:12:52  peter
 * stop time..
 *
 * Revision 1.66  2001/07/31 23:19:16  macke
 * Lite mer benchmarking i grafikmotorn...
 *
 * Revision 1.65  2001/07/31 21:57:56  macke
 * Nytt försök med vertexarrays..
 *
 * Revision 1.64  2001/07/30 23:43:19  macke
 * Häpp, då var det kört.
 *
 * Revision 1.63  2001/07/12 13:53:48  peter
 * glDrawElements...
 *
 * Revision 1.62  2001/07/09 13:47:52  peter
 * gcc-3.0 fixar
 *
 * Revision 1.61  2001/07/06 01:47:14  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.60  2001/06/09 01:58:53  macke
 * Grafikmotor reorg
 *
 * Revision 1.59  2001/06/07 05:14:20  macke
 * Reaper v0.9
 *
 * Revision 1.58  2001/05/25 09:54:44  peter
 * ljudfixar..
 *
 * Revision 1.57  2001/05/15 03:44:55  macke
 * *** empty log message ***
 *
 * Revision 1.56  2001/05/15 01:42:27  peter
 * minskade ner på debugutskrifterna
 *
 * Revision 1.55  2001/05/15 01:28:08  truls
 * terrain->render does it all...
 *
 * Revision 1.54  2001/05/14 20:00:53  macke
 * bugfix och rök på missiler..
 *
 * Revision 1.53  2001/05/14 15:05:46  truls
 * time flows like a river
 *
 * Revision 1.52  2001/05/10 14:32:33  macke
 * no message
 *
 * Revision 1.51  2001/05/10 11:40:17  macke
 * häpp
 *
 */

#include "os/compat.h"

#include <string>
#include <iterator>
#include <vector>
#include <iostream>

#include "main/types.h"
#include "main/types_io.h"
#include "os/gl.h"

#include "gfx/managers.h"
#include "gfx/interfaces.h"
#include "gfx/io.h"
#include "gfx/settings.h"
#include "gfx/pm/pm.h"
#include "gfx/vertex_array.h"

#include "res/res.h"
#include "world/world.h"
#include "os/debug.h"

namespace reaper {
namespace gfx {
namespace {
template<typename T>
void print_vector(const std::vector<T> &v)
{
	std::copy(v.begin(),v.end(),std::ostream_iterator<T>(std::cout));
	std::cout << std::endl;
}

debug::DebugOutput dout("gfx::terrain",0);
}

int Terrain::num_triangles = 0;
int Terrain::num_vertices = 0;

/// Terrain data
class Terrain::Impl
{
	/// &Vertex styled to fit the GeForce CVA optimized format
	class CVAvertex
	{
	public:
		Point p;
		TexCoord t;
		ByteColor c;

		void operator=(const ByteColor &_c) { c = _c; }
		void operator=(const TexCoord &_t)  { t = _t; }
		void operator=(const Point &_p)     { p = _p; }
	};

	std::vector<int>       index_array;
	lowlevel::VertexArray<CVAvertex> vertex_array;
	
	int array_size;
	int n_vertices;
        
	std::auto_ptr<pm::Pmd> pmd;
        float detail_repeats;      
	std::string main_tex, detail_tex;
	TextureRef tr;

	/// Transfers terrain data to video/agp memory and locks it down
	void load_geometry();
public:
        Impl(const string &mesh_id, const string &main_id, const string &detail_id, float dr);

	void render(const Camera &c);
	pm::Pmd * get_pmd() { return pmd.get(); }
};

Terrain::Impl::Impl(const string &mesh_id, const string &main_id, const string &detail_id, float dr) : 
	array_size(0), n_vertices(0), pmd(new pm::Pmd),
        detail_repeats(dr),
	main_tex(main_id), detail_tex(detail_id),
	tr(TextureMgr::get_ref())
{
	using std::vector;
	
	lowlevel::MeshData md;
	lowlevel::read_mesh(mesh_id, md);

	// Compute terrain dimensions
	float max_x = 0;
	float min_x = 0;
	float max_z = 0;
	float min_z = 0;

	for (int i=0; i < md.points.size(); ++i) {
		max_x = max(md.points[i].x, max_x);
		max_z = max(md.points[i].z, max_z);
		min_x = min(md.points[i].x, min_x);
		min_z = min(md.points[i].z, min_z);
	}

	n_vertices = md.points.size(); 
	array_size = sizeof(CVAvertex)*n_vertices;
	vertex_array.resize(n_vertices);

	// Default material
	Material m;

	m.ambient_color = Color(1,1,1);
	m.diffuse_color = Color(.7,.7,.7);
	m.specular_color = Color(0,0,0);
	m.emission_color = Color(0,0,0);
	m.shininess = 0;

	LightRef lr = LightMgr::get_ref();
	if(!lr.valid()) {
		dout << "Light manager not initialized. Colors not calculated.\n";
	} 

	// reserve place in arrays to avoid realloc
	pmd->vdl.reserve(md.points.size());
	for(int i=0;i<md.points.size();++i) {
		if(lr.valid()) {
			md.colors[i] = lr->calc_diffuse_light(md.points[i], md.normals[i], m);
		} else {
			clamp(md.colors[i]);
		}

		pmd->addv(pm::Vertex(md.points[i],md.colors[i],md.texcoords[i]));

		vertex_array[i] = md.points[i];
		vertex_array[i] = md.texcoords[i];
		vertex_array[i] = md.colors[i];
	}

	md.points.resize(0);
	md.texcoords.resize(0);
	md.colors.resize(0);

	// reserve place in arrays to avoid realloc
	index_array.resize(md.triangles.size()*3);
	pmd->fl.reserve(md.triangles.size());

	vector<int>::iterator idx = index_array.begin();
	

	world::WorldRef wr;
	vector<IdxTriangle>::const_iterator i, e = md.triangles.end();
	for (i = md.triangles.begin(); i != e; ++i) {
		pmd->addf(i->v1, i->v2, i->v3);	

		*idx++ = i->v1;
		*idx++ = i->v2;
		*idx++ = i->v3;

		const Point &p1 = pmd->vdl[i->v1].position.p;
		const Point &p2 = pmd->vdl[i->v2].position.p;
		const Point &p3 = pmd->vdl[i->v3].position.p;

		wr->add_object(new world::Triangle(p1,p2,p3));
	}

	// preload textures
	tr->load(main_id);
	tr->load(detail_id);
}

void Terrain::Impl::render(const Camera &c)
{
	if(!Settings::current.draw_terrain)
		return;

	static bool mesh_made = false;	

	if(Settings::current.terrain_detail < 1.0 && !mesh_made) {
		mesh_made = true;
		dout << "Making progressive mesh\n";
		pmd->set_num_con_lim(25000.0);
		pmd->set_alpha_threshold(1.6E-9);
		pmd->set_update_steps(16);
		pmd->makeprog(); 
	} 

	int n_active_vertices = 0;
	if(mesh_made) {
		OldCamera tc(c.pos, c.pos + c.front, c.up, c.horiz_fov, c.vert_fov);
		pmd->set_cam(tc);
		pmd->update_fast();
		pmd->update();
		n_active_vertices = pmd->progmesh.size();
	} else {
		n_active_vertices = index_array.size();	
	}
	
	Terrain::num_vertices  = n_active_vertices;
	Terrain::num_triangles = n_active_vertices/3;

	// setup vertex arrays
	ClientStateKeeper s1(GL_TEXTURE_COORD_ARRAY, true);
	ClientStateKeeper s2(GL_COLOR_ARRAY, true);	
	glTexCoordPointer(2,GL_FLOAT,sizeof(CVAvertex),&vertex_array[0].t);

	tr->use(main_tex);

	if(Settings::current.use_arb_multitexturing && Settings::current.use_arb_texture_env_combine) {
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD_SIGNED_ARB);

		glClientActiveTextureARB((GLenum)GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,sizeof(CVAvertex),&vertex_array[0].t);

		glActiveTextureARB((GLenum)GL_TEXTURE1_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		tr->use(detail_tex);
		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glLoadIdentity();
		glScalef(detail_repeats,detail_repeats,1);

	}

	glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(CVAvertex),&vertex_array[0].c);			
	glVertexPointer(3,GL_FLOAT,sizeof(CVAvertex),&vertex_array[0].p);		
	
	const int *index_ptr = mesh_made ? &(pmd->progmesh)[0] : &index_array[0];
	vertex_array.glDrawElements(GL_TRIANGLES, n_active_vertices, GL_UNSIGNED_INT, index_ptr);

	if(Settings::current.use_arb_multitexturing && Settings::current.use_arb_texture_env_combine) {
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		tr->use("");

		glClientActiveTextureARB((GLenum)GL_TEXTURE1_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glClientActiveTextureARB((GLenum)GL_TEXTURE0_ARB);

		glActiveTextureARB((GLenum)GL_TEXTURE0_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	throw_on_gl_error("TerrainImpl::render()");
}

Terrain::Terrain(const string &mesh_id, const string &main_id, const string &detail_id, float dr) :
	i(new Impl(mesh_id, main_id, detail_id, dr)) 
{}
Terrain::~Terrain() 
{}
void Terrain::render(const Camera &c) { i->render(c); }
pm::Pmd* Terrain::get_pmd() { return i->get_pmd();  }

}
}
