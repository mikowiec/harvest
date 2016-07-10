#ifndef REAPER_HW_GL_HELPER_H
#define REAPER_HW_GL_HELPER_H

#include "os/gl.h"

#include "main/types.h"
#include "main/types4.h"

/* this shouldn't be here at all... */
#include "../src/gfx/gfx_types.h"

namespace reaper
{

using gfx::Color;
using gfx::ByteColor;
using gfx::TexCoord;

class StateKeeperBase
{
protected:
	GLenum state;
	bool value;
	bool prev_val;
public:
	StateKeeperBase(GLenum state, bool value);
	~StateKeeperBase();
};

/// Caches an OpenGL-state 
class StateKeeper : public StateKeeperBase
{
public:
	StateKeeper(GLenum state, bool value);
	~StateKeeper();
};

class ClientStateKeeper : public StateKeeperBase
{
public:
	ClientStateKeeper(GLenum state, bool value);
	~ClientStateKeeper();
};


// per-vertex helpers
//inline void glColor(const Color &c) { glColor3fv(c); }
inline void glColor3(const Color &c) { glColor3fv(c.get()); }
inline void glColor3(const ByteColor &c) { glColor3ubv(c.get()); }
inline void glColor4(const Color &c) { glColor4fv(c.get()); }
inline void glColor4(const ByteColor &c) { glColor4ubv(c.get()); }
	
#ifdef GL_EXT_secondary_color
//inline void glSecondaryColor(const Color &c) { glSecondaryColor3fvEXT(c); }
#endif

inline void glNormal(const Vector &n) { glNormal3fv(n.get()); }

inline void glTexCoord(const GLfloat f) { glTexCoord1f(f);     }
inline void glTexCoord(const TexCoord &t) { glTexCoord2fv(t.get()); }
//inline void glTexCoord3(const Vector4 &t) { glTexCoord3fv(t); }
//inline void glTexCoord(const Vector4 &t) { glTexCoord4fv(t); }

#ifdef GL_ARB_multitexture
//inline void glMultiTexCoord(GLenum unit, const GLfloat f) { glMultiTexCoord1fARB (unit, f);     }
inline void glMultiTexCoord(GLenum unit, const TexCoord & t) { glMultiTexCoord2fvARB(unit, t.get()); }
//inline void glMultiTexCoord3(GLenum unit, const Vector4 & t) { glMultiTexCoord3fvARB(unit, &t[0]); }
//inline void glMultiTexCoord4(GLenum unit, const Vector4 & t) { glMultiTexCoord4fvARB(unit, &t[0]); }
#endif


inline void glVertex(const Point2D & v) { glVertex2fv(v.get()); }
inline void glVertex(const Point & v) { glVertex3fv(v.get()); }
inline void glVertex(const Vector4 & v) { glVertex4fv(v.get()); }

// lighting helpers

inline void glMaterial(GLenum face, GLenum pname, GLint i)
{ glMateriali(face, pname, i); }

inline void glMaterial(GLenum face, GLenum pname, GLfloat f)
{ glMaterialf(face, pname, f); }

inline void glMaterial(GLenum face, GLenum pname, const Color & v)
{ glMaterialfv(face, pname, v.get()); }

inline void glLight(GLenum light, GLenum pname, GLint i)
{ glLighti(light, pname, i); } 

inline void glLight(GLenum light, GLenum pname, GLfloat f)
{ glLightf(light, pname, f); } 

inline void glLight(GLenum light, GLenum pname, const Vector4& v)
{ glLightfv(light, pname, v.get()); } 

template<typename T>
inline void glLight(GLenum light, GLenum pname, const Vec<T> & v)
{ glLightfv(light, pname, v.get()); } 

inline void glLight(GLenum light, GLenum pname, const Color & c)
{ glLightfv(light, pname, c.get()); } 

inline Matrix4 get_matrix(GLenum matrix)  {
	Matrix4 m;
	glGetFloatv(matrix, m.get());
	return m;
}

inline void glLoadMatrix(const Matrix4 & m) {
	glLoadMatrixf(m.get());	
}

inline void glMultMatrix(const Matrix4 & m) {
	glMultMatrixf(m.get());	
}

inline void glLoadMatrix(const Matrix & m) {
	glLoadMatrix(Matrix4(m));	
}

inline void glMultMatrix(const Matrix & m) {
	glMultMatrix(Matrix4(m));	
}
	
inline void glTranslate(const Point & t) {
	glTranslatef(t.x, t.y, t.z); 
}
inline void glScale    (const Vector & s) { 
	glScalef(s.x, s.y, s.z); 
}
inline void glRotate   (float angle, const Vector& axis) {
	glRotatef(angle, axis.x, axis.y, axis.z); 
}
inline void glRotate   (const Rotation &r) {
	glRotate(r.angle, r.axis); 
}
	
inline void texgen_mode(GLenum coord, GLint param)
{ glTexGeni(coord, GL_TEXTURE_GEN_MODE, param); }

inline void texenv_mode(GLint m)
{ glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m); }

