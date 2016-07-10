#include "os/compat.h"
#include "main/types_io.h"
#include "gfx/mesh/pmfactory.h"
#include "pmrealtimesys.h"
#include "main/types.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "gfx/mesh/pmrfast.h"
#include "gfx/mesh/pmrmorph.h"
#include "gfx/mesh/fbmskeleton.h"
#include "gfx/mesh/erosion.h"
#include <GL/glut.h>

using namespace reaper::gfx::mesh;
using namespace reaper::gfx;
using namespace reaper;
using std::cout;
using std::cin;

void create_mesh(PMFactory& pmf, int x_step, int y_step, float z_var, float param);
static void redraw();
static void idle();
static void handleSpecialKeys(int key, int x, int y);
void testmesh();
void handleKeys(unsigned char key, int x, int y);
static void setupLight();

void create_fbm_mesh(PMFactory& pmf, int max_x, int max_height);

bool overview = false;
float fang = 0.0;
float gcamh = -100.0;
float ucamh = 50.0;
int upper_bound = 1500;
int max_tex_pass = 3;
bool frend = true;
bool srend = true;
bool ccwrend = true;
Point gp;
Vector gv;

SRMesh* srmesh_ptr = NULL;
PMRMorph* rts_ptr = NULL;
char* file_name = "mesh.dat";

void create()
{
    try {
        cout << "Creating...\n";
        srmesh_ptr = new SRMesh;
        PMFactory pmf(srmesh_ptr);

        int d = 30;
	create_mesh(pmf, d, d, 30.0, 100.0 / (float) d);
        //create_fbm_mesh(pmf, 30, 10);
        pmf.report(false);
        pmf.set_lower_bound(1000);
        cout << "Building...\n";
        pmf.build();
    } catch (MeshException e) {
        cout << "\n\nMeshException: " << e.error << "\n";
        cin.get();
    }
}

void load()
{
    cout << "Loading...\n";
    srmesh_ptr = new SRMesh();
    srmesh_ptr->load("mesh.dat");
}

void save()
{
    cout << "Saving..\n";
    srmesh_ptr->save(file_name);
}

void simulate(int argc, char** argv)
{
    cout << "Glut..\n";
    glutInit(&argc, argv);
    glutInitWindowSize(1000, 1000);
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Progressive mesh test");

    glutSpecialFunc(handleSpecialKeys);
    glutKeyboardFunc(handleKeys);
    glutDisplayFunc(redraw);
    glutIdleFunc(idle);

    glShadeModel(GL_SMOOTH);
    ::glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glMatrixMode(GL_PROJECTION);
    gluPerspective( 90, 1.0, 10.0,  2000.0);
    glMatrixMode(GL_MODELVIEW); 

    cout << "Mesh..\n";

    testmesh();
    cout << "Go!\n";
    glutMainLoop();
}

void main(int argc, char** argv)
{
    if(argc < 2) {
        cout << "1. create-simul\n2. create-save\n3. load-simul\n";
        return;
    }

    switch(*argv[1]) {
        case '1':
            create();
            simulate(argc,argv);
            break;

        case '2':
            create();
            save();
            break;

        case '3':
            load();
            simulate(argc,argv);
            break;
    }
}

void testmesh()
{
    int tmp = 0;

    try {
        rts_ptr = new PMRMorph(*srmesh_ptr);
        rts_ptr->set_lower_bound(1000);
        rts_ptr->set_max_stack_depth(50);
        rts_ptr->setup();
        rts_ptr->report(false);

    } catch (SetTooSmall) {
        cout << "\nSetTooSmall\n";
    } catch (MalFormedInput) {
        cout << "\nMalFormedInput\n";
    } catch (ContractionOutOfDate) {
        cout << "\nContractionOutOfDate\n";
    } catch (MeshException e) {
        cout << "\n\nMeshException: " << e.error << "\n";
        cin.get();
    } catch (std::exception e) {
        cout << e.what() << "\n";
    }
}

