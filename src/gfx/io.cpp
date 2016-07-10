/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:33 $
 * $Log: io.cpp,v $
 * Revision 1.6  2002/12/29 17:04:33  pstrand
 * hw/ moved to os/
 *
 * Revision 1.5  2002/04/11 01:03:33  pstrand
 * explicit res_stream exception...
 *
 * Revision 1.4  2001/06/09 01:58:48  macke
 * Grafikmotor reorg
 *
 * Revision 1.3  2001/05/10 11:40:12  macke
 * häpp
 *
 */

#include "os/compat.h"
#include "os/debug.h"

#include <vector>
#include <iterator>

#include "main/types.h"
#include "main/types_io.h"
#include "res/res.h"
#include "gfx/io.h"

#include "res/urr.h"

#include <typeinfo>

using namespace std;

namespace reaper {
namespace gfx {
namespace lowlevel {

namespace { debug::DebugOutput derr("gfx::io"); }


void read_mesh(const std::string name, MeshData& md)
{
	std::auto_ptr< res::urr::Urr > lvl(res::urr::mk("meshes/"+name));
	std::auto_ptr< res::urr::Reader > rd(lvl->reader());

	using std::back_inserter;

	res::urr::get_arr_of_arr<Point>(rd, "vertices", back_inserter(md.points));
	res::urr::get_arr_of_arr<IdxTriangle>(rd, "polygons", back_inserter(md.triangles));
	res::urr::get_arr_of_arr<Vector>(rd, "normals", back_inserter(md.normals));
	res::urr::get_arr_of_arr<TexCoord>(rd, "tex_coords", back_inserter(md.texcoords));
	res::urr::get_arr_of_arr<Color>(rd, "colors", back_inserter(md.colors));
}


}
}
}
