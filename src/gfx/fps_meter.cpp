/*
 * $Author: pstrand $
 * $Date: 2003/06/04 16:29:57 $
 * $Log: fps_meter.cpp,v $
 * Revision 1.36  2003/06/04 16:29:57  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.35  2003/01/06 12:42:01  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.34  2002/12/29 17:04:32  pstrand
 * hw/ moved to os/
 *
 * Revision 1.33  2002/04/08 20:07:08  pstrand
 * valarray -> vector<float>  (just for compat with egcs, with is the only place valarray is used...)
 *
 * Revision 1.32  2001/12/18 14:14:16  macke
 * cancel last commit
 *
 * Revision 1.30  2001/12/15 20:02:05  peter
 * fpsmeter fix
 *
 * Revision 1.29  2001/12/06 15:10:00  peter
 * d'oh
 *
 * Revision 1.28  2001/12/05 21:58:38  peter
 * moved fps-meter code, debug_window is no more
 *
 * Revision 1.27  2001/11/26 02:20:06  peter
 * ljudfixar (smartptr), andra mindre fixar..
 *
 * Revision 1.26  2001/07/11 10:54:50  peter
 * time.h uppdateringar...
 *
 * Revision 1.25  2001/06/07 05:14:16  macke
 * Reaper v0.9
 *
 * Revision 1.24  2001/05/10 11:40:11  macke
 * häpp
 *
 */

#include "os/compat.h"

#include <string>
#include <vector>

#include "gfx/exceptions.h"
#include "gfx/fps_meter.h"
#include "gfx/misc.h"
#include "misc/font.h"
#include "os/gl.h"
#include "os/extra.h"
#include <stdio.h>

namespace reaper {
namespace gfx {

using namespace misc;

FPSMeterVP::FPSMeterVP()
{
	reinit();
}

void FPSMeterVP::reinit()
{
	glGetIntegerv(GL_VIEWPORT, vp);
}

void FPSMeterVP::render()
{
	glViewport(vp[0], vp[1], vp[2], 60);
	fps.render(false);
	glViewport(vp[0], vp[1]+60, vp[2], vp[3] - 60);
}

float avg(const std::vector<float>& v)
{
	float sum = 0;
	int n = v.size();
	for (int i = 0; i < n; ++i)
		sum += v[i];
	return sum / n;
}

void FPSMeter::render(bool blend, bool vertical)
{
        double dt = (os::time::get_time() - prev_time).approx() * 1e-6;
        prev_time = os::time::get_time();

        total_time += dt;

	if(frames==120)
		total_time = avg(v) * 120;

	frames++;
	v[array_index++] = dt;
	float diff = avg(v);
	if(array_index == v.size()) 
		array_index = 0;

        char buf[200];

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	if(blend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glDisable(GL_BLEND);
	}

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        glLoadIdentity();
        gluOrtho2D(0,1,0,1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if(vertical) {
		glTranslatef(.5,.5,0);
		glRotatef(90,0,0,-1);
		glTranslatef(-.5,-.5,0);
	}

        glColor4f(0,0,0,.3);
        glRectf(0,0,1,1);

        // Draw analog meter
        glColor4f(0,0,1,.8);
	meter(0,0.1, 1,.1, 100,1/dt);
        glColor4f(1,0,0,.8);
	meter(0,0.2, 1,.1, 100,1/diff);
        glColor4f(0,1,0,.8);
	meter(0,0.3, 1,.1, 100,frames/total_time);

        glBegin(GL_LINES);
                glColor4f(1,1,1,.8);

                for(unsigned i=0;i<10;i++){
                        glVertex2f(0.1*i,0);
                        glVertex2f(0.1*i,0.5f);
                }
        glEnd();

        // Draw digital meter
	snprintf(buf,sizeof(buf),"CURRENT FPS: %5.1f AVERAGE FPS: %5.1f TOTAL AVERAGE FPS: %5.1f",1.0/dt,1.0/diff,frames/total_time);
	reaper::gfx::font::glPutStr(0.1f,0.6f,buf, reaper::gfx::font::Medium, 0.01, 0.15);

	glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();    

	if(blend) {
		glDisable(GL_BLEND);
	}
	throw_on_gl_error("draw_fps_meter()");
}

}
}

