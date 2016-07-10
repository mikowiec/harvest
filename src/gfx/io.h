#ifndef REAPER_GFX_IO_H
#define REAPER_GFX_IO_H

#include <iostream>
#include <typeinfo>
#include <vector>
#include "gfx/exceptions.h"
#include "main/types_io.h"
#include "gfx/gfx_types.h"
#include "misc/parse.h"

namespace reaper {

inline void throw_if_bad(const std::istream &s, const std::string &msg)
{
        if(!s.good())
                throw reaper::gfx::gfx_fatal_error(msg);
}

inline std::istream& skip_label(std::istream& is)
{
	reaper::misc::until(is, ':');
	is.get();
	reaper::misc::skip_spaces(is);
	return is;
}

template<class T, class Out>
inline void read_data(std::istream &is, Out out, const std::string &caller)
{
	is.get();
	if(is.peek() == ']') {
		is.get();
		return;
	} else do {
		T t;
		skip_label(is) >> t;
		*out++ = t;
	} while(is.get() == ',');

	throw_if_bad(is,caller + ": Unable to read " + typeid(T).name());
}

namespace gfx {
inline std::ostream& operator<<(std::ostream& os, const Color& v)  {
	return io_hlp::write<4>(os, v.get(), 1.0f);
}

inline std::istream& operator>>(std::istream& is, Color& v) throw(io_error) {
	return io_hlp::read(is, v.get(), 4, 1.0f);
}

inline std::istream& operator>>(std::istream &is, TexCoord &t) throw(io_error) {
	// Assumes no padding...
	return io_hlp::read(is, &t.s, 2);
}

inline std::ostream& operator<<(std::ostream &os, const TexCoord &t) {
	return os << '[' << t.s << ", " << t.t << ']';
}



namespace lowlevel
{

struct MeshData
{
	std::vector<Point> points;
	std::vector<TexCoord> texcoords;
	std::vector<Vector> normals;
	std::vector<Color> colors;
	std::vector<IdxTriangle> triangles;
};

void read_mesh(const std::string name, MeshData& mesh);

}
}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2002/09/23 19:39:34 $
 * $Log: io.h,v $
 * Revision 1.16  2002/09/23 19:39:34  pstrand
 * *** empty log message ***
 *
 * Revision 1.15  2002/09/23 12:08:43  fizzgig
 * Moved all anonymous namespaces into the reaper-namespace
 *
 * Revision 1.14  2002/06/19 17:21:15  pstrand
 * ''
 *
 * Revision 1.13  2002/06/18 12:18:58  pstrand
 * types_io.h upprensning
 *
 * Revision 1.12  2002/02/18 11:51:49  peter
 * std::
 *
 * Revision 1.11  2001/11/26 10:46:03  peter
 * kompilatorgnäll... ;)
 *
 * Revision 1.10  2001/11/21 00:27:51  picon
 * missile smoke / ship tail / misc
 *
 * Revision 1.9  2001/08/06 12:16:13  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.8.2.1  2001/08/03 13:43:52  peter
 * pragma once obsolete...
 *
 * Revision 1.8  2001/07/30 23:43:16  macke
 * Häpp, då var det kört.
 *
 * Revision 1.7  2001/07/06 01:47:11  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.6  2001/06/09 01:58:48  macke
 * Grafikmotor reorg
 *
 * Revision 1.5  2001/05/18 22:15:59  peter
 * hmm...
 *
 * Revision 1.4  2001/05/10 11:40:12  macke
 * häpp
 *
 */

