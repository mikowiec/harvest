/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:42 $
 * $Log: zstream.cpp,v $
 * Revision 1.11  2002/12/29 17:04:42  pstrand
 * hw/ moved to os/
 *
 * Revision 1.10  2002/05/04 12:24:50  pstrand
 * *** empty log message ***
 *
 * Revision 1.9  2002/04/06 22:25:32  pstrand
 * win32fixar
 *
 * Revision 1.8  2002/04/06 20:08:25  pstrand
 * *** empty log message ***
 *
 * Revision 1.7  2002/01/21 20:57:40  peter
 * no message
 *
 * Revision 1.6  2001/07/27 15:46:45  peter
 * *** empty log message ***
 *
 * Revision 1.5  2001/07/09 13:33:11  peter
 * gcc-3.0 fixar
 *
 * Revision 1.4  2001/04/23 20:39:01  peter
 * *** empty log message ***
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

#include "os/compat.h"
#include "os/debug.h"
#include "res/zstream.h"
#include "misc/iostream_helper.h"

#include "zlib.h"

#include <iostream>

namespace reaper
{
namespace res
{


class GZip
{
	z_stream zstr;
	std::streambuf& is;
	int data_len, cur_pos;
	unsigned char* read_buf;
	bool is_eof;
public:
	GZip(std::streambuf& s);
	int read(unsigned char* ptr, int len);
	bool eof() const;
};


// FIXME.. learn from book, make it good...

void ignore(std::streambuf& is, char stop)
{
	int c;
	while ((c = is.sbumpc()) != stop && c != -1)
	{ }
}


GZip::GZip(std::streambuf& s) : is(s), cur_pos(0), is_eof(false)
{
	char hdr[10];
	is.sgetn(hdr, 10);
	if (hdr[3] & 4) {
		char low = is.sgetc();
		char high = is.sgetc();
		ignore(is, low + high * 256);
	}
	if (hdr[3] & 8)
		ignore(is, '\0');
	if (hdr[3] & 16)
		ignore(is, '\0');
	if (hdr[3] & 2) {
		char junk[2];
		is.sgetn(junk, 2);
	}
	read_buf = new unsigned char[4096];
	zstr.next_in = 0;
	zstr.avail_in = 0;
	zstr.zalloc = 0;
	zstr.zfree = 0;
	zstr.opaque = 0;
	inflateInit2(&zstr, -15);  // -15, magic number.. see zlib docs
}

int GZip::read(unsigned char* ptr, int len)
{
	if (is_eof)
		return -1;
	zstr.next_out = ptr;
	zstr.avail_out = len;
	while (zstr.avail_out > 0) {
		if (zstr.avail_in == 0) {
			is.sgetn(reinterpret_cast<char*>(read_buf), 4096);
			zstr.next_in = read_buf;
			zstr.avail_in = 4096;
		}
		int res = inflate(&zstr, Z_NO_FLUSH);
		if (res == Z_STREAM_END) {
			is_eof = true;
			break;
		}
	}
	return len - zstr.avail_out;
}


typedef std::char_traits<char> ct;

izstreambuf::izstreambuf(std::streambuf& s)
 : is(s), gzip(0)
{
	setg(ibuf, ibuf, ibuf);
	init();
}

izstreambuf::~izstreambuf()
{
	delete gzip;
}

void izstreambuf::init()
{
	gzip = new GZip(is);
}

int izstreambuf::read_some()
{
	int n = 512;
	if ((n = gzip->read((unsigned char*)ibuf, n)) == -1) {
		setg(0,0,0);
		return -1;
	} else {
		setg(ibuf, ibuf, ibuf + n);
		return n;
	}
}

ct::int_type izstreambuf::underflow()
{
	if (gptr() < egptr())
		return ct::to_int_type(*gptr());
	
	if (read_some() < 0)
		return ct::eof();
	else
		return ct::to_int_type(*gptr());
}


typedef std::istream std_istream;

izstream::izstream(std::istream& is)
 : std_istream(0)
 , z_buf(*is.rdbuf())
{
	init(&z_buf);
}


}
}

