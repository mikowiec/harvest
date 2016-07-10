#include "os/compat.h"

#include "os/debug.h"
#include "main/types_ops.h"
#include "main/types4_ops.h"
#include "gfx/interfaces.h"
#include "gfx/texture.h"
#include "gfx/camera.h"
#include "gfx/settings.h"
#include "misc/stlhelper.h"
#include <vector>

using namespace std;

//#define ENVMAP_DEBUG

namespace reaper {
namespace gfx {
namespace {
const int cubemap_size = 128;
debug::DebugOutput dout("gfx::shaders");
} // end anonymous namespace

using namespace texture;
using world::Frustum;

class EnvMapper::Impl
{
	typedef vector<Camera> CamCont;
	DynamicCubeMap cubemap;
	CamCont cams;
public:
	Impl();
	
	void generate(const Point &pos, const Frustum &frustum, Sky &sky, Terrain &tr);
	void setup();
	void restore();
};

EnvMapper::Impl::Impl() : cubemap(cubemap_size) 
{	
	cams.reserve(6);
	misc::push_back(cams) <<
		Camera(Point(), Vector( 1,0, 0), Vector(0,-1,0), 90, 90) <<
		Camera(Point(), Vector(-1,0, 0), Vector(0,-1,0), 90, 90) <<
		Camera(Point(), Vector(0, 1, 0), Vector(0,0, 1), 90, 90) <<
		Camera(Point(), Vector(0,-1, 0), Vector(0,0,-1), 90, 90) <<
		Camera(Point(), Vector(0, 0, 1), Vector(0,-1,0), 90, 90) <<
		Camera(Point(), Vector(0, 0,-1), Vector(0,-1,0), 90, 90);
}

void EnvMapper::Impl::generate(const Point &pos, const Frustum &frustum, Sky &sky, Terrain &tr) 
{
	int side = CubeMap::PosX;
	CamCont::iterator i, end;

	if(Settings::current.environment_mapping == 2) {
		i = cams.begin();
		end = cams.end();
	} else {
		static int update = 0;
		if(++update > 5) {
			update = 0;
		}
		i = cams.begin() + update;
		end = i + 1;
		side += update;
	}
	
	setup_viewport(cubemap);
	for(; i != end; ++i) {
		i->pos = pos;
		i->setup_view();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		sky.render(*i);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		tr.render(*i);
		cubemap.copy_to_texture(static_cast<CubeMap::Side>(side++));	
	}
	restore_viewport();
}

// render all sides with one clear
/* 
	vector<int> vps;
	misc::push_back(vps) << 
		0 << 128 << 
		256 << 128 <<
		128 << 256 <<
		128 << 0 << 
		384 << 128 <<
		128 << 128;
	int *vp = &vps[0];

	glPushAttrib(GL_VIEWPORT_BIT | GL_SCISSOR_BIT);
	glEnable(GL_SCISSOR_TEST);
	glViewport(0,0,512,384);
	glScissor(0,0,512,384);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int side = CubeMap::PosX;
	CamCont::iterator i, end;

	if(Settings::current.environment_mapping == 2) {
		i = cams.begin();
		end = cams.end();
	} else {
		static int update = 0;
		if(++update > 5) {
			update = 0;
		}
		i = cams.begin() + update;
		end = i + 1;
		side += update;
	}
	
	for(; i != end; ++i) {
		i->pos = pos;
		i->setup_view();

		glViewport(vp[0],vp[1],128,128);
		glScissor(vp[0],vp[1],128,128);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		sky.render(*i);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		tr.render(*i);

		cubemap.use();
		glCopyTexSubImage2D(side, 0, 0, 0, vp[0], vp[1], 128, 128);
		vp += 2;
	}

	glPopAttrib();
}
*/
void EnvMapper::Impl::setup() 
{
	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
	cubemap.enable_reflection_map();
	cubemap.use();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_ADD_SIGNED_ARB);
}
void EnvMapper::Impl::restore()
{
	cubemap.restore_mapping();
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
}

//////////////////////////////////////////////////////////////////////////
EnvMapper::EnvMapper() : i(new Impl) {}
EnvMapper::~EnvMapper() {}

void EnvMapper::generate(const Point &pos, const Frustum &frustum, Sky &sky, Terrain &tr) {
	i->generate(pos, frustum , sky, tr); 
}
void EnvMapper::setup() { 
	i->setup(); 
}
void EnvMapper::restore() {
	i->restore(); 
}

}
}

