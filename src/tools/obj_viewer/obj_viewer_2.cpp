
#include "os/compat.h"

#include <iostream>
#include <vector>

#include "os/gfx.h"
#include "os/event.h"

#include "main/types.h"
#include "main/types_ops.h"
#include "main/types_io.h"
#include "main/types_param.h"

#include "gfx/gfx.h"
#include "gfx/camera.h"
#include "gfx/exceptions.h"
#include "gfx/managers.h"
#include "gfx/interfaces.h"

#include "object/base.h"
#include "object/factory.h"

#include "os/gl.h"
#include "misc/parse.h"
#include "misc/test_main.h"

using namespace std;
using namespace reaper;
using namespace object;

using namespace hw::event;

class OVMap : public Mapping
{
public:
	Event operator()(const Event& e)
	{
		Event ev(e);
		ev.recv = System;
		return ev;
	}
	bool is_repeat(const Event& e)
	{
		switch (e.id) {
		case 'D': return false;
		case 'C': return false;
		default:  return e.is_key();
		}
	}

};

void foo(string name, string main_tex = "", string det_tex = "", int dr = 100)
{
	GLUquadricObj* glu_sp = gluNewQuadric();
	bool is_obj = main_tex.empty();

	Matrix mtx(true);
	float zoom = 1;

	bool reload = false;
	int win_size = 800;
	int lod = 0;

	MtxParams rot(2);
	bool is_rot = false;

	SillyPtr obj;
	gfx::Terrain* ter = 0;

	hw::gfx::Gfx gx(hw::gfx::VideoMode(win_size, win_size));

	hw::event::EventSystem es(gx);
	es.set_mapping(new OVMap());
	hw::event::EventProxy ep = hw::event::EventSystem::get_ref(0);

	gfx::RendererRef rr = gfx::Renderer::create(&gx);
	gfx::TextureRef tex = gfx::TextureMgr::get_ref();
	gfx::MeshRef mesh = gfx::MeshMgr::get_ref();
	gfx::VertexRef vert = gfx::VertexMgr::create();

        reaper::glEnable(GL_NORMALIZE);
        reaper::glEnable(GL_TEXTURE_2D);
        reaper::glEnable(GL_DEPTH_TEST);
        reaper::glEnable(GL_LIGHTING);
        reaper::glEnable(GL_LIGHT0);
	reaper::glEnable(GL_CULL_FACE);
	reaper::glEnableClientState(GL_VERTEX_ARRAY);
	reaper::glEnable(GL_POINT_SMOOTH);
	glAlphaFunc(GL_GREATER, 0.0);
//	glPolygonOffset(0, -5);

	reaper::glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0,0,win_size,win_size);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70,1,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	MkInfo mk(name, Matrix(true), Nature);

	if (is_obj) {
		obj = factory::inst().create(mk, TMap<SillyBase>());
		gluLookAt(0,0,-20,0,0,0,0,1,0);
	} else {
		ter = new gfx::Terrain(name, main_tex, det_tex, dr);
		gluLookAt(0,600,200,0,0,0,0,1,0);
		zoom = 0.10;
	}

	bool change = true;
	float det = 1.0; // terrain detail

	bool do_z_add = false, do_r_add = false;
	float x = 0, y = 0;
	bool quit = false;
	float xx = 0, yy = 0, zz = 0, ww = 0, vv = 0, uu = 0;
	while (!quit) {
		namespace id = hw::event::id;
		Event ev;
		while (ep.get_event(ev)) {
			change = true;
			switch (ev.id) {
				case id::Escape: quit = true; break;
				case id::Left:     mtx = mtx*Matrix( 1,Vector(0,1,0)) ; break;
				case id::Right:    mtx = mtx*Matrix(-1,Vector(0,1,0)) ; break;
				case id::Up:       mtx = mtx*Matrix( 1,Vector(1,0,0)) ; break;
				case id::Down:     mtx = mtx*Matrix(-1,Vector(1,0,0)) ; break;
				case id::Insert:   mtx = mtx*Matrix( 1,Vector(0,0,1)) ; break;
				case id::Delete:   mtx = mtx*Matrix(-1,Vector(0,0,1)) ; break;
				case id::PageDown: zoom *= 1.02; break;
				case id::PageUp:   zoom *= 0.98; break;
				case id::Home:     reaper::glEnable(GL_LIGHTING); break;
				case id::End:      reaper::glDisable(GL_LIGHTING); break;
				case id::F1:       reaper::glClearColor( 0, 0, 0,0); break;
				case id::F2:       reaper::glClearColor( 0, 0,.5,0); break;
				case id::F3:       reaper::glClearColor( 0,.5,.0,0); break;
				case id::F4:       reaper::glClearColor( 5, 0, 0,0); break;
				case id::F5:       reaper::glClearColor(.5, 0,.5,0); break;
				case id::F6:       reaper::glClearColor(.5,.5, 0,0); break;
				case id::F7:       reaper::glClearColor( 0,.5,.5,0); break;
				case id::F8:       reaper::glClearColor( 1,.5, 1,0); break;
				case id::F9:       reaper::glClearColor(.5, 1, 1,0); break;
				case id::F10:      reaper::glClearColor( 1, 1,.5,0); break;
				case id::F11:      reaper::glClearColor( 1, 1, 1,0); break;
				case id::F12:      tex->purge(); break;
				case 'A': 	   zoom *= 1.1; break;
				case 'Z': 	   zoom *= 0.9; break;
				case 'S': 	   det = min(1.0, det + 0.01); break;
				case 'X': 	   det = max(0.0, det - 0.01); break;
				case 'D': 	   lod++; break;
				case 'C': 	   lod--; break;
				case id::Axis0:
					if (do_r_add) {
						mtx = mk_rot_mtx( 20*(ev.val - x), Vector(0, 1, 0)) * mtx;
					}
					x = ev.val;
					break;
				case id::Axis1:
					if (do_r_add) {
						mtx = mk_rot_mtx(-20*(ev.val - y), Vector(1, 0, 0)) * mtx;
					}
					if (do_z_add) {
						zoom *= 1 + (ev.val - y);
					}
					y = ev.val;
					break;
				case id::Btn0: do_r_add = (ev.val > 0.1); break;
				case id::Btn1: do_z_add = (ev.val > 0.1); break;
				default: break;
			}
		}

		if(change) {
			change = false;
			if (zoom<0.05)
				zoom = 0.05;

			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glPushMatrix();
			glMultMatrixf(Matrix4(mtx).get());
			glScalef(zoom,zoom,zoom);

			if (is_obj) {
				obj->render();
				if (!is_rot) {
					try {
						mesh->render(name, lod);
					} catch (gfx::gfx_fatal_error) {
						is_rot = true;
						change = true;
					}
				} else {
					mesh->render(name, lod, rot);
				}

			} else {
				gfx::Camera cam(Point(0, 1, 0), Point(0,0,0), Vector(0,0,1), 90, 90);
				ter->render(cam);
			}
			glPopMatrix();
		} else {
			hw::time::msleep(10);
		}
		gx.update_screen();
	}

	obj.invalidate();
	rr->shutdown();
}


int test_main()
{
	switch (argv.size()) {
	case 1:
		foo(argv[0]);
		break;
	case 3:
		foo(argv[0], argv[1], argv[2]);
		break;
	case 4:
		foo(argv[0], argv[1], argv[2], misc::stoi(argv[3]));
		break;
	default:
                cout << "Usage: obj_viewer <object file>\n";
//                cout << "       obj_viewer <terrain file> <main texture> <detail texture>\n";
//                cout << "       obj_viewer <terrain file> <main texture> <detail texture> <detail repeats>\n";
                cout << "Specify object and texture files without extensions!\n";
                cout << "Texture name will be loaded from object file\n";
	}

        return 0;
}
