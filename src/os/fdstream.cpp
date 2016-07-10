/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:12 $
 * $Log: fdstream.cpp,v $
 * Revision 1.2  2003/01/06 12:42:12  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.8  2002/04/08 13:17:15  pstrand
 * *** empty log message ***
 *
 * Revision 1.7  2002/04/06 22:25:32  pstrand
 * win32fixar
 *
 * Revision 1.6  2002/04/06 20:16:25  pstrand
 * *** empty log message ***
 *
 * Revision 1.5  2001/11/27 13:11:00  peter
 * hata visual c+
 *
 * Revision 1.4  2001/04/16 22:07:16  peter
 * streambuf init
 *
 * Revision 1.3  2001/01/05 01:36:52  peter
 * no message
 *
 */


#include <fcntl.h>
#include <stdio.h>

#include "os/compat.h"

#if defined(WIN32) && !defined(CYGWIN)
#include <io.h>
#else
#include <unistd.h>
#endif

#include "os/fdstream.h"

namespace reaper
{
namespace os
{


/* XXX: use traits for read/write functions... */


const size_t fd_streambuf::bufsize = 512;

fd_streambuf::fd_streambuf(int f) : fd(f)
{
	setg(0, 0, 0);
	setp(obuf, obuf + sizeof(obuf));
}

typedef std::char_traits<char> ct;

ct::int_type fd_streambuf::overflow(ct::int_type c)
{
	sync();
	obuf[0] = c;
	pbump(1);
	return ct::not_eof(c);
}

int fd_streambuf::sync()
{
	int a = 0, n, l = pptr() - pbase();
	while (l > 0 && ((n = write(fd, pbase() + a, pptr() - pbase() + a)) != 0)) {
		a += n;
		l -= n;
	}
	setp(obuf, obuf + sizeof(obuf));
	return 0;
}

ct::int_type fd_streambuf::underflow()
{
	int n = read(fd, ibuf, sizeof(ibuf));
	if (n == -1) {
		return ct::eof();
	} else {
		setg(ibuf, ibuf, ibuf + n);
		return ibuf[0];
	}
}

fd_streambuf::~fd_streambuf()
{
	sync();
	close(fd);
}


fd_stream::fd_stream(int fd)
 : std::iostream(0)
 , fdstreambuf(fd)
{
	init(&fdstreambuf);
}
}
}


