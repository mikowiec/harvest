#ifndef REAPER_HW_BITS_H
#define REAPER_HW_BITS_H

namespace reaper
{
namespace os
{
namespace lowlevel
{

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef signed int int32;
typedef signed short int16;
typedef signed char int8;


// convert to little endian

#ifdef IS_BIG_ENDIAN

inline uint32 to_le(uint32& i)
{
	i = ((i & 0xff) << 24) | ((i & 0xff00) << 8)
	  | ((i & 0xff0000) >> 8) | ((i & 0xff000000) >> 24);
	return i;
}

inline uint16 to_le(uint16& i)
{
	i = ((i & 0xff) << 8) | (i >> 8);
	return i;
}

inline int32 to_le(int32& i)
{
	i = ((i & 0xff) << 24) | ((i & 0xff00) << 8)
	  | ((i & 0xff0000) >> 8) | ((i & 0xff000000) >> 24);
	return i;
}

inline int16 to_le(int16& i)
{
	i = ((i & 0xff) << 8) | (i >> 8);
	return i;
}

#else

inline uint32 to_le(uint32 i) { return i; }
inline uint16 to_le(uint16 i) { return i; }
inline int32 to_le(int32 i) { return i; }
inline int16 to_le(int16 i) { return i; }

#endif


}
}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:12 $
 * $Log: bits.h,v $
 * Revision 1.2  2003/01/06 12:42:12  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.4  2001/08/06 12:16:18  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.3.4.1  2001/08/03 13:43:56  peter
 * pragma once obsolete...
 *
 * Revision 1.3  2001/07/06 01:47:17  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.2  2001/03/21 10:27:52  peter
 * *** empty log message ***
 *
 * Revision 1.1  2001/03/15 00:31:31  peter
 * endianness...
 *
 */

