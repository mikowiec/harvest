#include "os/compat.h"

#include <sstream>

#include "gfx/renderer.h"
#include "gfx/interfaces.h"
#include "gfx/misc.h"
#include "gfx/settings.h"
#include "gfx/texture.h"

#include "misc/font.h"

namespace reaper {
namespace gfx {
namespace lowlevel {
namespace {
char *profilertext[] = {
	"sky      ",
	"terrain  ",
	"water    ",
	"object fi",
	"object rd", 
	"effect rd", 
	"lights   ", 
	"shadows  ", 
	"hud      ", 
	"clear    ",
	"simulate ",
	"cubemap  "
};

char *countertext[] = {
	"silly   ",
	"static  ",
	"dynamic ",
	"shot    ",
	"effect  ",
	"shadow  "
};

template<class T>
void put_stat(const char *str, T val, float x, float y) 
{
	std::ostringstream ss;
	ss << str << val;
	reaper::gfx::font::glPutStr(x,y, ss.str().c_str());
}
} // end anonymous namespace

Statistics::Statistics(Renderer *r_) :
	tr(TextureMgr::get_ref()), mr(MeshMgr::get_ref()), lr(LightMgr::get_ref()),
	r(r_),
	cnt("stat_counters"), prf("stat_profilers")
{
	char **pt = profilertext;
	float y = .23;
	for(int i = Profiles_Begin; i != Profiles_End; i++) {
		prf.insert(new reaper::misc::time::Profiler2(*pt++, 0, y, .3, .025, 300),
			   static_cast<Profiles>(i));
		y += .025;
	}

	reset();

	proj_setup.begin();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	proj_setup.end();

	proj_restore.begin();
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	proj_restore.end();

}

void Statistics::reset()
{
	char **ct = countertext;
	cnt.purge();
	for(int i = Counters_Begin; i != Counters_End; i++) {
		cnt.insert(new std::pair<int, std::string>(0, std::string(*ct++)), 
		           static_cast<Counters>(i));
	}
}

void Statistics::setup()
{
	proj_setup.call();
}

void Statistics::restore()
{
	proj_restore.call();
}

void Statistics::render()
{
	using gfx::font::glPutStr;
	using gfx::misc::meter;
	using namespace os::time;
	static TimeSpan then = get_time();

	TimeSpan now = get_time();
	float dt = (now - then).approx().to_s();
	then = now;


	setup();

	// fallthrough
	switch(Settings::current.draw_stats) {
	case 4:
		{
		float frame_length = 1e6/50.0;

		glColor3f(1,1,1);
		for(PrfMap::iterator i = prf.begin(); i != prf.end(); ++i) {
			i->draw_txt_short();
		}

		tr->use("");
		bool red = true;
		glPointSize(0.8);
		for(PrfMap::iterator i = prf.begin(); i != prf.end(); ++i) {
			glColor3f(0,0,1);
			i->draw_last(frame_length);
			glColor3f(0,1,0);
			i->draw_avg(frame_length);
			if(red) {
				glColor3f(1,0,0);
			} else {
				glColor3f(1,1,1);
			}
			i->draw_history(frame_length);
			red = !red;
		}
		}
	case 3:
		{
		glColor3f(1,1,1);
		int mesh_verts = mr->get_num_vertices();
		int mesh_tris = mr->get_num_triangles();
		int total_verts = Terrain::num_vertices + mesh_verts;
		int total_tris = Terrain::num_triangles + mesh_tris;
		mr->reset_stats();

		static int verts_per_sec = 0;
		static int tris_per_sec = 0;
		static int verts_accum = 0;
		static int tris_accum = 0;
		static float time = 0;

		verts_accum += total_verts;
		tris_accum += total_tris;
		time += dt;

		if(time >= 1) {
			tris_per_sec = static_cast<int>(tris_accum / time);
			verts_per_sec = static_cast<int>(verts_accum / time);
			time -= 1;
			tris_accum -= tris_per_sec;
			verts_accum -= verts_per_sec;
		}

		put_stat("Terrain verts: ", Terrain::num_vertices/1000,0,.725);
		put_stat("Meshes verts:  ", mesh_verts/1000,0,.7);
		put_stat("Total verts:   ", total_verts/1000,0,.675);

		put_stat("Terrain tris:  ", Terrain::num_triangles/1000,0,.65);
		put_stat("Meshes tris:   ", mesh_tris/1000,0,.625);
		put_stat("Total tris:    ", total_tris/1000,0,.6);

		put_stat("Verts/second:  ", verts_per_sec/1000,0,.575);
		put_stat("Tris/second:   ", tris_per_sec/1000,0,.55);
		}
	case 2:
		glColor3f(1,1,1);
		put_stat("Textures(k): ", texture::TextureBase::all_textures_size() >> 10,0,.8);
		put_stat("Resident(k): ", texture::TextureBase::resident_textures_size() >> 10,0,.775);
	case 1:
		{
		glColor3f(1,1,1);
		float pos = .8;
		for(CntMap::iterator i = cnt.begin(); i != cnt.end(); ++i) {
			put_stat(i->second.c_str(), i->first, .8, pos);
			pos -= .025;
		}

		put_stat("s_lights:", lr->num_static_lights(), .8, pos -= .025);
		put_stat("sim_effs:", r->simeff_size(), .8,  pos -= .025);
		put_stat("orp_effs:", r->orpeff_size(), .8,  pos -= .025);
		}
	default: break;
	}

	restore();
}

}
}
}

/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:02 $
 * $Log: statistics.cpp,v $
 * Revision 1.13  2003/01/06 12:42:02  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.12  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.11  2002/09/20 23:41:11  pstrand
 * *** empty log message ***
 *
 * Revision 1.10  2002/09/13 07:45:55  pstrand
 * gcc-varningar
 *
 * Revision 1.9  2002/05/29 00:35:12  fizzgig
 * cleanup, small fixes, improvement, etc
 *
 * Revision 1.8  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.7  2002/05/10 14:12:50  fizzgig
 * shadow code cleanup
 *
 * Revision 1.6  2002/04/29 20:28:29  pstrand
 * leak (Statistics::reset(), cnt.purge())
 *
 * Revision 1.5  2002/02/28 12:40:09  fizzgig
 * shadow optimization + better texture statistics
 *
 * Revision 1.4  2002/01/28 00:50:41  macke
 * gfx reorg
 *
 * Revision 1.3  2002/01/23 09:47:56  macke
 * hmf!
 *
 * Revision 1.1  2002/01/22 22:38:07  macke
 * massive gfx cleanup
 *
 */
