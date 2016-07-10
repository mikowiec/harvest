#include "os/compat.h"

#include "gfx/mesh.h"
#include "gfx/exceptions.h"
#include "gfx/io.h"
#include "gfx/pm/pm.h"
#include "os/debug.h"
#include "os/gl.h"
#include "res/res.h"

#include "main/types_ops.h"
#include "main/types_io.h"


#include <list>
#include <stdexcept>
#include <iterator>

namespace reaper {
namespace {
	debug::DebugOutput dout("gfx::Object",0);
}
namespace gfx {
namespace mesh {
using namespace std;

const string Sheep::name("sheep");

Sheep::Sheep()
{
	Material material;

	material.ambient_color = Color(.2,.2,.2);
	material.diffuse_color = Color(1,1,1);
	material.emission_color = Color(0,0,0);
	material.specular_color = Color(.6,.6,.6);
	material.shininess = 80;

	GLUquadricObj *qobj = gluNewQuadric();

	gluQuadricTexture(qobj,GL_TRUE);

	list.begin();
		material.use();								
		glRotatef(90,1,0,0); // rotate so z axis points downwards
		gluSphere(qobj,1,20,20);
		material.diffuse_color = Color(0,0,0);
		material.use();

		gluQuadricTexture(qobj,GL_FALSE);

		glTranslatef(0,-.9,.2);
		gluSphere(qobj,.2,10,10);
		glTranslatef(0,.9,-.2);

		glTranslatef(.5,.5,0);			
		gluCylinder(qobj,.1,.1,1,8,1);
		glTranslatef(-1,0,0);
		gluCylinder(qobj,.1,.1,1,8,1);
		glTranslatef(0,-1,0);
		gluCylinder(qobj,.1,.1,1,8,1);
		glTranslatef(1,0,0);
		gluCylinder(qobj,.1,.1,1,8,1);
	list.end();

	gluDeleteQuadric(qobj);		

	throw_on_gl_error("Sheep::Sheep()");
}

void Sheep::render(int lod) const
{
	list.call();
	throw_on_gl_error("Sheep::render()");
}

}
}
}

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:33 $
 * $Log: mesh.cpp,v $
 * Revision 1.22  2002/12/29 17:04:33  pstrand
 * hw/ moved to os/
 *
 * Revision 1.21  2002/09/23 12:08:43  fizzgig
 * Moved all anonymous namespaces into the reaper-namespace
 *
 * Revision 1.20  2002/04/23 22:50:18  fizzgig
 * mesh-loading cleanup
 *
 * Revision 1.19  2002/03/04 10:22:49  fizzgig
 * sheep!
 *
 * Revision 1.18  2002/02/08 11:27:52  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.17  2002/01/17 04:56:56  peter
 * gcc-2 is happier this way..
 *
 * Revision 1.16  2002/01/16 00:25:27  picon
 * terrängbelysning
 *
 * Revision 1.15  2001/07/30 23:43:17  macke
 * Häpp, då var det kört.
 *
 * Revision 1.14  2001/07/23 23:48:10  macke
 * Slimmad grafikhantering samt lite namnbyten
 *
 * Revision 1.39  2001/07/09 13:33:06  peter
 * gcc-3.0 fixar
 *
 * Revision 1.38  2001/06/07 05:14:19  macke
 * Reaper v0.9
 *
 * Revision 1.37  2001/05/15 01:42:26  peter
 * minskade ner på debugutskrifterna
 *
 * Revision 1.36  2001/05/15 00:06:24  peter
 * bitrötaburk, jag flyttar!
 *
 * Revision 1.35  2001/05/10 11:40:14  macke
 * häpp
 *
 */

