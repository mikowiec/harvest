#ifndef GLH_EXTENSIONS
#define GLH_EXTENSIONS

#include <string.h>
#include <stdio.h>

#ifdef _WIN32
# include "os/windows.h"
#else
# include <stdlib.h>
#endif

#include <GL/gl.h>

#ifdef _WIN32
# include <GL/wglext.h>
#else
# include <GL/glx.h>
# include <GL/glxext.h>
#endif

#include "glh_genext.h"

int glh_init_extensions(const char *origReqExts);
const char* glh_get_unsupported_extensions();
void glh_shutdown_extensions();

#endif /* GLH_EXTENSIONS */
