#include "os/compat.h"

#include "main/types_ops.h"
#include "gfx/camera.h"
#include "gfx/settings.h"
#include "gfx/misc.h"
#include "os/gl.h"

namespace reaper {
namespace gfx {

Camera::Camera(const Point &p, const Point &lookat, const Vector &u, float hfov, float vfov) :
	pos(p), front(norm(lookat-p)), up(u), horiz_fov(hfov), vert_fov(vfov)
{}

Camera::Camera(const Point &p, const Vector &f, const Vector &u, float hfov, float vfov) :
	pos(p), front(f), up(u), horiz_fov(hfov), vert_fov(vfov)
{}
Camera::Camera(const OldCamera &c) :
	pos(c.eye), front(norm(c.center-c.eye)), up(c.up), horiz_fov(c.horiz_fov), vert_fov(c.vert_fov)
{}

OldCamera::OldCamera(const Point &e, const Point  &look, const Vector &u, float hfov, float vfov) :
	eye(e), center(look), up(u), horiz_fov(hfov), vert_fov(vfov)
{}

const world::Frustum& Camera::setup_view() const
{
	const float view_dist = Settings::current.maximum_view_distance;

	// Set projection matrix
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(vert_fov, horiz_fov/vert_fov, 1, view_dist);

	// Set viewing matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Point lookat = pos + front;
	gluLookAt(pos.x, pos.y, pos.z,
		  lookat.x, lookat.y, lookat.z,
		  up.x, up.y, up.z);		

	// Set current viewing frustum
	Vector right = norm(cross(front,up));
	Vector up2 = norm(cross(front,right));
	frustum = world::Frustum(pos, front * view_dist / 2, up2, horiz_fov, vert_fov);

	misc::BillBoard::set_vectors(right,up2);
	
	return frustum;
}

}
}
