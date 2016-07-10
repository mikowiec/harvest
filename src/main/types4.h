#ifndef REAPER_MAIN_TYPES4_H
#define REAPER_MAIN_TYPES4_H

#include "main/types.h"

namespace reaper {

/// 4 element vector
#if 0
class Vector4 {                        
public:
	typedef float value_type;

	float x,y,z,w;

	Vector4() {}
	Vector4(const Vector4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
	Vector4(const float a, const float b, const float c, const float d) : x(a), y(b), z(c), w(d) {}
	explicit Vector4(const float* v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}
//	explicit Vector4(const Vector3& v, float W) : x(v.x), y(v.y), z(v.z), w(W) {}
	explicit Vector4(const Point& v) : x(v.x), y(v.y), z(v.z), w(1) {}
	explicit Vector4(const Vector& v) : x(v.x), y(v.y), z(v.z), w(0) {}

	Vector4& operator=(const Vector4 &v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
                return *this;
        }

	const float* get() const { return &x; }
	      float* get() { return &x; }

	Vector as_vector() const { return Vector(get()); }
	Point  as_point()  const { return Point(get()); }
};
#endif

class Quaternion;
class Matrix;

/// 4x4 element matrix (internal storage format in opengl-style)
class Matrix4 {
private:
	float e[16];
public:
	Matrix4() {}

	explicit Matrix4(bool id)                                     { if(id) load_identity(); }
	explicit Matrix4(const float degrees, const Vector4 &axis)    { rotate(degrees, axis); } 
	explicit Matrix4(const float degrees, const Vector &axis)     { rotate(degrees, Vector4(axis)); } 
	explicit Matrix4(const Rotation &r)                           { rotate(r.angle, Vector4(r.axis)); } 
	explicit Matrix4(const Vector4& translation)                  { translate(translation); }
//	explicit Matrix4(const Vector3& translation)                  { translate(Vector4(translation, 0)); }
	explicit Matrix4(const float x, const float y, const float z) { scale(x, y, z); }
	explicit Matrix4(const Quaternion &q);
	explicit Matrix4(const Matrix &);

	Matrix4(const float m11,const float m12,const float m13,const float m14,
	 	const float m21,const float m22,const float m23,const float m24,
		const float m31,const float m32,const float m33,const float m34,
		const float m41,const float m42,const float m43,const float m44) {
		e[0]  = m11;e[1]  = m21;e[2]  = m31;e[3]  = m41;
		e[4]  = m12;e[5]  = m22;e[6]  = m32;e[7]  = m42;
		e[8]  = m13;e[9]  = m23;e[10] = m33;e[11] = m43;
		e[12] = m14;e[13] = m24;e[14] = m34;e[15] = m44;
	}

	// When defining matrices in C arrays, it is easiest to define them with
	// the column increasing fastest.  However, some APIs (OpenGL in particular) do this
	// backwards, hence the "constructor" from C matrices, or from OpenGL matrices.
	// Note that matrices are stored internally in OpenGL format.

	/// Build the matrix from data stored in 'c_array' format (column increasing fastest)
	void c_matrix (const float* initArray) { 
		int i=0;
		for(int y=0;y<4;++y) {
			for(int x=0;x<4;++x) {
				(*this)[x][y] = initArray[i++];
			}
		}
	}

	/// Build the matrix from data stored in 'openGL' format (row increasing fastest)
	void opengl_matrix (const float* initArray) { 
		int i = 0; 
		for(int x=0;x<4;++x) {
			for(int y=0;y<4;++y) {
				(*this)[x][y] = initArray[i++];
			}
		}
	}  

	// m[x][y] is the correct form
	/**@name element access operators */
	///@{
		const float* operator[] (const unsigned int i) const { return e+(i<<2); }
		      float* operator[] (const unsigned int i)       { return e+(i<<2); }
	///@}

	const float* get() const { return e; }
	      float* get()       { return e; }

	Vector4 get_col(const unsigned int i) const { return Vector4(e+(i<<2)); }
	Vector4 get_pos() const { return get_col(3); }

	/**@name Construct various matrices; REPLACES CURRENT CONTENTS OF THE MATRIX! */
	///@{                      
		Matrix4& load_identity() { 
			for (int i=0;i<16;++i) {
				e[i] = 0;
			}

			e[0] = 1; 
			e[5] = 1; 
			e[10] = 1;
			e[15] = 1; 

			return *this;
		}

		/// Angle in degrees, vector must be normalized
		inline Matrix4& rotate (float angle, const Vector4 &axis);
		inline Matrix4& translate(const Vector4& translation);
		inline Matrix4& scale(float x, float y, float z);
		inline Matrix4& basis_change(const Vector4& v, const Vector4& n);
		inline Matrix4& basis_change(const Vector4& u, const Vector4& v, const Vector4& n);
		inline Matrix4& projection_matrix(bool perspective, float l, float r, float t, float b, float n, float f);
	///@}
};

}
#endif

/*
* $Author: pstrand $
* $Date: 2002/06/18 12:19:25 $
* $Log: types4.h,v $
* Revision 1.4  2002/06/18 12:19:25  pstrand
* upprensning
*
* Revision 1.3  2001/12/02 19:47:45  peter
* get_row -> get_col
*
* Revision 1.2  2001/08/06 12:16:27  peter
* MegaMerge (se strandy_test-grenen för diffar...)
*
* Revision 1.1.2.2  2001/08/03 13:44:04  peter
* pragma once obsolete...
*
* Revision 1.1.2.1  2001/07/31 17:34:07  peter
* testgren...
*
* Revision 1.1  2001/07/30 23:43:23  macke
* Häpp, då var det kört.
*
*/
