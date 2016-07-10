#ifndef REAPER_AI_EXCEPTIONS_H
#define REAPER_AI_EXCEPTIONS_H

#include "main/exceptions.h"
#include <string>

namespace reaper
{
namespace ai
{   

        using std::string;

        /**@name Exceptions - base classes */
        //@{
                /// Fatal errors
                class ai_fatal_error
                        : public fatal_error_base
                {
                protected:
                        string err;
                public:
                        ai_fatal_error(const string &s) : err(s) {}
                        const char* what() const { return err.c_str(); }
                };

                /// Non-fatal errors
                class ai_error
                        : public error_base
                {   
                protected:
                        string err;
                public:
                        ai_error(const string &s) : err(s) {}
                        const char* what() const { return err.c_str(); }
                };
        //@}
}
}
#endif

/*
 * $Author: peter $
 * $Date: 2001/08/06 12:16:00 $
 * $Log: exceptions.h,v $
 * Revision 1.6  2001/08/06 12:16:00  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.5.4.1  2001/08/03 13:43:44  peter
 * pragma once obsolete...
 *
 * Revision 1.5  2001/07/06 01:47:05  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.4  2001/01/04 23:07:24  niklas
 * no message
 *
 * Revision 1.3  2001/01/03 13:40:51  peter
 * *** empty log message ***
 *
 * Revision 1.2  2000/12/04 19:02:41  niklas
 * *** empty log message ***
 *
 * Revision 1.1  2000/12/04 12:23:15  niklas
 * Lade till ai/exeptions.h
 *
 */


