#ifndef REAPER_GFX_STATIC_GEOM_H
#define REAPER_GFX_STATIC_GEOM_H

#include "misc/smartptr.h"
#include "gfx/displaylist.h"

namespace reaper {
namespace gfx {
namespace misc {

/// Static geometry encapsulator (uses displaylist or VAR, depending on availability)
// FIXME: renders triangles only, also assumes 4 color components
class StaticGeometry
{
	class VARGeometry;

	DisplayList list;	
	reaper::misc::SmartPtr<VARGeometry> vargeom;

	static void render(const VARGeometry *vg);
	static void setup(const float *v, const float *c,
		          const float *n, const float *t,
			  size_t stride);
	static void restore(const float *c, const float *n, const float *t);

public:
	// if indices are given, glDrawElements is used, otherwise glDrawArrays

	StaticGeometry();
	StaticGeometry(const Point *verts, const Color *colors,
		       const Vector *normals, const TexCoord *tcoords, 
		       bool separate_arrays, size_t num_verts,
		       const int* indices = 0, size_t num_indices = 0);
	StaticGeometry(const StaticGeometry&);
	~StaticGeometry();

	StaticGeometry& operator=(const StaticGeometry &);

	void init(const Point *verts, const Color *colors,
	          const Vector *normals, const TexCoord *tcoords, 
	          bool separate_arrays, size_t num_verts,
	          const int* indices = 0, size_t num_indices = 0);

	inline void render() const;
};

////////////////////////////////////////////////////////////////////////////////////

inline void StaticGeometry::render() const
{
	if(vargeom == 0) {
		list.call();
	} else {
		render(vargeom.get());
	}	
}

}
}
}

#endif

/*
 * $Author: macke $
 * $Date: 2002/02/13 21:57:18 $
 * $Log: static_geom.h,v $
 * Revision 1.4  2002/02/13 21:57:18  macke
 * misc effects modifications
 *
 * Revision 1.3  2002/01/23 09:47:56  macke
 * hmf!
 *
 * Revision 1.1  2002/01/22 22:38:07  macke
 * massive gfx cleanup
 *
 */
