#include "os/compat.h"
#include "os/gl.h"
#include "os/debug.h"

#include "ext/glh/glh_extensions.h"

#include "os/gl/gl_info.h"
#include "main/exceptions.h"

#include <cstdio>
#include <map>
#include <sstream>

using namespace std;

namespace reaper {
namespace {
void APIENTRY ReaperGLDrawRangeElements(GLenum mode, GLuint start, GLuint end, 
                                        GLsizei count, GLenum type, const GLvoid *indices)
{
	glDrawElements(mode,count,type,indices);
}

reaper::debug::DebugOutput dout("os::gl",2);
} // end anonymous namespace

#ifdef WIN32
typedef int (__stdcall *funcptr)();
#else
typedef int (*funcptr)();
#endif

funcptr get_proc_address(const char* p)
{
#ifdef WIN32
	return wglGetProcAddress(p);
#else
#ifdef SOLARIS
	return 0;
#else
	return (funcptr)glXGetProcAddressARB((const GLubyte*)p);
#endif
#endif
}
} // end namespace reaper

#ifdef WIN32

namespace reaper {
namespace os {
namespace gfx {


}
}
}

#else

void not_impl(const char* s) { printf("GL call not implemented: %s\n", s); exit(2); }

#ifdef SOLARIS

//void glClientActiveTextureARB(GLenum) { not_impl("glClientActiveTextureARB");  }
//void glActiveTextureARB(GLenum) { not_impl("glActiveTextureARB");  }

/*
void* glXAllocateMemoryNV(int, float, float, float) {
	not_impl(); 
	return 0;
}
void glXFreeMemoryNV(void*) { not_impl();  }
*/

//void APIENTRY glLockArraysEXT (GLint, GLsizei) { not_impl(); }
//void APIENTRY glUnlockArraysEXT (void) { not_impl(); }


void APIENTRY glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices)
{
	reaper::ReaperGLDrawRangeElements(mode, start, end, count, type, indices);
}

/*
void APIENTRY glMultiTexCoord2fARB (unsigned int, int, int)
{
	not_impl("glMultiTexCoord2fARB");
}
*/

#endif


#endif


