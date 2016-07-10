/****************************************************************************

  example5.cpp
  
    A GLUI program demonstrating subwindows, rotation controls,
    translation controls, and listboxes
    
      -----------------------------------------------------------------------
      
	7/10/98 Paul Rademacher (rademach@cs.unc.edu)
	
****************************************************************************/

#include "os/compat.h"

#include <memory>
#include <string>
#include <vector>

#include "editor.h"

int   last_x, last_y;
float rotationX = 0.0, rotationY = 0.0;

using namespace reaper::level_editor;

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
//	glutPassiveMotionFunc( myGlutPassiveMotion );
	
	build_gui();
	::glEnable(GL_SCISSOR_TEST);
	
	GLUI_Master.set_glutIdleFunc( myGlutIdle );
	
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
	if(rr.valid()) {
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
	if(rr.valid() && cam_move_mode) {
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
	else if(rr.valid() && obj_move_mode) {
		if(lock_altitude) {
			Point pos(obj_pos.x, obj_control, obj_pos.y);

			Vector xmove = Matrix(cam_rotate).row(0)*(prev_x - x);
			xmove.y = 0;
			pos += norm(xmove);
			
			Vector zmove = Matrix(cam_rotate).row(2)*(prev_y - y);
			zmove.y = 0;
			pos += norm(zmove);

			obj_pos.x = pos.x;
			obj_control = pos.y;
			obj_pos.y = pos.z;
		} else {
			Point pos(obj_pos.x, obj_control, obj_pos.y);
			pos += Matrix(cam_rotate).row(0)*(prev_x - x);
			pos += Matrix(cam_rotate).row(2)*(prev_y - y);
			obj_pos.x = pos.x;
			obj_control = pos.y;
			obj_pos.y = pos.z;
		}
		obj_moved(0);
	}

	prev_x = x;
	prev_y = y;

	glutPostRedisplay(); 
}

void myGlutPassiveMotion(int x, int y)
{
	reaper::glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	int tx, ty, tw, th;
	GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
	y = ty+th-y;
	glScissor(x-5,y-5,10,10);
	reaper::glClearColor(0,1,0,0);
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

	if(rr.valid()) {
		draw();
	} else {
		reaper::glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);		
	}

	glutSwapBuffers(); 
}
