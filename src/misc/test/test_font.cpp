#include "os/compat.h"

#include <stdio.h>

#include "os/gl.h"

#include "os/gfx.h"
#include "os/time.h"
#include "os/debug.h"
#include "main/exceptions.h"

#include "misc/font.h"

using namespace reaper;


debug::DebugOutput derr("test_font",0);


int foo()
{
	os::gfx::Gfx gx;

	glViewport(0, 0, gx.current_mode().width, 
			 gx.current_mode().height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90,1,1,20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 3, 0,0,0, 0,1,0);

	glColor3f(1.0, 1.0, 1.0);
	glClearColor(0, 0, 0, 0);
	for (int i = 0; i < 100; i++) {
		derr << "Row: " << i << '\n';
		glClear(GL_COLOR_BUFFER_BIT);
		gfx::font::glPutStr(0, 0, "Test 42!", gfx::font::Medium, 0.1, 0.15);
		gfx::font::glPutStr(1, 1, "Test 43!", gfx::font::Medium, 0.1, 0.15);
		gfx::font::glPutStr(0, 1, "Test 44!", gfx::font::Medium, 0.1, 0.15);
		gfx::font::glPutStr(1, 0, "Test 45!", gfx::font::Medium, 0.2, 0.15);
		gfx::font::glPutStr(-1, -1, "ABXZ\"0123()!?/\\[]^%", gfx::font::Medium, 0.2, 0.2);
		gx.update_screen();
		os::time::msleep(100);
	}
	return 0;
}


int main()
{      
        try {
		foo();
                return 0;
        }
        catch(const fatal_error_base &e) {
                derr << e.what() << std::endl;
                return 1;
        } 
        catch(const error_base &e) {
                derr << e.what() << std::endl;
                return 1;
        }
        catch(std::exception &e) {
                derr << "Std exception: " << e.what() << std::endl;
                return 1;
        }
}
 

