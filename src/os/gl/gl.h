
/*
 * Include this to use OpenGL, do not include GL/gl.h directly.
 */

#ifndef REAPER_HW_GL_H
#define REAPER_HW_GL_H

#ifdef WIN32

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "os/windows.h"

#endif

#ifdef SOLARIS
#define GL_SUN_triangle_list 1
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "ext/glh/glh_extensions.h"
#include "os/gl/gl_state.h"


#ifdef WIN32
inline
GLvoid* glAllocateMemoryNV(GLsizei size, GLfloat readfreq, GLfloat writefreq, GLfloat priority)
{
	return wglAllocateMemoryNV(size, readfreq, writefreq, priority);
}
inline
void glFreeMemoryNV(GLvoid *pointer)
{
	wglFreeMemoryNV(pointer);
}
#else
inline
void* glAllocateMemoryNV(int s,float r,float w,float p)
{
	return glXAllocateMemoryNV(s, r, w, s);
}
inline
void glFreeMemoryNV(void *pointer)
{
	glXFreeMemoryNV(pointer);
}
#endif


#ifdef SOLARIS
#undef GL_ARB_multitexture

//void glClientActiveTextureARB(GLenum);
//void glActiveTextureARB(GLenum);
//void glMultiTexCoord2fARB(unsigned,int,int);
#endif

#include "os/gl/gl_helper.h"

#ifdef X11
extern "C" {
//void* glXAllocateMemoryNV(int, float, float, float);
//void glXFreeMemoryNV(void *);
}


#ifdef MESA_GL

// GL_NV_FENCE
typedef PFNGLGENFENCESNVPROC    PFNGLGENFENCESNV;
typedef PFNGLDELETEFENCESNVPROC PFNGLDELETEFENCESNV;
typedef PFNGLSETFENCENVPROC     PFNGLSETFENCENV;
typedef PFNGLTESTFENCENVPROC    PFNGLTESTFENCENV;
typedef PFNGLFINISHFENCENVPROC  PFNGLFINSIHFENCENV;
typedef PFNGLISFENCENVPROC      PFNGLISFENCENV;
typedef PFNGLGETFENCEIVNVPROC   PFNGLGETFENCEIVNV;

#endif

#if 0
// GL_NV_FENCE
extern PFNGLGENFENCESNV    glGenFencesNV;
extern PFNGLDELETEFENCESNV glDeleteFencesNV;
extern PFNGLSETFENCENV     glSetFenceNV;
extern PFNGLTESTFENCENV    glTestFenceNV;
extern PFNGLFINSIHFENCENV  glFinishFenceNV;
extern PFNGLISFENCENV      glIsFenceNV;
extern PFNGLGETFENCEIVNV   glGetFenceivNV;
#endif

/*
extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC  glCompressedTexImage2DARB;
extern PFNGLGETCOMPRESSEDTEXIMAGEARBPROC glGetCompressedTexImageARB;
extern PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV;
extern PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
*/

#if 0
#ifdef NO_VERTEX_ARRAY_TYPEDEFS
typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);
#endif

#ifdef NO_VERTEX_ARRAY_PROTOTYPES
extern PFNGLLOCKARRAYSEXTPROC   glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;
#endif
#endif

#endif

// From X-headers. very annoying.
#ifdef None
#undef None
#endif

namespace reaper {
namespace os {
namespace gfx {

bool init_opengl_1_2();
void enable_extensions();


}
}
}

#endif