// "get" (state query) helpers
inline GLboolean glGetBoolean(GLenum pname)
{
	GLboolean v;
	glGetBooleanv(pname, &v);
	return v;
}

inline GLfloat glGetFloat(GLenum pname)
{
	GLfloat v;
	glGetFloatv(pname, &v);
	return v;
}

inline GLint glGetInteger(GLenum pname)
{
	GLint i;
	glGetIntegerv(pname, &i);
	return i;
}

inline void glGet(int pname, double &v) { glGetDoublev(pname, &v); }
inline void glGet(int pname, double *v) { glGetDoublev(pname, v); }
inline void glGet(int pname, float &v) { glGetFloatv(pname, &v); }
inline void glGet(int pname, float *v) { glGetFloatv(pname, v); }
inline void glGet(int pname, int &v) { glGetIntegerv(pname, &v); }
inline void glGet(int pname, int *v) { glGetIntegerv(pname, v); }

inline void glGet(int pname, bool &v) {
        GLboolean b; glGetBooleanv(pname, &b); v = b == GL_TRUE; 
}
inline void glGet(int pname, bool *v) { 
        GLboolean b; glGetBooleanv(pname, &b); *v = b == GL_TRUE; 
}

inline Vector4 gl_get_vector(GLenum pname)
{
	Vector4 v;
	glGetFloatv(pname, v.get());
	return v;
}

/*
// handy for register combiners
inline vec3f range_compress(const vec3f & v)
{ vec3f vret(v); vret *= .5f; vret += .5f; return vret; }

  inline vec3f range_uncompress(const vec3f & v)
  { vec3f vret(v); vret -= .5f; vret *= 2.f; return vret; }
*/

inline void glClearColor(const Color &c)
{
	::glClearColor(c.r, c.g, c.b, c.a);
}	

inline void glClearColor(float r, float g, float b, float a)
{
	::glClearColor(r, g, b, a);
}	

inline int glGetTexLevelParameter2D(GLint lod,GLenum pname)
{
	int param;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, lod, pname, &param);
	return param;
}

inline void glTexParameter2D(GLenum pname, GLenum param)
{
	glTexParameteri(GL_TEXTURE_2D, pname, param);
}

inline void glBindTexture2D(GLuint name)
{
//	reaper::glBindTexture(GL_TEXTURE_2D, name);
	glBindTexture(GL_TEXTURE_2D, name);
}


// Ensures correct behaviour when used together with VertexArray 
// defined in gfx/vertex_array.cpp
void glDrawArrays(GLenum mode, GLint first, GLsizei count);
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices);

#ifdef X11
// urk!
inline void glEnableClientState(int v) {
	::glEnableClientState(static_cast<GLenum>(v));
}
inline void glActiveTextureARB(int v) {
	::glActiveTextureARB(static_cast<GLenum>(v));
};
inline int glGetInteger(int v) {
	return glGetInteger(static_cast<GLenum>(v));
}
inline void glGetIntegerv(int v, GLint* p) {
	::glGetIntegerv(static_cast<GLenum>(v), p);
}
#endif

}

#endif


// NOTE: THIS SOURCE HAS BEEN MODIFIED TO SUIT THE REAPER FRAMEWORK
/*
glh - is a platform-indepenedent C++ OpenGL helper library 

  
    Copyright (c) 2000 Cass Everitt
    Copyright (c) 2000 NVIDIA Corporation
    All rights reserved.
    
      Redistribution and use in source and binary forms, with or
      without modification, are permitted provided that the following
      conditions are met:
      
	* Redistributions of source code must retain the above
	copyright notice, this list of conditions and the following
	disclaimer.
	
	  * Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the following
	  disclaimer in the documentation and/or other materials
	  provided with the distribution.
	  
	    * The names of contributors to this software may not be used
	    to endorse or promote products derived from this software
	    without specific prior written permission. 
	    
	      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	      ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
	      FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	      REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	      INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	      BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	      LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	      CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	      LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	      ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
	      POSSIBILITY OF SUCH DAMAGE. 
	      
		
		  Cass Everitt - cass@r3.nu
*/

/*
 * $Author: pstrand $
 * $Date: 2003/03/31 15:19:19 $
 * $Log: gl_helper.h,v $
 * Revision 1.5  2003/03/31 15:19:19  pstrand
 * mindre fixar..
 *
 * Revision 1.4  2003/03/12 12:23:14  pstrand
 * basfall!
 *
 * Revision 1.3  2003/01/11 23:59:12  fizzgig
 * improved opengl implementation reporting
 *
 * Revision 1.2  2003/01/06 23:05:17  pstrand
 * hw->os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.20  2002/05/20 09:59:58  fizzgig
 * dynamic environment mapping!
 *
 * Revision 1.19  2002/03/11 10:50:48  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.18  2002/01/28 00:50:16  macke
 * gl_state stuff should be in gl_state, not here as well...
 *
 * Revision 1.17  2002/01/10 23:09:11  macke
 * massa bök
 *
 * Revision 1.16  2001/08/20 16:13:53  peter
 * StateKeeper
 *
 * Revision 1.15  2001/08/06 12:16:22  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 *
 */
