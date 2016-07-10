#include "os/compat.h"
#include <vector>
#include "os/gl.h"
#include "os/gl/gl_info.h"
#include "gfx/vertex_array.h"
#include "gfx/static_geom.h"
#include "gfx/settings.h"
#include "gfx/exceptions.h"

namespace reaper {
namespace gfx {
namespace misc {

class StaticGeometry::VARGeometry {
public:
	const float *v;
	const float *c;
	const float *n;
	const float *t;

	size_t stride;
	size_t num_verts;
	std::vector<int> indices;
	size_t num_indices;

	lowlevel::VertexArray<float> array;

	VARGeometry(const Point *verts,    const Color *colors,
		    const Vector *normals, const TexCoord *tcoords,
		    size_t str, size_t nv, const int *i, size_t ni)
	: v(0), c(0), n(0), t(0),
	  stride(str), num_verts(nv),
	  num_indices(ni),
	  array(stride * num_verts)
	{
		if(i) {
			indices.resize(ni);
			std::copy(i, i+ni, indices.begin());
		}

		v = array.get();
		//FIXME: eek!
		c = reinterpret_cast<const float*>(colors)  ? v + 3 : 0;
		n = reinterpret_cast<const float*>(normals) ? v + 3 + (c ? 4 : 0) : 0;
		t = reinterpret_cast<const float*>(tcoords) ? v + 3 + (c ? 4 : 0) + (n ? 3 : 0) : 0;
	}
};

StaticGeometry::StaticGeometry(
	const Point *v,  const Color *c,
	const Vector *n, const TexCoord *t, 
	bool separate_arrays, size_t num_verts, 
	const int *indices, size_t num_indices) :
	vargeom(0)
{
	init(v,c,n,t,separate_arrays,num_verts,indices,num_indices);
}

StaticGeometry::StaticGeometry() : vargeom(0) {}

StaticGeometry::~StaticGeometry() { }

void StaticGeometry::init(
	const Point *v,  const Color *c,
	const Vector *n, const TexCoord *t, 
	bool separate_arrays, size_t num_verts, 
	const int *indices, size_t num_indices)
{
	const int stride = (3 + (c ? 4 : 0) + (n ? 3 : 0) + (t ? 2 : 0)) * sizeof(float);

	if(Settings::current.use_nv_vertex_array_range) {
		// calculate offsets
		vargeom = reaper::misc::SmartPtr<VARGeometry>(new VARGeometry(v, c, n, t, stride,
							  num_verts, indices, num_indices));
		float *ptr = vargeom->array.get();
		for(int i=0; i<num_verts; i++) {
			*ptr++ = v[i].x;
			*ptr++ = v[i].y;
			*ptr++ = v[i].z;
			if(c) {
				*ptr++ = c[i].r;
				*ptr++ = c[i].g;
				*ptr++ = c[i].b;
				*ptr++ = c[i].a;
			}
			if(n) {
				*ptr++ = n[i].x;
				*ptr++ = n[i].y;
				*ptr++ = n[i].z;
			}
			if(t) {
				*ptr++ = t[i].s;
				*ptr++ = t[i].t;
			}
		}
	} else {
		//FIXME: eek!
		setup(reinterpret_cast<const float*>(v), 
			reinterpret_cast<const float*>(c), 
			reinterpret_cast<const float*>(n), 
			reinterpret_cast<const float*>(t), 
			separate_arrays ? 0 : stride);
		list.begin();

// totally sucky and utterly stupid matrox driver workaround... 
#ifdef WIN32
		const os::gfx::OpenGLinfo& gl = os::gfx::opengl_info();

		if(gl.renderer() == "Matrox G200") {
			glBegin(GL_TRIANGLES);
			if(indices) {
				for(const int *i = indices; i < indices + num_indices; i++) {
					if(c) glColor4(c[*i]);
					if(t) glTexCoord(t[*i]);
					if(n) glNormal(n[*i]);
					glVertex(v[*i]);
				}
			} else {
				for(int i = 0; i < num_verts; i++) {
					if(c) glColor4(c[i]);
					if(t) glTexCoord(t[i]);
					if(n) glNormal(n[i]);
					glVertex(v[i]);
				}
			}
			glEnd();
		} else 
#endif		
		{
			if(indices) {
				glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);
			} else {
				glDrawArrays(GL_TRIANGLES, 0, num_verts);
			}
		}
		list.end();
		restore(reinterpret_cast<const float*>(c),
			reinterpret_cast<const float*>(n),
			reinterpret_cast<const float*>(t));
	}

	throw_on_gl_error("StaticGeometry::init()");
}

void StaticGeometry::render(const VARGeometry *vg)
{
	setup(vg->v, vg->c, vg->n, vg->t, vg->stride);
	if(vg->num_indices > 0) {
		vg->array.glDrawElements(GL_TRIANGLES, vg->num_indices, 
                                         GL_UNSIGNED_INT, &vg->indices[0]);
	} else {
		vg->array.glDrawArrays(GL_TRIANGLES, 0, vg->num_verts);
	}
	restore(vg->c, vg->n, vg->t);
}

void StaticGeometry::setup(const float *v, const float *c,
	 		   const float *n, const float *t,
			   size_t stride)
{
	glVertexPointer(3, GL_FLOAT, stride, v);
	if(c) {
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, stride, c);
	}
	if(n) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, stride, n);
	}
	if(t) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, stride, t);
	}
}	

//FIXME: Lots of unneccesary state switches back & forth 
void StaticGeometry::restore(const float *c, const float *n, const float *t)
{
	if(c) {
		glDisableClientState(GL_COLOR_ARRAY);
	}
	if(n) {
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(t) {
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

StaticGeometry::StaticGeometry(const StaticGeometry& sg)
 : list(sg.list), vargeom(sg.vargeom)
{ }

StaticGeometry& StaticGeometry::operator=(const StaticGeometry &sg) 
{
	list = sg.list;
	vargeom = sg.vargeom;
	return *this;
}

}
}
}

/*
 * $Author: fizzgig $
 * $Date: 2003/01/18 10:43:08 $
 * $Log: static_geom.cpp,v $
 * Revision 1.14  2003/01/18 10:43:08  fizzgig
 * gl cleanup
 *
 * Revision 1.13  2003/01/06 12:42:02  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.12  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.11  2002/06/06 07:59:38  pstrand
 * gl init, settings, fix
 *
 * Revision 1.10  2002/04/23 22:50:19  fizzgig
 * mesh-loading cleanup
 *
 * Revision 1.9  2002/04/08 21:12:50  pstrand
 * no vector<>::assign in egcs
 *
 * Revision 1.8  2002/04/08 20:07:08  pstrand
 * valarray -> vector<float>  (just for compat with egcs, with is the only place valarray is used...)
 *
 * Revision 1.7  2002/02/13 21:57:18  macke
 * misc effects modifications
 *
 * Revision 1.6  2002/02/07 00:02:52  macke
 * STLPort 4.5.1 Win32-fix + shadow & render reorg
 *
 * Revision 1.5  2002/01/27 12:04:08  macke
 * matrox final fix?
 *
 * Revision 1.4  2002/01/26 23:27:13  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.3  2002/01/23 09:47:56  macke
 * hmf!
 *
 * Revision 1.1  2002/01/22 22:38:07  macke
 * massive gfx cleanup
 *
 */
