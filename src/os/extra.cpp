
#define _BSD_SOURCE
#define _POSIX_C_SOURCE 199506L

#include "os/compat.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef WIN32
#define vsnprintf _vsnprintf
#endif

namespace reaper {


int snprintf(char* buf, size_t maxlen, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int r = vsnprintf(buf, maxlen, fmt, ap);
	va_end(ap);
	return r;
}

#ifndef WIN32
FILE* popen(const char* file, const char* mode)
{
	return ::popen(file, mode);
}

int pclose(FILE* fp)
{
	return ::pclose(fp);
}
#endif

}


