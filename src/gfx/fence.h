#include "os/gl.h"

namespace reaper {
namespace gfx {
namespace misc {

/// Encapsulates the GL_NV_FENCE extension (check before usage!!)
class Fence
{
	GLuint fence;
public:
	inline Fence();
	inline ~Fence();

	inline void set()  const;  ///< inserts fence into gl command stream
	inline void wait() const;  ///< waits until fence has been reached
	inline bool test() const;  ///< tests if fence has been reached
};

////////////////////////////////////////////////////////////////////////////////////
// FENCE
////////////////////////////////////////////////////////////////////////////////////

inline Fence::Fence()
{
	glGenFencesNV(1,&fence);
}
inline Fence::~Fence()
{
	glDeleteFencesNV(1,&fence);
}

inline void Fence::set() const
{
	glSetFenceNV(fence,GL_ALL_COMPLETED_NV);
}

inline void Fence::wait() const
{
	glFinishFenceNV(fence);
}

inline bool Fence::test() const
{
	return glTestFenceNV(fence) == GL_TRUE;
}

}
}
}

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:32 $
 * $Log: fence.h,v $
 * Revision 1.4  2002/12/29 17:04:32  pstrand
 * hw/ moved to os/
 *
 * Revision 1.3  2002/01/23 09:47:55  macke
 * hmf!
 *
 * Revision 1.1  2002/01/22 22:38:03  macke
 * massive gfx cleanup
 *
 */
