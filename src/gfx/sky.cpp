/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:34 $
 * $Log: sky.cpp,v $
 * Revision 1.37  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.36  2002/05/22 10:04:46  fizzgig
 * no message
 *
 * Revision 1.35  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.34  2002/03/06 21:08:15  fizzgig
 * Texture-preload now functioning in TexShaders as well..
 *
 * Revision 1.33  2002/02/10 13:58:40  macke
 * cleanup
 *
 * Revision 1.32  2002/01/26 23:27:13  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.31  2002/01/10 23:09:08  macke
 * massa bök
 *
 * Revision 1.30  2002/01/07 14:00:26  peter
 * resurs och ljudmeck
 *
 * Revision 1.29  2001/12/17 16:28:33  macke
 * div bök
 *
 * Revision 1.28  2001/11/26 02:20:07  peter
 * ljudfixar (smartptr), andra mindre fixar..
 *
 * Revision 1.27  2001/10/04 00:14:12  macke
 * Screenshots åt rätt håll, VC-fix i refptr samt lite städ
 *
 * Revision 1.26  2001/09/28 01:36:38  macke
 * hmm?
 *
 * Revision 1.25  2001/07/30 23:43:19  macke
 * Häpp, då var det kört.
 *
 * Revision 1.24  2001/07/06 01:47:14  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.23  2001/06/09 01:58:53  macke
 * Grafikmotor reorg
 *
 * Revision 1.22  2001/05/10 11:40:17  macke
 * häpp
 *
 */

#include "os/compat.h"

#include <string>

#include "main/types.h"
#include "gfx/exceptions.h"
#include "gfx/camera.h"
#include "gfx/settings.h"
#include "gfx/managers.h"
#include "gfx/misc.h"
#include "gfx/interfaces.h"
#include "gfx/shaders.h"

#include "os/gl.h"

namespace reaper {
namespace gfx {

class Sky::Impl
{
        Color color;
        float altitude;
        float repeats;
	misc::DisplayList list;
	std::string texture;
	float sim_time;
	shaders::TexShader shader;
	TextureRef tr;
public:
	Impl(const std::string &tex_file, const Color &col, float alt, float reps);
        void render(const Camera &cam);
	void simulate(float dt);
};

Sky::Impl::Impl(const std::string &tex_file, const Color &col, float alt, float reps) :
	color(col),
	altitude(alt),
	repeats(reps),
	texture(tex_file),
	sim_time(0),
	shader("sky"),
	tr(TextureMgr::get_ref())
{
	if(Settings::current.use_arb_multitexturing && Settings::current.use_arb_texture_env_combine) {
		list.begin();
		for(int i=-5;i<5;i++) {
			glBegin(GL_TRIANGLE_STRIP);
			for(int j=-5;j<6;j++) {
				glMultiTexCoord2fARB((GLenum)GL_TEXTURE0_ARB,i,j);
				glMultiTexCoord2fARB((GLenum)GL_TEXTURE1_ARB,i,j);
				glVertex3f(i*1000,altitude,j*1000);

				glMultiTexCoord2fARB((GLenum)GL_TEXTURE0_ARB,(i+1),j);
				glMultiTexCoord2fARB((GLenum)GL_TEXTURE1_ARB,(i+1),j);
				glVertex3f((i+1)*1000,altitude,j*1000);
			}
			glEnd();
		}
		list.end();
	} else {
		list.begin();
		// FIXME: maximum view distance fixed to 5000m here 
		for(int i=-5;i<5;i++) {
			glBegin(GL_TRIANGLE_STRIP);
			for(int j=-5;j<6;j++) {
				glTexCoord2f(i,j);
				glVertex3f(i*1000,altitude,j*1000);
				glTexCoord2f(i+1,j);
				glVertex3f((i+1)*1000,altitude,j*1000);
			}
			glEnd();
		}
		list.end();
	}

	if(Settings::current.use_arb_multitexturing && Settings::current.use_arb_texture_env_combine) {		
		shader.pre_load();
	} else {
		tr->load(tex_file);
	}
}

void Sky::Impl::render(const Camera &cam)
{
	if(!Settings::current.draw_sky)
		return;

	glPushMatrix();
	glTranslatef(cam.pos.x,0,cam.pos.z);		

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(repeats/10.0,repeats/10.0,1);		
	glTranslatef(cam.pos.x/2000 + sim_time/50,cam.pos.z/2000 + sim_time / 80,0);

	if(Settings::current.use_arb_multitexturing && Settings::current.use_arb_texture_env_combine) {		
		shader.use();
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glLoadIdentity();
		glScalef(repeats/4.0,repeats/4.0,1);		
		glTranslatef(cam.pos.x/1000 + sim_time/20,cam.pos.z/1000 + sim_time/20,0);
	} else {
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		tr->use(texture);	
	}

	glColor3(color);
	list.call();

	if(Settings::current.use_arb_multitexturing && Settings::current.use_arb_texture_env_combine) {		
		glMatrixMode(GL_TEXTURE);
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glLoadIdentity();
		glActiveTextureARB(GL_TEXTURE0_ARB);
		shader.restore();
	} else {
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
        throw_on_gl_error("Sky::render()");
}

void Sky::Impl::simulate(float dt) 
{
	sim_time += dt;
}

Sky::Sky(const std::string &tex_file, const Color &col, float alt, float reps) :
	i(new Impl(tex_file,col,alt,reps)) {
}
Sky::~Sky() {
}
void Sky::render(const Camera &cam) { i->render(cam); }
void Sky::simulate(float dt)        { i->simulate(dt); }

}
}
