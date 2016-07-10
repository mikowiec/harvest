#ifndef REAPER_MAIN_TYPES_H
#define REAPER_MAIN_TYPES_H

#include "main/types_base.h"

namespace reaper {

/** @name Data primitives */
//@{

class Point;

class Vector : public Vec<GetX<Vec3> >
{
public:
	Vector() { }
	Vector(const Vec<GetX<Vec3> >& v)
	{ x = v.x; y = v.y; z = v.z; }
	Vector(float xi, float yi, float zi)
	{ x = xi; y = yi; z = zi; }
	Point to_pt() const;
};

class Point : public Vec<GetX<Vec3> >
{
public:
	Point() { }
	Point(const Vec<GetX<Vec3> >& v)
	{ x = v.x; y = v.y; z = v.z; }
	Point(float xi, float yi, float zi)
	{ x = xi; y = yi; z = zi; }
	Vector to_vec() const { return Vector(x,y,z); };
};

inline Point Vector::to_pt() const { return Point(x,y,z); }

class Vector4 : public Vec<GetX<Vec4> >
{
public:
	Vector4() { }
	Vector4(const Vec<GetX<Vec4> >& v)
	{ x = v.x; y = v.y; z = v.z; w = v.w; }
	Vector4(float xi, float yi, float zi, float wi)
	{ x = xi; y = yi; z = zi; w = wi; }
	Vector4(const Point& p)
	{ x = p.x; y = p.y; z = p.z; w = 1; }
	Vector4(const Vector& p)
	{ x = p.x; y = p.y; z = p.z; w = 0; }

	Point as_point() const { return Point(x,y,z); }
	Vector as_vector() const { return Vector(x,y,z); }
};


class Point2D : public Vec<GetX<Vec2> >
{
public:
	Point2D() { }
	Point2D(float xi, float yi)
	{ x = xi; y = yi; }
	Point2D(const Vec<GetX<Vec2> >& v)
	{ x = v.x; y = v.y; }
};

//@}
/**@name Triangle types */
//@{        

/// %Triangle (v1,v2,v3)
template<class T>
class triangle_base {
public:
	enum { N = 3 };
	typedef T value_type;

        T v1,v2,v3;

        triangle_base() {}
        triangle_base(T V1,T V2,T V3) : v1(V1), v2(V2), v3(V3) {}
        triangle_base(const triangle_base &t) : v1(t.v1), v2(t.v2), v3(t.v3) {}
        explicit triangle_base(const T *t) : v1(t[0]), v2(t[1]), v3(t[2]) {}

        triangle_base& operator=(const triangle_base &t) {
                v1 = t.v1; 
                v2 = t.v2;
                v3 = t.v3;
                return *this;
        }

	const T* get() const { return &x; }
	      T* get() { return &x; }
};       

/// %Triangle with references to vertices
typedef triangle_base<Point&> RefTriangle;
/// %Triangle with pointers to vertices
typedef triangle_base<Point*> PtrTriangle;
/// %Triangle with points
typedef triangle_base<Point> PntTriangle;
/// %Triangle with integer indicies
typedef triangle_base<int> IdxTriangle;

//@}              


class Rotation {
public:
	Vector axis;
	float angle;

	Rotation() {}
	Rotation(float an, const Vector &ax) : axis(ax), angle(an) {}
	Rotation(float an, float x, float y, float z) : axis(x,y,z), angle(an) {}
};


class Quaternion;
class Matrix4;
class Matrix3x3;


void mk_rot_mtx(Matrix3x3&, float a, const Vector& axis);
void mk_rot_mtx(Matrix3x3&, const Rotation& r);
void mk_scale_mtx(Matrix3x3&, float x, float y, float z);
void mtx_from_quat(Matrix3x3&, const Quaternion& q);
void mk_id_mtx(Matrix3x3&);



class Matrix3x3 {
	Vector v[3];
public:
	Matrix3x3() { }
	Matrix3x3(const Vector& c0, const Vector& c1, const Vector& c2);
	Matrix3x3(const Rotation& r) { mk_rot_mtx(*this, r); }
	Matrix3x3(float angle, const Vector& axis) { mk_rot_mtx(*this, angle, axis); } 
        Matrix3x3(float x, float y, float z) { mk_scale_mtx(*this, x,y,z); }
	Matrix3x3(const Quaternion& q) { mtx_from_quat(*this, q); }

	const Vector col(int i) const { return Vector(v[0][i], v[1][i], v[2][i]); }
	void col(int i, const Vector& c) {
		v[0][i] = c[0];
		v[1][i] = c[1];
		v[2][i] = c[2];
	}

	const Vector& row(int i) const        { return v[i]; }
	      Vector& row(int i)              { return v[i]; }
	const Vector& operator[](int i) const { return v[i]; }
	      Vector& operator[](int i)       { return v[i]; }
};


/** 3x4 matrix with 4th row defined as (0,0,0,1)
 *  Indexed by row, columns can only be read (for now)
 *  Should probably be replaced by a proper matrix class 
 *  parameterized over dimensions supporting all meaningful 
 *  operations.
 */
class Matrix {
private:
	Matrix3x3 m;
	Point p;
public:
	Matrix() { }

