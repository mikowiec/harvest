#include "os/compat.h"
#include "pmvertex.h"
#include <vector>
#include "main/types.h"
#include "contractions.h"
#include <iostream>


namespace reaper 
{
namespace gfx
{
namespace mesh
{

using std::cout;

void TexCoord::set_alpha(float param)
{
	a[3] = a[4] = a[5] = param;
}

TexCoord TexCoord::operator+(const TexCoord& b)
{
	TexCoord r;

	r.u = u + b.u;
	r.v = v + b.v;
	r.a[3] = r.a[4] = r.a[5] = a[3] + b.a[3];

	r.a[0] = a[0] + b.a[0];
	r.a[1] = a[1] + b.a[1];
	r.a[2] = a[2] + b.a[2];
	
	return r;
}

TexCoord TexCoord::operator*(const float f)
{
	TexCoord r;

	r.u = f*u;
	r.v = f*v;
	r.a[3] = r.a[4] = r.a[5] = f*a[3];

	r.a[0] = f*r.a[0];
	r.a[1] = f*r.a[1];
	r.a[2] = f*r.a[2];

	return r;
}

TexCoord TexCoord::operator=(const TexCoord& b)
{
	u = b.u;
	v = b.v;
	a[3] = a[4] = a[5] = b.a[3];
	a[0] = b.a[0];
	a[1] = b.a[1];
	a[2] = b.a[2];

	return *this;
}


PMVertex& PMVertex::operator=(const Contraction& c)
{
	
	vt = c.vt;
	vu = c.vu;

	normal = c.normal;
	color = c.color;
	
	point = c.get_point();
	
	return *this;

}

PMVertex::PMVertex(const Contraction& c)
{
	*this = c;
	color.x = c.color.x;
	color.y = c.color.y;
	color.z = c.color.z;

	flr[0] = flr[1] = -1;
}

void PMVertex::render()
{
		glBegin(GL_TRIANGLES);  //tells OpenGL that we're going to start drawing triangles

				glColor3f(1.0, 1.0, 1.0);
				glVertex3f(point.x, point.y+1.0, point.z);
				glVertex3f(point.x-0.66, point.y-0.66, point.z);
				glVertex3f(point.x+0.66, point.y-0.66, point.z);

		glEnd();
}

bool PMVertex::has_children()
{
	return (vt != -1);
}

bool PMVertex::has_parent()
{
	return (parent != -1);
}


void PMVertex::report(int i) const
{
	cout << "\n";
	cout << "-----------------------\n";
	cout << "| PMVertex reporting: |\n";
	cout << "-----------------------\n";
	cout << "vt:" << vt << "\tvu:" << vu << "\n";
	cout << "parent: " << parent << "\n";
	cout << "flr: " << flr[0] << ", " << flr[1] << "\n";
	cout << "fn: " << fn[0] << ", " << fn[1] << ", " << fn[2] << ", " << fn[3] << "\n";
	cout << "fni: " << fni[0] << ", " << fni[1] << ", " << fni[2] << ", " << fni[3] << "\n";
	cout << "My number is: " << i << " and I am ";
	if(is_active)cout << "active!\n";
	else cout << " NOT active.\n";
	cout << "@ " << "(" << point.x << ", " << point.y << ", " << point.z << ")\n";
	cout << "\n";
}

void PMFace::report(int i) const
{
	cout << "\n";
	cout << "---------------------\n";
	cout << "| PMFace reporting: |\n";
	cout << "---------------------\n";
	cout << "vertices: (" << vertices[0] << ", " << vertices[1] << ", " << vertices[2] << ")\n";
	cout << "neighbours: " << nf[0] << ", " << nf[1] << ", " << nf[2] << "\n";
	cout << "My number is: " << i << " and I am ";
	if(is_active)cout << "ACTIVE\n";
	else cout << "INACTIVE\n";
	cout << "\n";
}

}
}
}