static void idle()
{

    try {
        rts_ptr->frame_timer.start();

        static float ang = 0.0;
        static float cang = 3.14*270.0/180.0;
        static float ox = 50.0;
        static float oy = 50.0;
        static float oz = 6.0;
        static int cnt = 0;

        cnt++;
        float x = cos(ang) * 2;
        float y = sin(ang) * 2;
        float cx = cos(cang) * 30;
        float cy = sin(cang) * 20;
        float cz = 1.0;

        rts_ptr->set_cam( Point(ox + cx + x , oy + cy + y, oz + cz + ucamh), Point(x, y, 0), 120);
        rts_ptr->set_upper_bound(upper_bound);
        rts_ptr->refine();
        ang += 0.01;
        cang += 0.01;

        if(!overview){
            glMatrixMode(GL_MODELVIEW);   
            glLoadIdentity();
            glMatrixMode(GL_PROJECTION);   
            glLoadIdentity();

            setupLight();

            //sets up the projection matrix for a perspective transform
            gluPerspective(	 90,      //view angle
                1.0,     //aspect ratio
                1.0,     //near clip
                2000.0); //far clip

            gluLookAt(ox + cx + x , oy + cy + y, oz + cz + ucamh, ox + cx + x*2.0, oy + cy + y*2.0, oz + ucamh, 0, 0, 1);
        }

        redraw();
        rts_ptr->frame_timer.stop();

    } catch (MeshException e) {
        cout << "\n\nMeshException: " << e.error << "\n";
        (*rts_ptr).spec_report(true);
        redraw();
        cin.get();
        return;
    }

}


static void setupLight()
{

    GLfloat yellow[3]={1,1,1};
    GLfloat pos1[4]={0,0,100,0};


    GLfloat white[3]={1,1,1};
    GLfloat pos0[4]={0,2,5,1};

    GLfloat nocolor[4] = {0.0, 0.0, 0.0, 0.0};

    /* set colors to white for light 0, and set its position */
    glLightfv(GL_LIGHT0, GL_AMBIENT, nocolor);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, nocolor);
    glLightfv(GL_LIGHT0, GL_POSITION, pos1);
    ::glEnable(GL_LIGHT0); 
    //::glDisable(GL_LIGHT0);

    ::glEnable(GL_LIGHTING);  /* enable lighting */
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
    //::glEnable(GL_NORMALIZE);
}

static void redraw()                               //all drawing code goes here
{
    static float z_rot = 0.0;
    static float tilt = 0.0;

    tilt += 0.03;
    if( tilt>360) tilt = 0.0;
    z_rot-= 0.05;
    if( z_rot>360) z_rot = 0.0;

    ::glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //clears the colour and depth buffers
    ::glEnable(GL_DEPTH_TEST);

    glPushMatrix();         //saves the current matrix on the top of the matrix stack

    if(overview){
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);   //changes the current matrix to the projection matrix
        glLoadIdentity();
        setupLight();

        gluPerspective(	 90, 1.0, 10.0,  2000.0); //far clip

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef(0,0, gcamh);
        glRotatef(10, cos(tilt), sin(tilt), 1);
        glTranslatef(-50, -50, 0);
    }

    if(rts_ptr) {
        rts_ptr->set_max_tex_pass(max_tex_pass);
        glFrontFace(ccwrend ? GL_CCW :GL_CW);
        glShadeModel(srend ? GL_SMOOTH : GL_FLAT);        

        if(frend) {
            rts_ptr->fast_render();
        } else { 
            rts_ptr->render();
        }
    }

    glPopMatrix();         
    glutSwapBuffers();
    rts_ptr->render_cnt++;
}	

float get_z(float max)
{
    static float reso = 100000;
    return  ((float) ( rand() % ( (int) (max * reso) )) ) / (reso / max);
}

