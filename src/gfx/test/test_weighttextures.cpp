#include "os/compat.h"

#include <sstream>
#include <list>
#include <map>
#include <iterator>

#include "gfx/gfx.h"
#include "gfx/misc.h"
#include "gfx/managers.h"
#include "gfx/exceptions.h"
#include "gfx/settings.h"
#include "gfx/io.h"
#include "main/types_io.h"
#include "os/debug.h"
#include "os/gfx.h"
#include "os/gl.h"
#include "msg/event.h"
#include "os/time.h"
#include "res/res.h"
#include "res/config.h"
#include "misc/stlhelper.h"
#include "misc/iostream_helper.h"
#include "misc/test_main.h"

#include "ext/nvparse.h"

#include "sys/stat.h"
#include "sys/types.h"

namespace reaper {
debug::DebugOutput dout("TestWeightTextures");

using reaper::misc::push_back;
using gfx::throw_on_gl_error;
using namespace std;

void test_and_apply_rc(const char *rc)
{
	nvparse(rc);

	bool error = false;
	for (const char* const* errors = nvparse_get_errors(); *errors; errors++) {
		error = true;
		dout << *errors << "\n";
	}

	if(error)
		exit(1);
}

// store weight in alpha channel
void setup_colors1(Color *src, Color *pre_light, float *alpha)
{
	for(int i=0; i<4; i++) {
		src[i].r = pre_light[i].r;
		src[i].g = pre_light[i].g;
		src[i].b = pre_light[i].b;
		src[i].a = alpha[i];
	}
}

// store weight in rgb channel (only channel that can be requested explicitly)
void setup_colors2(Color *src, Color *pre_light, float *alpha)
{
	for(int i=0; i<4; i++) {
		src[i].r = alpha[i];
		src[i].g = alpha[i];
		src[i].b = alpha[i];
	}
}

template<class T>
ostream& operator << (ostream &os, vector<T> &v)
{
	std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os," "));
	return os;
}

int foo()
{
	// init gl system

        os::gfx::Gfx gx;
	os::event::EventSystem es(gx);
	os::event::EventProxy ep = os::event::EventSystem::get_ref(os::event::System);

        gfx::TextureRef::create();
	gfx::TextureRef tr = gfx::TextureMgr::get_ref();

	gfx::Settings s;
        gfx::init_gl_settings(s);
	if(!s.use_arb_multitexturing || !s.use_ext_secondary_color || !s.use_nv_register_combiners ) {
		dout << "need multitexturing, secondary color and register combiners!\n";
		return 1;
	}

	throw_on_gl_error("system init");

	// initialize geometry

	vector<Color> colors;
	//push_back(colors) << Color(1,0,0) << Color(0,1,0) << Color(0,0,1) << Color(1,1,1);
	push_back(colors) << Color(.9,.9,.4) << Color(.8,.5,.9) << Color(.5,.9,.9) << Color(.7,.9,.7);

	float weights_snow[]  = { 1 ,0 ,0 ,0 };
	float weights_rock[]  = { 0 ,1 ,0 ,0 };
	float weights_dry[]   = { 0 ,0 ,1 ,0 };
	float weights_grass[] = { 0 ,0 ,0 ,1 };

	vector<Color> snow_col(4);
	vector<Color> rock_col(4);
	vector<Color> dry_col(4);
	vector<Color> grass_col(4);

	setup_colors1(&snow_col[0], &colors[0],  weights_snow);
	setup_colors2(&rock_col[0], &colors[0],  weights_rock);
	setup_colors1(&dry_col[0], &colors[0],   weights_dry);
	setup_colors2(&grass_col[0], &colors[0], weights_grass);

	vector<Point> points;
	push_back(points) << Point(.1,.1,0) << Point(.9,.1,0) << Point(.9,.9,0) << Point(.1,.9,0);

	vector<TexCoord> texcoords;
	push_back(texcoords) << TexCoord(0,0) << TexCoord(1,0) << TexCoord(1,1) << TexCoord(0,1);

	dout << "points: " << points << "\n";
	dout << "colors: " << colors << "\n";
	dout << "texcoords: " << texcoords << "\n";
	dout << "snow_col: " << snow_col << "\n";
	dout << "rock_col: " << rock_col << "\n";
	dout << "dry_col: " << dry_col << "\n";
	dout << "grass_col: " << grass_col << "\n";

	// intialize gl

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0,0,0,0);
	glColor3f(.4,.4,.4 );
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // pass on equal too, not just less (default)

	glBlendFunc(GL_ONE, GL_ONE); // add src and dst
	glEnable(GL_REGISTER_COMBINERS_NV); // Don’t forget this!

	glVertexPointer(3, GL_FLOAT, sizeof(Point), &points[0]);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);

	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glTexCoordPointer(2, GL_FLOAT, sizeof(TexCoord), &texcoords[0]);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glTexCoordPointer(2, GL_FLOAT, sizeof(TexCoord), &texcoords[0]);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	throw_on_gl_error("gl_init");

	// load register combiner specification

	res::res_stream is(res::Shader, "texture_blend_2_tmu.rc");
	int filesize = misc::get_size(is);
	char *rcfile = new char[filesize+1];
	is.read(rcfile, filesize);
	rcfile[filesize] = 0;
	test_and_apply_rc(rcfile);

	throw_on_gl_error("nvparse");

	dout << "Starting...\n";

	// main loop

	bool exit = false;
	while(!exit) {
		os::event::Event e;
		while (ep.get_event(e)) {
			bool dn = e.is_pressed();
			switch (e.id) {
			case os::event::id::Escape: 
				exit = true; 
				break;
			}
		}	 

		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// draw first pass
		
		glActiveTextureARB(GL_TEXTURE0_ARB);
		tr->use("terrain_snow");
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		tr->use("terrain_rock");

		glColorPointer(            4, GL_FLOAT, sizeof(Color), &snow_col[0]);		
		glSecondaryColorPointerEXT(3, GL_FLOAT, sizeof(Color), &rock_col[0]);

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glDrawArrays(GL_QUADS, 0, 4);

		// draw second pass 
		
		glActiveTextureARB(GL_TEXTURE0_ARB); 
		tr->use("terrain_dryland");

		glActiveTextureARB(GL_TEXTURE1_ARB);
		tr->use("terrain_grass");

		glColorPointer(            4, GL_FLOAT, sizeof(Color), &dry_col[0]);		
		glSecondaryColorPointerEXT(3, GL_FLOAT, sizeof(Color), &grass_col[0]);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glDrawArrays(GL_QUADS, 0, 4);		

		gx.update_screen();
	};	

	return 0;
}
}

int test_main() { return reaper::foo();}
