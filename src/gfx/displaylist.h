#ifndef REAPER_GFX_DISPLAYLIST_H
#define REAPER_GFX_DISPLAYLIST_H

#include "os/gl.h"

namespace reaper {
namespace gfx {
namespace misc {

/// Encapsulates OpenGL display lists
class DisplayList
{
	mutable bool owned;
	GLuint list;
	static int compilation_in_progress;
public:
	static bool compiling() { return compilation_in_progress > 0; }

        inline DisplayList();
        inline DisplayList(const DisplayList &);
	inline DisplayList(bool no_init); ///< defer initialisation until later
	inline ~DisplayList();

	inline void init();
	inline void kill();

	inline void begin() const; ///< Begin list compilation
	inline void end()  const;  ///< End list compilation
        inline void call() const;  ///< Execute compiled list	

	inline DisplayList& operator=(const DisplayList &);
};

////////////////////////////////////////////////////////////////////////////////////
// DISPLAYLIST
////////////////////////////////////////////////////////////////////////////////////

inline DisplayList::DisplayList() : owned(true), list(glGenLists(1)) 
{
}

inline DisplayList::DisplayList(bool no_init) : owned(false), list(0) 
{
}

inline DisplayList::DisplayList(const DisplayList &dl)
{
	*this = dl;
}

inline DisplayList::~DisplayList()
{
	if(owned) {
		glDeleteLists(list,1); 
	}
}

inline void DisplayList::init() 
{
	if(owned) {
		glDeleteLists(list,1);
	} else {
		owned = true;
	}
	list = glGenLists(1);
}

inline void DisplayList::kill()
{
	if(owned) {
		glDeleteLists(list,1);
		owned = false;
	}
}

inline void DisplayList::begin() const
{
	compilation_in_progress++;
	glNewList(list, GL_COMPILE); 
}

inline void DisplayList::end() const
{
	glEndList(); 
	compilation_in_progress--;
}

inline void DisplayList::call() const 
{
	glCallList(list); 
}

inline DisplayList& DisplayList::operator=(const DisplayList &dl)
{
	list = dl.list;
	owned = dl.owned;
	dl.owned = false;
	return *this;
}

}
}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:32 $
 * $Log: displaylist.h,v $
 * Revision 1.5  2002/12/29 17:04:32  pstrand
 * hw/ moved to os/
 *
 * Revision 1.4  2002/01/26 23:27:12  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.3  2002/01/23 09:47:55  macke
 * hmf!
 *
 * Revision 1.1  2002/01/22 22:38:03  macke
 * massive gfx cleanup
 *
 */
