#ifndef REAPER_MAIN_TYPES4_IO_H
#define REAPER_MAIN_TYPES4_IO_H

#include "main/types4.h"
#include "main/types_io.h"

namespace reaper {

inline std::istream& operator>>(std::istream &is, Matrix4 &m) throw(io_error)
{
	io_hlp::read(is, m.get(), 16);
	if(is.bad()) 
		throw io_error("Error parsing Matrix4");
	return is;

}

inline std::ostream& operator<<(std::ostream& os, const Matrix4 &m)
{
	return io_hlp::write<16>(os, m.get());
}

}

#endif

