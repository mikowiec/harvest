#include "os/compat.h"
#include "editor.h"
#include "gfx/misc.h"
#include "main/types_io.h"
#include <deque>
#include "os/time.h"
#include "misc/font.h"
#include <iomanip>
#include "gfx/managers.h"
#include "gfx/instances.h"

namespace
{
float fetch_radius(const std::string &id)
{
	return std::max(1.0f, gfx::MeshMgr::get_ref()->get_radius(id));
}
}

namespace reaper
{
namespace level_editor
{

Object::Object(const std::string& mesh, const Matrix& mtx, CompanyID c) :
	ObjImpl<object::DynamicBase>(mtx, fetch_radius(mesh), c, mesh),
	rd(mesh, mtx, fetch_radius(mesh)),
	mesh_id(mesh),
	company(write(c))
{
}

void Object::set_pos(const Matrix &m, const Point &p)
{
	data.mtx() = m;
	data.mtx().pos() = p;
}

void Object::reset_rotation()
{
	set_mtx(Matrix(get_pos()));
}

void myExit()
{
	clear_graph(0);
	
	rr.destroy();
	wr.release();
	exit(0);
}

gfx::Camera set_cam()
{
	if(selected_object.valid()) {
		return gfx::Camera(camera_pos, selected_object->get_pos(), Matrix(cam_rotate).row(1), 90, 90/xy_aspect);
	} else {
		return gfx::Camera(camera_pos, Matrix(cam_rotate).row(2), Matrix(cam_rotate).row(1), 90, 90/xy_aspect);
	}
}

void select_object(int x, int y)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	y = viewport[1]+viewport[3]-y;

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1,1,1);

	gfx::Camera c(set_cam());

	// Set projection matrix
        glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix(x,y,10,10,viewport);
	gluPerspective(c.vert_fov,c.horiz_fov/c.vert_fov,1,5000*1.2);

	// Set viewing matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Point lookat = c.pos + c.front;
	gluLookAt(c.pos.x, c.pos.y, c.pos.z,
		  lookat.x, lookat.y, lookat.z,
		  c.up.x, c.up.y, c.up.z);		

	// Set current viewing frustum
	Vector right = norm(cross(c.front,c.up));
	Vector up = norm(cross(c.front,right));
	Frustum frustum(c.pos, c.front * 5000, up, c.horiz_fov, c.vert_fov);

	std::map<GLuint, DynamicPtr> sel_objects;
	std::vector<GLuint> sel_buffer(20000);	
	
	glSelectBuffer(sel_buffer.size(), &sel_buffer[0]);

	glPushAttrib(GL_SCISSOR_BIT);
	glEnable(GL_SCISSOR_TEST);
	glScissor(x-5,y-5,10,10);

	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(-1);

	GLuint name = 666;
	for(dyn_iterator i = wr->find_dyn(frustum); i != wr->end_dyn(); ++i) {
		if(i->is_dead())
			continue;

		glLoadName(name);
		sel_objects[name] = (*i).get();

		glPushMatrix();
		glTranslate(i->get_pos());
		gfx::misc::sphere(i->get_radius(),20,20);
		glPopMatrix();

		name++;
	}
	int hits = glRenderMode(GL_RENDER);
	glPopAttrib();

	// find closest object
	int closest_obj = -1;
	unsigned int closest_obj_z = -1;
	std::vector<GLuint>::iterator ptr = sel_buffer.begin();
	for(int i=0;i<hits;i++) {
		int n_names = *ptr++;
		if(n_names != 1) { 
			cout << n_names << " detected -- > SELECTION ERROR!\n"; exit(0);
		}

		unsigned int min_z = *ptr++;
		unsigned int max_z = *ptr++;
		int obj = *ptr++;

		if(min_z < closest_obj_z) {
			closest_obj_z = min_z;
			closest_obj = obj;
		}
	}
	
	if(selected_object.valid())
		deselect_object();

	if(closest_obj != -1) {
		selected_object.dynamic_assign(sel_objects[closest_obj]);
		if(selected_object.valid()) {
			sel_object();
		}
	}
}

void deselect_object()
{
	selected_object = 0;
	
	object_mod_panel->disable();
	remove_object->disable();
	update_object->disable();

	cam_move(OBJ_DESELECTED);
	glutPostRedisplay();
}

void sel_object()
{
	object_mod_panel->enable();
	remove_object->enable();
	update_object->enable();

	Point pos = selected_object->get_pos();

	if(lock_altitude) {
		obj_height = pos.y - wr->get_altitude(Point2D(pos.x, pos.z));
		obj_control = obj_height;
	} else {
		obj_altitude = pos.y;
		obj_control = obj_altitude;
	}
	
	obj_pos = pos;
	std::swap(obj_pos.y, obj_pos.z);
	cam_move(OBJ_SELECTED);

	glutPostRedisplay();
}


void draw()
{
	if(selected_object.valid()) {
		gfx::sphere(Matrix(selected_object->get_pos()),
			    selected_object->get_radius(),
			    Color(1,0,0,.2)).draw();

	}

	rr->render(set_cam());

	Frustum f = rr->setup_view(set_cam());

	if(draw_object_finders) {
		glColor3f(0,1,0);
		glBegin(GL_LINES);
		for(dyn_iterator i = wr->begin_dyn(); i != wr->end_dyn(); ++i) {
			if(i->is_dead())
				continue;

			misc::SmartPtr<Object> o;
			o.dynamic_assign((*i).get());

			if(filter_objects && o->get_mesh() != names[object_type])
				continue;

			if(filter_company && o->get_company() != companies[company_type])
				continue;

			Point p = o->get_pos();
			glVertex3fv(p.get());
			p.y = 1500;
			glVertex3fv(p.get());
		}
		glEnd();
		glEnable(GL_BLEND);
		glColor4f(0,1,1,.1);
		if(draw_object_finders) {
			for(dyn_iterator i = wr->find_dyn(f); i != wr->end_dyn(); ++i) {
				if(i->is_dead())
					continue;

				if((*i).get() != selected_object) {
					glPushMatrix();
					glTranslate(i->get_pos());
					gfx::misc::sphere(i->get_radius(),10,10);
					glPopMatrix();
				}
			}
		}
		glDisable(GL_BLEND);
	}

	ostringstream ss;
	ss << std::right << std::fixed << std::setprecision(0);
	ss << "Camera: " << camera_pos;
	if(selected_object.valid()) {
		ss << " -- Object type: " << selected_object->get_mesh();
		ss << " Company: " << selected_object->get_company();
		ss << " Position: " << selected_object->get_pos();
	}

	glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(1,1,1);
	gfx::font::glPutStr(0,0,ss.str());


}

}
}
