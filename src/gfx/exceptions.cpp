#include "os/compat.h"

#include "gfx/exceptions.h"
#include "gfx/settings.h"

#include "os/gl.h"

namespace reaper {
namespace gfx {   

const bool *gl_error::test_for_errors = &Settings::current.test_for_errors;

void throw_on_gl_error_check(const std::string &s)
{                
        GLenum err = glGetError();
	if(err) {
		const char *gl_err_str = reinterpret_cast<const char*>(gluErrorString(err));
                throw gfx_fatal_error(s + " - OpenGL error: " + gl_err_str);
        }
}

}
}
