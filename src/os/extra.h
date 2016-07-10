/*
 * Reexports various functions found on 
 * most (posix|unix|windows) systems,
 * but localizes the trouble of finding 
 * the corrent includes to one file,
 * os/extra.cpp
 */

#ifndef REAPER_OS_EXTRA_H
#define REAPER_OS_EXTRA_H

#include <stdio.h>

namespace reaper {

int snprintf (char* buf, size_t maxlen, const char* fmt, ...);

FILE* popen(const char* file, const char* mode);
int pclose(FILE*);

}

#endif

