#ifndef REAPER_GFX_MISC_H
#define REAPER_GFX_MISC_H

#include <string>
#include "main/types.h"

namespace reaper {
namespace world { class Sphere; }
namespace gfx {
class Color;
namespace misc {

/// Draws an analog meter at pt(x,y) with given length and width. 
/// (maxval indicates the value where the meter is at length)
void meter(float x, float y, float length, float width, float maxval, float value);

/// Loads a png texture into data (memory malloced by function), returns number of channels
int load_png(const std::string &file,char *&data,int &w,int &h, bool check_dim = true);
void save_png(const std::string &file,char *data,int w,int h);

/// Draws a sphere at origo with given radius
void sphere(double radius, int slices, int stacks);
void sphere(const world::Sphere &, int slices, int stacks);

/// Billboard drawing (textured quads that always face the camera)
class BillBoard
{
public:	
	// call these everytime the modelview matrix is changed
	// (renderer calls set_vectors() once per frame)
	
	/// Retrieve billboard vectors from gl
	static void set_vectors();              
	/// Set vectors manually 
	static void set_vectors(const Vector &right, const Vector &up);  

	// whatever you do, don't mix the two add calls...

	static void add(float size, const Point &pos);
	static void add(float size, const Point &pos, const Color &c);

	// ... and call the correct render function!

	static void render();       
	static void render_colors();
};


}
}
}
#endif

/*
 * $Author: peter $
 * $Date: 2002/01/23 04:42:48 $
 * $Log: misc.h,v $
 * Revision 1.27  2002/01/23 04:42:48  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.26  2002/01/22 23:44:02  peter
 * reversed last two revs
 *
 * Revision 1.24  2002/01/11 13:41:59  peter
 * d'oh
 *
 * Revision 1.23  2002/01/11 13:18:58  peter
 * d'oh
 *
 * Revision 1.22  2002/01/11 13:00:08  peter
 * auto_ptr -> ptr
 *
 * Revision 1.21  2002/01/10 23:09:08  macke
 * massa bök
 *
 * Revision 1.20  2001/11/20 21:44:09  picon
 * firing shots after restart now works..
 *
 * Revision 1.19  2001/09/10 00:24:54  macke
 * Grafikfix... (skum cpu-läcka i ljussättningen kvarstår)
 *
 * Revision 1.18  2001/08/06 12:16:14  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.17.4.1  2001/08/03 13:43:53  peter
 * pragma once obsolete...
 *
 * Revision 1.17  2001/07/06 01:47:12  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.16  2001/06/07 05:14:18  macke
 * Reaper v0.9
 *
 * Revision 1.15  2001/05/10 11:40:13  macke
 * häpp
 *
 */

