#ifndef REAPER_GFX_GFX_TYPES_H
#define REAPER_GFX_GFX_TYPES_H

#include "main/types_base.h"
#include <algorithm>

namespace reaper {
namespace gfx {

///////////////////////////////////////////////////////////////////
// support structs for reuse of main/types_ops.h
template<typename I>
struct GetR : public I
{
	typename I::value_type* get() { return &r; }
	const typename I::value_type* get() const { return &r; }
};

template<typename I>
struct GetS : public I
{
	typename I::value_type* get() { return &s; }
	const typename I::value_type* get() const { return &s; }
};

template<typename T, int uc>
struct VecRGBA {
	enum { N = 4, upper_clamp = uc };
	typedef T value_type;	
	T r,g,b,a;
};

template<typename T>
struct VecST {
	enum { N = 2 };
	typedef T value_type;	
	T s,t;
};

///////////////////////////////////////////////////////////////////
/// Float color
class Color : public Vec<GetR<VecRGBA<float, 1> > > {
public:
	Color() { }

	Color(const Vec<GetR<VecRGBA<float, 1> > >& v)
	{ r = v.r; g = v.g; b = v.b; a = v.a; }
	Color(float ri, float gi, float bi, float ai = 1.0f)
	{ r = ri;  g = gi;  b = bi;  a = ai; }
};

///////////////////////////////////////////////////////////////////
typedef unsigned char byte;
inline byte ftob(float f) { return static_cast<byte>(f); } 

/// Byte Color
class ByteColor : public Vec<GetR<VecRGBA<byte, 255> > > {
public:
	ByteColor() { }

	ByteColor(const Vec<GetR<VecRGBA<byte, 255> > >& v)
	{ r = v.r; g = v.g; b = v.b; a = v.a; }
	ByteColor(byte ri, byte gi, byte bi, byte ai = 255)
	{ r = ri;  g = gi;  b = bi;  a = ai; }

	ByteColor(const Color &c)
	{ r = ftob(c.r*255); g = ftob(c.g*255); b = ftob(c.b*255); a = ftob(c.b*255); }
};

///////////////////////////////////////////////////////////////////
/// Texture coordinates (s,t)
class TexCoord : public Vec<GetS<VecST<float> > > {
public:
	TexCoord() { }

	TexCoord(const Vec<GetS<VecST<float> > >& v)
	{ s = v.s; t = v.t; }
	TexCoord(float si, float ti)
	{ s = si;  t = ti; }
};

////////////////////////////////////////////////////////////////////////////////
/// Material info
class Material {
public:
        Color diffuse_color;
        Color ambient_color;
        Color emission_color;
        Color specular_color;
        float shininess;
        
	/// Sets material parameters into gl
        void use() const;
};

////////////////////////////////////////////////////////////////////////////////
using std::min;

template <typename Impl>
inline Vec<Impl>& max_clamp(Vec<Impl> &c) {       
	using std::min;

	for(int i=0; i<Impl::N; i++) {
		c[i] = min(c[i],static_cast<typename Impl::value_type>(Impl::upper_clamp));
	}
        return c;
}

using std::max;

template <typename Impl>
inline Vec<Impl>& min_clamp(Vec<Impl> &c) {
	using std::max;

	for(int i=0; i<Impl::N; i++) {
		c[i] = max(c[i],static_cast<typename Impl::value_type>(0));
	}
        return c;
}

template <typename Impl>
inline Vec<Impl>& clamp(Vec<Impl> &c) {
	min_clamp(c);
	max_clamp(c);
	return c;
}

}
}

#endif

/*
* $Author: macke $
* $Date: 2002/02/08 11:27:51 $
* $Log: gfx_types.h,v $
* Revision 1.11  2002/02/08 11:27:51  macke
* rendering reorg and silhouette shadows!
*
* Revision 1.10  2002/01/31 05:35:04  peter
* *** empty log message ***
*
* Revision 1.9  2002/01/26 23:27:12  macke
* Matrox fix, plus some misc stuff..
*
* Revision 1.8  2002/01/24 11:55:26  peter
* trams :(
*
* Revision 1.7  2002/01/23 04:42:46  peter
* mackes gfx-fix in igen, och fix av dylik... ;)
*
* Revision 1.6  2002/01/22 23:44:00  peter
* reversed last two revs
*
* Revision 1.4  2002/01/17 04:56:56  peter
* gcc-2 is happier this way..
*
* Revision 1.3  2002/01/16 00:25:25  picon
* terrängbelysning
*
* Revision 1.2  2001/11/21 00:27:51  picon
* missile smoke / ship tail / misc
*
* Revision 1.1  2001/07/30 23:43:15  macke
* Häpp, då var det kört.
*
*/