void create_fbm_mesh(PMFactory& pmf, int max_x, int max_height)
{
    int x, y;
    int max_y = max_x;

    Point p, n;
    gfx::mesh::TexCoord t[3];

    float param = 100.0 / max_x;

    FBMSkeleton skel(max_x, max_y, 1.0, 1.0, 30, 2.0 , 10.2);
    Erosion eros(skel.hf, 100.0, 100.0, max_height);
    eros.thermal( 32.0, 0.01, 10);


    FBMSkeleton gskel(max_x, max_y, 1.0 / 5.0, 9, 1.9, 11.2);
    FBMSkeleton rock_noise(max_x, max_y, 1.0, 10, 1.0, 1);
    FBMSkeleton snow_noise(max_x, max_y, 1.0, 10, 1.0, 1);

    pmf.get_mesh()->tex.new_tex("terrain_snow.png");
    pmf.get_mesh()->tex.new_tex("terrain_field.png");
    pmf.get_mesh()->tex.new_tex("terrain_rock.png");

    float grass_limit = max_height*0.45;
    float snow_limit = grass_limit + max_height*0.25;

    for(y = 0; y < max_y; y++){
        for(x = 0; x < max_x; x++){

            p = Point(x * param, y*param, skel.hf.get(x,y) * max_height);
            n = Point(0.0, 0.0, 0.0);

            float std_alpha_f = 0.5;

            float snow =  p.z > (snow_limit+gskel.hf.get(max_x- x -1, max_y-y-1))?0.6 + 0.4* snow_noise.hf.get(x, y):0.0;
            float grass = p.z < (grass_limit + (gskel.hf.get(x,y) * max_height)  )? rock_noise.hf.get(x,y) : 0.0;
            float rock = 1.0 - (snow + grass);
            float tex_scale = 1.0f / 1.0f;

            t[0].u = (float) tex_scale * p.x / 7.0;
            t[0].v = (float) tex_scale * p.y / 7.0;
            t[0].set_alpha(snow);

            t[1].u = (float) tex_scale * p.x / 5.0;
            t[1].v = (float) tex_scale * p.y / 5.0;
            t[1].set_alpha(grass);

            t[2].u = (float) tex_scale * p.x / 6.0;
            t[2].v = (float) tex_scale * p.y / 6.0;
            t[2].set_alpha(rock);

            pmf.new_vertex(p, n, t);
        }
    }

    for(y = 0; y < max_y - 1; y++){
        for(x = 0; x < max_x - 1; x++){
            int p0 = y * max_x + x;
            int p1 = p0 + 1;
            int p2 = (y + 1) * max_x + x;
            int p3 = p2 + 1;

            pmf.new_face(p0, p1, p2);
            pmf.new_face(p1, p3, p2);
        }
    }

}

void create_mesh(PMFactory& pmf, int x_step, int y_step, float z_var, float param)
{
    gfx::mesh::TexCoord t[1];
    float z = 0.0;
    int x,y;

    pmf.get_mesh()->tex.new_tex("test_turret.png");

    for(y = 0; y < y_step; y++) {
        for(x = 0; x < x_step; x++) {
            Point p(x * param, y * param, get_z(z_var));
            Point n(0.0, 0.0, 0.0);
            t[0].u = p.x / 40.0;
            t[0].v = p.y / 40.0;

            pmf.new_vertex(p, n, t);
        }
    }

    for(y = 0; y < y_step - 1; y++){
        for(x = 0; x < x_step - 1; x++){

            int p0 = y * x_step + x;
            int p1 = p0 + 1;
            int p2 = (y + 1) * x_step + x;
            int p3 = p2 + 1;

            pmf.new_face(p0, p1, p2);
            pmf.new_face(p1, p3, p2);
        }
    }
}


void handleKeys(unsigned char key, int x, int y)
{
    if(key > '0' && key < '9')max_tex_pass = key - '0';

    switch(key)

    {
    case 'd':
        rts_ptr->col_debug = !rts_ptr->col_debug;
    case 'a':
        ucamh += 1.0;
        break;
    case 'z':
        ucamh -= 1.0;
        break;
    case 'f':
        frend = !frend;
        break;
    case 's':
        srend = !srend;
        break;
    case 27:    /* ESC */
        exit(0); /* dirty exit */
        break;   /* unnecessary, I know */
    case '+':
        upper_bound += 20;
        break;
    case '-':
        upper_bound -= 20;
        break;
    case 'c':
        ccwrend = !ccwrend;
        break;
    case ' ': 
        overview = !overview;
        break;
    }
}


static void handleSpecialKeys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_LEFT:
        fang += 0.05;
        break;
    case GLUT_KEY_RIGHT:
        fang -= 0.05;
        break;
    case GLUT_KEY_UP:
        gcamh+=10.0;
        break;
    case GLUT_KEY_DOWN:
        gcamh-=10.0;
        break;
    }
}
