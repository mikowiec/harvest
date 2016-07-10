/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:34 $
 * $Log: misc.cpp,v $
 * Revision 1.37  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.36  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.35  2002/04/14 19:27:06  pstrand
 * try again... (uniqueptr)
 *
 * Revision 1.34  2002/04/14 09:22:04  pstrand
 * quadtree redesign and Sphere::radius -> Sphere::r
 *
 * Revision 1.33  2002/04/11 01:03:34  pstrand
 * explicit res_stream exception...
 *
 * Revision 1.32  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.31  2002/02/08 11:27:53  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.30  2002/01/26 23:27:12  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.29  2002/01/23 10:17:36  macke
 * hmf!
 *
 */

#include "os/compat.h"


#include "gfx/misc.h"
#include "gfx/abstracts.h"
#include "gfx/camera.h"
#include "gfx/exceptions.h"
#include "gfx/settings.h"
#include "gfx/displaylist.h"
#include "gfx/managers.h"

#include "os/gl.h"
#include "os/debug.h"
#include "world/query_obj.h"
#include "main/types_ops.h"

namespace reaper {
namespace gfx {
namespace misc {
namespace {
reaper::debug::DebugOutput dout("gfx::misc",0);

GLUquadricObj* init_sphere()
{
	GLUquadricObj* q = gluNewQuadric();
	gluQuadricNormals(q, GLU_SMOOTH);
	return q;
}

GLUquadricObj* sphere_q = init_sphere();

}

int DisplayList::compilation_in_progress = 0;

//////////////////////////////////////

void meter(float x, float y, float length, float width, float maxval, float value)
{
	float rel_value = (length/maxval)*value;
	rel_value = std::min(rel_value,1.0f);
	rel_value = std::max(rel_value,0.0f);
	glRectf(x, y, x+rel_value, y+width);
}

void sphere(double radius, int slices, int stacks)
{
	gluSphere(sphere_q,radius,slices,stacks);
}

void sphere(const world::Sphere &s, int slices, int stacks)
{
	glPushMatrix();
	glTranslate(s.p);
	sphere(s.r,slices,stacks);
	glPopMatrix();
}

/////////////////////////////////////////////////////////////////////////////////////////////


} // end namespace misc

void SimEffect::simulate(float dt)
{
	throw gfx_fatal_error("SimEffect::simulate - not implemented!");
}

void Material::use() const
{
        glMaterial(GL_FRONT,GL_AMBIENT,ambient_color);
        glMaterial(GL_FRONT,GL_DIFFUSE,diffuse_color);
        glMaterial(GL_FRONT,GL_SPECULAR,specular_color);
        glMaterial(GL_FRONT,GL_EMISSION,emission_color);
        glMaterial(GL_FRONT,GL_SHININESS,shininess);
        throw_on_gl_error("Material::use() - exiting");
}

RenderInfo::RenderInfo(const std::string &mesh_file, const Matrix &m, bool b)
: next_ri(0), mtx(&m), param(0), blend(b), radius(0), mesh(mesh_file),
  texture("")
{
	MeshRef mr;

	radius  = mr->get_radius(mesh);
	texture = mr->get_texture(*this);
}

RenderInfo::RenderInfo(const std::string &mesh_file, const Matrix &m, MtxParams &p, bool b)
: next_ri(0), mtx(&m), param(&p), blend(b), radius(0), mesh(mesh_file),
  texture("")
{
	MeshRef mr;

	radius = mr->get_radius(mesh);
	texture = mr->get_texture(*this);
}

void RenderInfo::link(RenderInfo &ri)
{
	if(next_ri) {
		if(ri.next_ri) {
			throw gfx_fatal_error("Illegal attempt to link two renderinfo-lists...");
		}
		ri.next_ri = next_ri;
	}
	next_ri = &ri;
}			

RenderInfo& RenderInfo::operator=(const RenderInfo &ri)
{
	next_ri = ri.next_ri;
	mtx     = ri.mtx;
	param   = ri.param;
	blend   = ri.blend;
	radius  = ri.radius;
	mesh    = ri.mesh;
	texture = ri.texture;
	return *this;
}


}
}
 
