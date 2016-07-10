#ifndef REAPER_HW_FDSTREAM_H
#define REAPER_HW_FDSTREAM_H

#include <iostream>

namespace reaper
{
namespace os
{


class fd_streambuf
 : public std::basic_streambuf<char>
{
	static const size_t bufsize;
	int fd;
	char ibuf[512];
	char obuf[512];
public:
	typedef std::char_traits<char> ct;
	fd_streambuf(int f);
	virtual ~fd_streambuf();
	virtual ct::int_type overflow(ct::int_type c);
	virtual ct::int_type underflow();
	virtual int sync();
};


class fd_stream
: public std::iostream
{
	fd_streambuf fdstreambuf;
public:
	fd_stream(int);
};


}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:12 $
 * $Log: fdstream.h,v $
 * Revision 1.2  2003/01/06 12:42:12  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.5  2001/08/06 12:16:20  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.4.4.1  2001/08/03 13:43:57  peter
 * pragma once obsolete...
 *
 * Revision 1.4  2001/07/06 01:47:19  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.3  2001/02/07 13:56:56  peter
 * fix.
 *
 * Revision 1.2  2001/01/05 01:36:53  peter
 * no message
 *
 */

