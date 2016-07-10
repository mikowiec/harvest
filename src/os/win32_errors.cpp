
/* $Id: win32_errors.cpp,v 1.2 2003/06/04 18:55:45 pstrand Exp $ */

#include "os/compat.h"

#include "os/win32_errors.h"

#include "os/windows.h"
#include "os/extra.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef E_PENDING
#define E_PENDING                       0x80070007L
#endif
#define _FACDS  0x878


#define MAKE_DSHRESULT(code)  MAKE_HRESULT(1, _FACDS, code)
#define DSERR_ALLOCATED                 MAKE_DSHRESULT(10)
#define DSERR_CONTROLUNAVAIL            MAKE_DSHRESULT(30)
#define DSERR_INVALIDPARAM              E_INVALIDARG
#define DSERR_INVALIDCALL               MAKE_DSHRESULT(50)
#define DSERR_GENERIC                   E_FAIL
#define DSERR_PRIOLEVELNEEDED           MAKE_DSHRESULT(70)
#define DSERR_OUTOFMEMORY               E_OUTOFMEMORY
#define DSERR_BADFORMAT                 MAKE_DSHRESULT(100)
#define DSERR_UNSUPPORTED               E_NOTIMPL
#define DSERR_NODRIVER                  MAKE_DSHRESULT(120)
#define DSERR_ALREADYINITIALIZED        MAKE_DSHRESULT(130)
#define DSERR_NOAGGREGATION             CLASS_E_NOAGGREGATION
#define DSERR_BUFFERLOST                MAKE_DSHRESULT(150)
#define DSERR_OTHERAPPHASPRIO           MAKE_DSHRESULT(160)
#define DSERR_UNINITIALIZED             MAKE_DSHRESULT(170)
#define DSERR_NOINTERFACE               E_NOINTERFACE


#define DIERR_OLDDIRECTINPUTVERSION     \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_OLD_WIN_VERSION)
#define DIERR_BETADIRECTINPUTVERSION    \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_RMODE_APP)
#define DIERR_BADDRIVERVER              \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_BAD_DRIVER_LEVEL)
#define DIERR_DEVICENOTREG              REGDB_E_CLASSNOTREG
#define DIERR_NOTFOUND                  \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND)
#define DIERR_OBJECTNOTFOUND            \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND)
#define DIERR_INVALIDPARAM              E_INVALIDARG
#define DIERR_NOINTERFACE               E_NOINTERFACE
#define DIERR_GENERIC                   E_FAIL
#define DIERR_OUTOFMEMORY               E_OUTOFMEMORY
#define DIERR_UNSUPPORTED               E_NOTIMPL
#define DIERR_NOTINITIALIZED            \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_NOT_READY)
#define DIERR_ALREADYINITIALIZED        \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_ALREADY_INITIALIZED)
#define DIERR_NOAGGREGATION             CLASS_E_NOAGGREGATION
#define DIERR_OTHERAPPHASPRIO           E_ACCESSDENIED
#define DIERR_INPUTLOST                 \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_READ_FAULT)
#define DIERR_ACQUIRED                  \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_BUSY)
#define DIERR_NOTACQUIRED               \
    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_INVALID_ACCESS)
#define DIERR_READONLY                  E_ACCESSDENIED
#define DIERR_HANDLEEXISTS              E_ACCESSDENIED
#define DIERR_INSUFFICIENTPRIVS         0x80040200L
#define DIERR_DEVICEFULL                0x80040201L
#define DIERR_MOREDATA                  0x80040202L
#define DIERR_NOTDOWNLOADED             0x80040203L
#define DIERR_HASEFFECTS                0x80040204L
#define DIERR_NOTEXCLUSIVEACQUIRED      0x80040205L
#define DIERR_INCOMPLETEEFFECT          0x80040206L
#define DIERR_NOTBUFFERED               0x80040207L
#define DIERR_EFFECTPLAYING             0x80040208L


char* ds_error(int e)
{

	static char errstr[100];
	switch (e) {
	case E_PENDING:			strcpy(errstr, "Pending"); break;
	case DSERR_ALLOCATED:		strcpy(errstr, "Already allocated"); break;
	case DSERR_NOAGGREGATION:	strcpy(errstr, "No aggregation support"); break;
	case DSERR_NODRIVER:		strcpy(errstr, "No driver found"); break;
	case DSERR_INVALIDPARAM:	strcpy(errstr, "Invalid param"); break;
	case DSERR_ALREADYINITIALIZED:	strcpy(errstr, "Already initialized"); break;
	case DSERR_GENERIC:		strcpy(errstr, "Generic error"); break;
	case DSERR_BADFORMAT:		strcpy(errstr, "Bad format"); break;
	case DSERR_OUTOFMEMORY:		strcpy(errstr, "Out of memory"); break;
	case DSERR_UNINITIALIZED:	strcpy(errstr, "Uninitialized"); break;
	case DSERR_UNSUPPORTED:		strcpy(errstr, "Unsupported"); break;
	case DSERR_PRIOLEVELNEEDED:	strcpy(errstr, "Prio level needed"); break;
	default:	reaper::snprintf(errstr, 100, "unknown err: %x", e); break;
	}
	return errstr;
}


char* di_error(int e)
{

	static char errstr[100];
	switch (e) {

	case DIERR_INPUTLOST:		strcpy(errstr, "Input lost"); break;
	case DIERR_INVALIDPARAM:	strcpy(errstr, "Invalid param"); break;
	case DIERR_NOTACQUIRED:		strcpy(errstr, "Not acquired"); break;
	case DIERR_NOTINITIALIZED:	strcpy(errstr, "Not initialized"); break;
	case E_PENDING:			strcpy(errstr, "Pending"); break;
	case DIERR_BETADIRECTINPUTVERSION: strcpy(errstr, "Betadirectinputversion"); break;
	case DIERR_OLDDIRECTINPUTVERSION:  strcpy(errstr, "Olddirectinputversion"); break;
	case DIERR_OUTOFMEMORY:		strcpy(errstr, "Out of memory"); break;
	case DIERR_OTHERAPPHASPRIO:	strcpy(errstr, "Other app has prio"); break;

	default:	reaper::snprintf(errstr, 100, "unknown err: %x", e); break;
	}
	return errstr;
}

const char* win32_strerror()
{
	static char msgbuf[1000];
	FormatMessage( 
	    FORMAT_MESSAGE_FROM_SYSTEM | 
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
	    GetLastError(),
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	    (LPTSTR) &msgbuf,
	    0,
	    NULL 
	);
	return msgbuf;
}


#ifdef __cplusplus
}
#endif

