#include "os/compat.h"

#include <sstream>
#include <list>
#include <map>

#include "gfx/gfx.h"
#include "gfx/misc.h"
#include "gfx/managers.h"
#include "gfx/exceptions.h"
#include "gfx/shaders.h"
#include "gfx/settings.h"
#include "os/debug.h"
#include "os/gfx.h"
#include "os/gl.h"
#include "msg/event.h"
#include "os/time.h"
#include "res/res.h"
#include "res/config.h"

namespace reaper {
debug::DebugOutput dout("gfx::Shader");

void vertex(float x, float y)
{
	glMultiTexCoord2fARB((GLenum)GL_TEXTURE0_ARB,x*2,y*2);
	glMultiTexCoord2fARB((GLenum)GL_TEXTURE1_ARB,x*3,y*3);
	glVertex2f(x,y);
}

void trans()
{
	float t = os::time::get_time().approx().to_s() * 0.1;
	glTranslatef(t,t,0);
}

int foo()
{
	using gfx::shaders::TexShader;

	os::gfx::Gfx gx;

	os::event::EventSystem es(gx);
	os::event::EventProxy ep = os::event::EventSystem::get_ref(os::event::System);

        gfx::TextureRef::create();
	gfx::TextureRef tmr = gfx::TextureMgr::get_ref();

        gfx::Settings s;
        gfx::init_gl_settings(s);

	TexShader *shader = new TexShader("sky");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);

	gfx::misc::DisplayList list;
	list.begin();
	glBegin(GL_QUADS);
	vertex( 0,0);	
	vertex(.5,0);	
	vertex(.5,1);	
	vertex( 0,1);
	glEnd();
	list.end();

	glClearColor(Color(0,1,0));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	bool exit = false;
	while(!exit) {
		os::event::Event e;
		while (ep.get_event(e)) {
			bool dn = e.is_pressed();
			switch (e.id) {
			case os::event::id::Escape: exit = true; 
			                                   break;
			case os::event::id::F1: delete shader;
			                        shader = new TexShader("sky");
			                        break;
			}
		}	

		glColor3f(0,0,1);
		glClear(GL_COLOR_BUFFER_BIT);
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		trans();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		shader->use();
		list.call();
		shader->restore();

		glTranslatef(.5,0,0);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		tmr->use("");
		list.call();

		tmr->use("sky");
		glColor3f(1,1,1);

		glEnable(GL_BLEND);
		list.call();

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		trans();
		glScalef(1.5,1.5,1);

		list.call();
		glDisable(GL_BLEND);

		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
				
		gx.update_screen();
	};
	

	return 0;
}
}

int main()
{
	using namespace reaper;

	try {
		return foo();
	}
	catch(fatal_error_base &e) {
		dout << e.what() << "\n";
	}
	catch(error_base &e) {
		dout << e.what() << "\n";
	}
	catch(...) {
		throw;
	}
}
