

#ifndef REAPER_GFX_CLOUD_MISC_H
#define REAPER_GFX_CLOUD_MISC_H

#include "main/types.h"
#include "main/types4.h"
#include "main/types4_ops.h"
#include "os/gl.h"

namespace reaper {
namespace gfx {
namespace cloud {

struct Project
{
	Matrix4 mv;
	Matrix4 proj;
	int width, height;
	Project() {
		save();
	}

	void save() {
		glGetFloatv(GL_MODELVIEW_MATRIX, mv.get());
		glGetFloatv(GL_PROJECTION_MATRIX, proj.get());
		int vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		width = vp[2];
		height = vp[3];
	}

	Vector up() const {
		return Vector(mv[0][1], mv[1][1], mv[2][1]);
	}

	Vector right() const {
		return Vector(mv[0][0], mv[1][0], mv[2][0]);
	}

	Point pos() const {
		float f = mv[3][3];
		return Vector(mv[3][0]/f, mv[3][1]/f, mv[3][2]/f);
	}

	void use() {
		glMatrixMode(GL_PROJECTION);
		glLoadMatrix(proj);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrix(mv);
	}

	Point2D operator()(const Point& p) const {
		Vector4 v = proj * mv * Vector4(p);
		Point p_vp = v.as_point() / v.get()[3];
		return Point2D((1+p_vp.x) * width * 0.5, (1+p_vp.y) * height * 0.5);
	}

};


}
}
}

#endif


