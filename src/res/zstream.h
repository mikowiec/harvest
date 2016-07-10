#ifndef REAPER_RES_ZSTREAM_H
#define REAPER_RES_ZSTREAM_H

#include <iostream>

namespace reaper
{
namespace res
{

class GZip;

class izstreambuf
 : public std::basic_streambuf<char>
{
	std::streambuf& is;
	GZip* gzip;
	char ibuf[512];
	void init();
	int read_some();
public:
	izstreambuf(std::streambuf&);
	~izstreambuf();

	typedef std::char_traits<char> ct;

	virtual ct::int_type underflow();
};


class izstream
: public std::istream
{
	izstreambuf z_buf;
public:
	izstream(std::istream&);
};


}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2002/04/06 22:25:33 $
 * $Log: zstream.h,v $
 * Revision 1.7  2002/04/06 22:25:33  pstrand
 * win32fixar
 *
 * Revision 1.6  2002/04/06 20:08:25  pstrand
 * *** empty log message ***
 *
 * Revision 1.5  2001/08/06 12:16:43  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.4.4.1  2001/08/03 13:44:15  peter
 * pragma once obsolete...
 *
 * Revision 1.4  2001/07/06 01:47:37  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.3  2001/04/19 01:13:05  peter
 * *** empty log message ***
 *
 * Revision 1.2  2001/04/18 23:55:54  peter
 * zstream... (möjligt det inte funkar helt..)
 *
 * Revision 1.1  2001/04/17 22:54:40  peter
 * zstream
 *
 *
 */


