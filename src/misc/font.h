#ifndef REAPER_MISC_FONT_H
#define REAPER_MISC_FONT_H

namespace reaper
{
namespace gfx
{
class Initializer;
namespace font
{

enum Font { Small, Medium, Large };

/**
  Draw a string at the specified location.
  scale_x and scale_y defines the size of a single letter.
 */
void glPutStr(float x, float y, const std::string& s, Font font = Medium,
	      float scale_x = 0.01, float scale_y = 0.02);


}

}
}

#endif


/*
 * $Author: peter $
 * $Date: 2002/01/23 04:42:50 $
 * $Log: font.h,v $
 * Revision 1.11  2002/01/23 04:42:50  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.10  2002/01/22 23:44:06  peter
 * reversed last two revs
 *
 * Revision 1.8  2001/11/26 03:19:32  peter
 * font reinit
 *
 * Revision 1.7  2001/08/06 12:16:30  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.6.4.1  2001/08/03 13:44:06  peter
 * pragma once obsolete...
 *
 * Revision 1.6  2001/07/06 01:47:25  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.5  2001/05/10 11:40:20  macke
 * häpp
 *
 * Revision 1.4  2001/05/04 09:24:24  peter
 * *** empty log message ***
 *
 */

