#include "os/compat.h"

#include "os/gfx.h"
#include "os/event.h"
#include "os/time.h"

#include "gfx/misc.h"
#include "gfx/vertex_array.h"

#include "misc/font.h"

#include <vector>
#include <iostream>
#include <cmath>

using namespace reaper;
using namespace reaper::gfx;
using namespace reaper::os::event;
using namespace std;
using reaper::gfx::misc::DisplayList;
using reaper::gfx::font::glPutStr;
using reaper::gfx::misc::meter;

os::time::Profiler gfx_time("gfx"), wait_time("wait"), frame_time("frame");

void report()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,1,0,80);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1,1,1);
	glPutStr(0, 4, gfx_time.report_last(), 0.008, 1.9);
	glPutStr(0, 8, wait_time.report_last(), 0.008, 1.9);

	glColor3f(0,0,1);
	meter(.5,4, .5, 1, 1, gfx_time.get_last()/(float)frame_time.get_last());
	meter(.5,8, .5, 1, 1, wait_time.get_last()/(float)frame_time.get_last());

	glColor3f(0,1,0);
	meter(.5,5, .5, 1, 1, gfx_time.get_avg()/frame_time.get_avg());
	meter(.5,9, .5, 1, 1, wait_time.get_avg()/frame_time.get_avg());

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

int main()
{
	try {
		os::gfx::Gfx gx;
		EventSystem es(gx);
		EventProxy ep = EventSystem::get_ref(0);

		bool true_val = true;
		
		os::gfx::init_nv_fence();
		os::gfx::init_nv_vertex_array_range();
		os::gfx::init_ext_compiled_vertex_array();

		vertex_array::Base::use_nv_fence = &true_val;
		vertex_array::Base::use_nv_vertex_array_range = &true_val;
		vertex_array::Base::use_ext_compiled_vertex_array = &true_val;
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(5);
		const int size = 200000;

		VertexArray<Point> vertex_var(size);
		VertexArray<Point> vertex_var2(size);
		std::vector<Point> vertex_mem(size);

		for(int i=0;i<size;i++) {
			float x = (i / size) / size;
			float y = rand() / float(RAND_MAX);
			vertex_var[i] = vertex_mem[i] = Point(x,y,0);
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-1.1,1.1,-1.1,1.1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		int count = 0;
		DisplayList list;
		frame_time.start();
		while(!ep.key(id::Escape)) {
			wait_time.start();
			gx->update_screen();
			wait_time.stop();

			frame_time.stop();
			frame_time.start();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			gfx_time.start();
			glRotatef(0.3,0,0,1);
			
			if(count++ == 200){
				cout << "build list\n";
				list.begin();
				glColor3f(1,0,0);
				glVertexPointer(3, GL_FLOAT, sizeof(Point), &vertex_var[0]);
				vertex_var.glDrawArrays(GL_POINTS,0,size);
				list.end();
			}

			if(count>300 && count % 100 >= 0 && count % 100 < 20) {
				list.call();
			} else {
				glColor3f(1,1,1);
				glVertexPointer(3, GL_FLOAT, sizeof(Point), &vertex_var[0]);
				vertex_var.glDrawArrays(GL_POINTS,0,size);
			}

			gfx_time.stop();
			report();
		}

		cout << "finished\n";
		return 0;
	}
	catch (fatal_error_base &e) {
		cout << e.what();
	}
	catch (error_base &e) {
		cout << e.what();
	}
}
