/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:36 $
 * $Log: water.cpp,v $
 * Revision 1.22  2002/12/29 17:04:36  pstrand
 * hw/ moved to os/
 *
 * Revision 1.21  2002/09/23 12:08:43  fizzgig
 * Moved all anonymous namespaces into the reaper-namespace
 *
 * Revision 1.20  2002/05/16 23:59:00  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.19  2002/03/24 17:35:51  pstrand
 * to_str -> ltos
 *
 * Revision 1.18  2002/01/26 23:27:14  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.17  2002/01/23 04:42:49  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.16  2002/01/22 23:44:03  peter
 * reversed last two revs
 *
 * Revision 1.14  2002/01/10 23:09:10  macke
 * massa bök
 *
 * Revision 1.13  2001/12/17 16:28:33  macke
 * div bök
 *
 * Revision 1.12  2001/12/14 16:31:26  macke
 * meck å pul
 *
 * Revision 1.11  2001/07/30 23:43:20  macke
 * Häpp, då var det kört.
 *
 * Revision 1.10  2001/07/09 13:47:53  peter
 * gcc-3.0 fixar
 *
 * Revision 1.9  2001/06/09 01:58:55  macke
 * Grafikmotor reorg
 *
 * Revision 1.8  2001/06/07 05:14:22  macke
 * Reaper v0.9
 *
 * Revision 1.7  2001/05/15 01:42:27  peter
 * minskade ner på debugutskrifterna
 *
 * Revision 1.6  2001/05/10 11:40:19  macke
 * häpp
 *
 */

#include "os/compat.h"

#include <string>
#include <iostream>
#include <vector>
#include <cmath>

#include "main/types.h"
#include "gfx/static_geom.h"
#include "gfx/interfaces.h"
#include "gfx/io.h"
#include "gfx/managers.h"
#include "gfx/settings.h"
#include "gfx/displaylist.h"

#include "os/gl.h"
#include "os/debug.h"

#include "misc/parse.h"

using reaper::misc::ltos;

namespace reaper {
namespace {
    debug::DebugOutput dout("gfx::water",0);
}
namespace gfx {

class Lake::Impl
{
	misc::DisplayList list;
	misc::StaticGeometry geom;
	string surface_tex;
	string reflect_tex;
	float  pos;
	Vector wave_dir;
	TextureRef tr;
	float time;
	float tex_time;
	int cur_tex;
public:
	Impl(const string &file, const string &texture, const Vector &wave_dir, float amplitude);
	void render(const world::Frustum &); 
	void simulate(float dt);
};
	
Lake::Impl::Impl(const string &file, const string &tex, const Vector &wd, float a) : 
	surface_tex(tex), reflect_tex("sky"), pos(0), wave_dir(wd), tr(TextureMgr::get_ref()),
	time(0), tex_time(0), cur_tex(0)
{
	using std::vector;

	lowlevel::MeshData mesh;
	lowlevel::read_mesh(file, mesh);

	//@todo tesselate triangles to waves
	for(int i=0;i<mesh.points.size();i++){
		mesh.texcoords[i].s *= 50;
		mesh.texcoords[i].t *= 50;
		mesh.points[i].y    += 150;
	}

	geom.init(&mesh.points[0], 0, 0, &mesh.texcoords[0], true,
		mesh.points.size(), &mesh.triangles[0].v1, mesh.triangles.size()*3);

	for(int i=0;i<32;++i) {
		tr->load(surface_tex + ltos(i, 2, '0'));
	}
}

void Lake::Impl::render(const world::Frustum &)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);			
	tr->use(surface_tex + ltos(cur_tex, 2, '0'));
	glPushMatrix();
	glTranslatef(0,2*sin(time),0);
	glColor4f(0,.75,1,.6);
	glNormal3f(0,1,0);
	geom.render();
	glPopMatrix();
}

void Lake::Impl::simulate(float dt) 
{
	time += dt;
	tex_time += dt;
	while(tex_time>1/30.0) {
		tex_time -= 1/30.0;
		cur_tex++;
		cur_tex %= 32;
	}
}

class River::Impl 
{};

////////////////////////////////////////////////////////////////////////////////

Lake::Lake(const std::string &file, const std::string &texture, const Vector &wave_dir, float amplitude) :
	i(new Impl(file,texture,wave_dir,amplitude)) {
}
Lake::~Lake() {}
void Lake::render(const world::Frustum &f) { i->render(f); }
void Lake::simulate(float dt) {	i->simulate(dt); }

River::River(const std::string &file, const std::string &texture, float speed) {
	throw gfx_fatal_error("River - not implemented!");
}
River::~River() {}
void River::render(const world::Frustum &) {}
void River::simulate(float dt) {}

}
}
