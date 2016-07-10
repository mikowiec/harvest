#include "os/compat.h"

#include <memory>
#include <string>
#include <vector>

#include "editor.h"
#include "os/gl_state.h"
#include "main/types4_ops.h"

using namespace reaper::level_editor;

namespace reaper {
namespace level_editor {

void myGlutKeyboard(unsigned char Key, int x, int y);
void myGlutMenu( int value );
void myGlutIdle();
void myGlutMouse(int button, int button_state, int x, int y );
void myGlutMotion(int x, int y );
void myGlutReshape( int x, int y );
void myGlutDisplay();
void myGlutPassiveMotion(int x, int y);

bool obj_move_mode = false;
bool cam_move_mode = false;
int prev_x;
int prev_y;
int last_x;
int last_y;
float rotationX = 0.0;
float rotationY = 0.0;

}
}

/**************************************** main() ********************/

void main(int argc, char* argv[])
{
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( 50, 50 );
	glutInitWindowSize( 800, 600 );
	
	main_window = glutCreateWindow( "Reaper Level Editor" );
	glutDisplayFunc( myGlutDisplay );
	GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
	GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
	GLUI_Master.set_glutSpecialFunc( NULL );
	GLUI_Master.set_glutMouseFunc( myGlutMouse );
	glutMotionFunc( myGlutMotion );
	//glutPassiveMotionFunc( myGlutPassiveMotion );
	
	reaper::reinit_glstates();

	build_gui();
	
	GLUI_Master.set_glutIdleFunc( myGlutIdle );

	world::WorldRef wr(0);
	
	/**** Regular GLUT main loop ****/		
	try {
		glutMainLoop();
	} catch (fatal_error_base &e) {
		cout << "Reaper exception: " << e.what() << "\n";
	} catch (error_base &e) {
		cout << "Reaper non-fatal exception: " << e.what() << '\n';
	} catch (exception &e) {
		cout << "STL exception: " << e.what() << '\n';
	}
}

/**************************************** myGlutKeyboard() **********/

namespace reaper {
namespace level_editor {

void myGlutKeyboard(unsigned char Key, int x, int y)
{
	switch(Key)
	{
	case 27: 
	case 'q':
		myExit();
		break;
	};
	
	glutPostRedisplay();
}


/***************************************** myGlutMenu() ***********/

void myGlutMenu( int value )
{
	myGlutKeyboard( value, 0, 0 );
}


/***************************************** myGlutIdle() ***********/

void myGlutIdle( void )
{
/* According to the GLUT specification, the current window is 
undefined during an idle callback.  So we need to explicitly change
	it if necessary */
	if ( glutGetWindow() != main_window ) 
		glutSetWindow(main_window);  
	
	GLUI_Master.sync_live_all();
	
	glutPostRedisplay();
}

/***************************************** myGlutMouse() **********/

void myGlutMouse(int button, int button_state, int x, int y )
{
	if(rr != 0) {
		if(button == GLUT_LEFT_BUTTON) {
			static int px = x;
			static int py = y;

			if(button_state == GLUT_DOWN) {
				px = x;
				py = y;

				obj_move_mode = true;
			} else {
				obj_move_mode = false;

				if(px == x && py == y) {
					select_object(x,y);
				}
			} 
			
		} else if(button == GLUT_RIGHT_BUTTON) {
			cam_move_mode = button_state == GLUT_DOWN;
		}

		prev_x = x;
		prev_y = y;
	}
}


/***************************************** myGlutMotion() **********/

void myGlutMotion(int x, int y )
{
	if(rr != 0 && cam_move_mode) {
		cam_rotate = Matrix4(Matrix(cam_rotate) * Matrix((prev_x-x)/4.0 ,Vector(0,1,0)));
		if(selected_object.valid()) {
			if(lock_altitude) {
				cam_zoom -= (prev_y - y) / 3;
				cam_move(0);
			} else {
				obj_control += (prev_y - y) / 2;
				obj_moved(0);
			}
		} else {
			cam_pos.y += prev_y - y;
			cam_move(0);
		}
	} 
	else if(rr != 0 && obj_move_mode) {
		Matrix rot(cam_rotate);
		Vector xmove(rot.col(0) * (prev_x - x));
		Vector zmove(rot.col(2) * (prev_y - y));
		Point  pos(obj_pos.x, obj_control, obj_pos.y);

		if(lock_altitude) {
			xmove.y = 0;
			zmove.y = 0;
		}
		pos += xmove;
		pos += zmove;

		obj_pos.x = pos.x;
		obj_control = pos.y;
		obj_pos.y = pos.z;

		obj_moved(0);
	}

	prev_x = x;
	prev_y = y;

	glutPostRedisplay(); 
}

void myGlutPassiveMotion(int x, int y)
{
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	y = ty+th-y;
	glScissor(x-5,y-5,10,10);
	glClearColor(0,1,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
}

/**************************************** myGlutReshape() *************/

void myGlutReshape( int x, int y )
{
	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	glViewport( tx, ty, tw, th );
	
	xy_aspect = (float)tw / (float)th;
	
	glutPostRedisplay();
}

/***************************************** myGlutDisplay() *****************/

void myGlutDisplay( void )
{
	using namespace world;

	if(rr != 0) {
		draw();
	} else {
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);		
	}

	glutSwapBuffers(); 
}

}
}
