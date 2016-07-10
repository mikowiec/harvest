
/* $Id: win32_errors.h,v 1.1 2002/12/29 16:58:14 pstrand Exp $ */


#ifndef REAPER_HW_WIN32_ERRORS_H
#define REAPER_HW_WIN32_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif


char* ds_error(int e);
char* di_error(int e);

const char* win32_strerror();

#ifdef __cplusplus
}
#endif

#endif