	explicit Matrix(const Matrix3x3& mtx, Point pt = Point(0,0,0)) : m(mtx), p(pt) { }
	explicit Matrix(const Vector& c0, const Vector& c1, const Vector& c2) : m(c0, c1, c2), p(0,0,0) { }

	explicit Matrix(const Vector& t) : p(t) { mk_id_mtx(m); }
	explicit Matrix(const Point& t)  : p(t) { mk_id_mtx(m); }
	explicit Matrix(const Rotation& r) :m(r), p(0,0,0) { }
	explicit Matrix(const float angle, const Vector &axis) : m(angle, axis), p(0,0,0) { }
	explicit Matrix(const float x, const float y, const float z) : m(x,y,z), p(0,0,0) { }
	explicit Matrix(const Quaternion &q) : m(q), p(0,0,0) { }
	explicit Matrix(const Matrix4 &m);

	// The row operations works only on the 3x3 part of the matrix, not the point!
	// get-funcs
	const Vector& row(int i) const        { return m.row(i); }
	      Vector& row(int i)              { return m.row(i); }
	const Vector& operator[](int i) const { return m[i]; }
	      Vector& operator[](int i)       { return m[i]; }
	const Point& pos() const              { return p; }
	      Point& pos()                    { return p; }
	const Vector col(int i) const         { return Vector(m[0][i], m[1][i], m[2][i]); }

	// Get the rotation part of the matrix
	const Matrix3x3& rot() const { return m; }

	// set-funcs
	void row(int i, const Vector& V) { m[i] = V; }
	void col(int i, const Vector& V) { m[0][i] = V[0]; m[1][i] = V[1]; m[2][i] = V[2]; }
	void rot(const Matrix3x3& mtx)   { m = mtx; }
	void pos(const Point &pos)       { p = pos; }

	static Matrix id();
};



/// Quaternion
class Quaternion {
public:
	typedef float value_type;

        float x,y,z,w;

        Quaternion() { }
        Quaternion(const Quaternion &q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
        Quaternion(float W,float X,float Y,float Z) :  x(X), y(Y), z(Z), w(W) {}
        explicit Quaternion(const float *V) : x(V[1]), y(V[2]), z(V[3]), w(V[0]) {}
	explicit Quaternion(const Matrix &m);

        Quaternion& operator=(const Quaternion &q) {
                x = q.x;
                y = q.y;
                z = q.z;
                w = q.w;
                return *this;
        }

	const float* get() const { return &x; }
	      float* get()       { return &x; }
};

template<typename T>
struct TMap { typedef T Org; };


       
}

#endif

/*
* $Author: pstrand $
* $Date: 2002/06/18 12:19:25 $
* $Log: types.h,v $
* Revision 1.57  2002/06/18 12:19:25  pstrand
* upprensning
*
* Revision 1.56  2002/04/08 21:30:13  pstrand
* *** empty log message ***
*
* Revision 1.55  2002/04/06 20:19:20  pstrand
* *** empty log message ***
*
* Revision 1.54  2002/02/13 21:57:19  macke
* misc effects modifications
*
* Revision 1.53  2002/01/29 04:09:54  peter
* matrisfippel
*
* Revision 1.52  2002/01/17 04:59:01  peter
* test
*
* Revision 1.51  2002/01/10 23:09:11  macke
* massa bök
*
* Revision 1.50  2002/01/09 13:57:35  peter
* Yet Another Vector Implementation, this time SimpleYetFlexible(tm)  ;)
*
* Revision 1.49  2001/12/05 14:04:12  peter
* matrix now in (semi)row-major
*
* Revision 1.48  2001/12/03 18:26:45  truls
* vector multipication
*
* Revision 1.47  2001/12/03 11:33:48  truls
* bracket indexing for vector-type
*
* Revision 1.46  2001/12/02 19:47:44  peter
* get_row -> get_col
*
* Revision 1.45  2001/08/06 12:16:27  peter
* MegaMerge (se strandy_test-grenen för diffar...)
*
* Revision 1.44.2.5  2001/08/05 14:01:27  peter
* objektmeck...
*
* Revision 1.44.2.4  2001/08/03 13:44:04  peter
* pragma once obsolete...
*
* Revision 1.44.2.3  2001/08/02 16:29:10  peter
* objfix...
*
* Revision 1.44.2.2  2001/08/01 00:47:27  peter
* visual fix...
*
* Revision 1.44.2.1  2001/07/31 17:34:06  peter
* testgren...
*
* Revision 1.44  2001/07/30 23:43:23  macke
* Häpp, då var det kört.
*
* Revision 1.43  2001/07/21 23:09:36  peter
* intel fix..
*
* Revision 1.42  2001/07/06 01:47:24  macke
* Refptrfix/headerfilsstäd/objekt-skapande/mm
*
* Revision 1.41  2001/05/10 00:09:15  peter
* *** empty log message ***
*
* Revision 1.40  2001/05/09 15:00:14  peter
* *** empty log message ***
*
* Revision 1.39  2001/05/08 00:14:57  macke
* dynamiska ljus.. buggar lite dock, så de är bortkommenterade..
*
*/

