
#ifndef REAPER_MAIN_TYPES_BASE_H
#define REAPER_MAIN_TYPES_BASE_H

namespace reaper {



template<class Impl>
class Vec : public Impl
{
public:	
	typedef typename Impl::value_type value_type;

	Vec() { }
	Vec(const value_type* p) {
		memcpy(get(), p, sizeof(value_type)*Impl::N);
	}
	Vec(value_type a, value_type b, value_type c) {
		get()[0] = a;
		get()[1] = b;
		get()[2] = c;
	}

	const value_type& operator[] (const int i) const { return get()[i]; }
	      value_type& operator[] (const int i)       { return get()[i]; }

};

struct Vec2 
{
	enum { N = 2 };
	float x, y;
	typedef float value_type;
};

struct Vec3
{
	enum { N = 3 };
	float x, y, z;
	typedef float value_type;
};

struct Vec4
{
	enum { N = 4 };
	float x, y, z, w;
	typedef float value_type;
};

template<class I>
struct GetX : public I
{
	typename I::value_type* get() { return &x; }
	const typename I::value_type* get() const { return &x; }
};


       
}

#endif

/*
* $Author: peter $
* $Date: 2002/01/23 04:42:50 $
* $Log: types_base.h,v $
* Revision 1.5  2002/01/23 04:42:50  peter
* mackes gfx-fix in igen, och fix av dylik... ;)
*
* Revision 1.4  2002/01/22 23:44:05  peter
* reversed last two revs
*
* Revision 1.2  2002/01/17 04:59:01  peter
* test
*
* Revision 1.1  2002/01/09 13:57:36  peter
* Yet Another Vector Implementation, this time SimpleYetFlexible(tm)  ;)
*
*
*/

