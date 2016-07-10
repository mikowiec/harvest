
#include <stdio.h>
#include <iostream>

#include "os/compat.h"
#include "gfx/gfx.h"
#include "gfx/main.h"
#include "os/debug.h"
#include "os/event.h"
#include "os/font.h"
#include "os/gfx.h"
#include "os/gl.h"
#include "os/time.h"
#include "world/world.h"

using namespace std;

using namespace reaper;
using namespace reaper::gfx;
using namespace reaper::world;
namespace htime  = reaper::os::time;
namespace hgfx   = reaper::os::gfx;
namespace hevent = reaper::os::event;
using reaper::os::font::glPutStr;

htime::timespan perf_cnt_freq = 1000000;  // number of tics per second
htime::timespan curtime;         // time in # of tics

namespace { reaper::os::debug::DebugOutput derr("os::win32", 1); }

void drawFrameCounter(float Diff);


int main()
{
	try {
	        //QueryPerformanceCounter(&time);
		curtime = htime::get_time();

		// Initialise graphics 
	        hgfx::Gfx gx;

	        // Print OpenGL implementation information
		cout << OpenGLinfo();

		LocalPlayer lp(0,Horny,Ammo_Box,Point(0,0,0),Quaternion(0,0,1,0),10,true,10,Assignment(),1);
		world::World world("test_level",lp);

	        // Get a handle to the event-system
		hevent::Events ev(gx);   
		hevent::Event e;

		// Initialize graphics settings
		Settings s;
		s.width = gx->current_mode().width;
		s.height = gx->current_mode().height;


	        // Get interface to a renderer
		Camera c;
		Renderer rnd(s);

		bool cont = true;
		while (cont) {
		        // Wait for event and exit when escape is pressed
			ev.poll_inputs();
			if (ev.get_event(e)) {
				if (e.is_key() && e.key == hevent::Escape)
					cont = false;
			}
			htime::timespan prevtime = curtime;
		        curtime = htime::get_time();
	                double timediff = (curtime - prevtime)/double(perf_cnt_freq);  

			rnd.draw(c);
//			derr << timediff << '\n';
			drawFrameCounter(timediff);
			gx->update_screen();
		}

	} catch (fatal_error_base& e) {
		derr << "Fatal exception: " << e.what() << endl;
	} catch (error_base& e) {
		derr << "Exception: " << e.what() << endl;
	} catch (exception& e) {
		derr << "Std Exception: " << e.what() << endl;
	}
        return 0;
}
        
void drawFrameCounter(float Diff)
{
        char buf[20];

        glPushAttrib(GL_COLOR_BUFFER_BIT);      
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);

        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);

        glPushMatrix();

                glLoadIdentity();
                gluOrtho2D(0,1,0,1);

                glColor4f(0,0,0,0.5f);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glRectf(0.0f, 0.0f, 1.0f, 0.07f);
                glDisable(GL_BLEND);

                // Draw analog meter

                glColor3f(1,0,0);
                glRectf(0, 0.01f, 1/(Diff*100.0), 0.02f);

                glBegin(GL_LINES);
                        glColor3f(1,1,1);

                        for(unsigned i=0;i<10;i++){
                                glVertex2f(0.1*i,0);
                                glVertex2f(0.1*i,0.03f);
                        }
                glEnd();

                // Draw digital meter

//                glRasterPos2f(0.01f,0.03f);      
                sprintf(buf,"FPS: %.1f",1.0/Diff);
		glPutStr(0.01f, 0.03f, buf);
//                unsigned int len = strlen(buf);

//                for(int i = 0; i < len; i++)
//                        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,buf[i]);

        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();    

        glPopAttrib();
        glPopAttrib();
}

