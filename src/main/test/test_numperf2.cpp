

#include "os/compat.h"

#include "os/time.h"

#include "main/types.h"
#include "main/types_ops.h"

// #include "main/mathlib.h"

//#include "ext/mtl/mtl.h"

#include <stdio.h>

#pragma auto_inline(on)
#pragma inline_recursion(on)
#pragma inline_depth(255)


class VectorS
{
public:
	float x, y, z;
	VectorS() : x(0), y(0), z(0) { }
	VectorS(float a, float b, float c)
	 : x(a), y(b), z(c)
	{ }
	float& operator[](int i) { return (&x)[i]; }
	const float& operator[](int i) const { return (&x)[i]; }
	VectorS& operator+=(const VectorS& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
};

template<int i>
struct VecImpl { };

template<> struct VecImpl<0> { };
template<> struct VecImpl<1> { float x; };
template<> struct VecImpl<2> { float x, y; };
template<> struct VecImpl<3> { float x, y, z; };
template<> struct VecImpl<4> { float x, y, z, w; };


template<int n>
class Vect : public VecImpl<n> {
public:	
	Vect() { }
	Vect(float xi, float yi) { x = xi, y = yi;  }
	Vect(float xi, float yi, float zi) { x = xi, y = yi, z = zi;  }
	Vect(float xi, float yi, float zi, float wi) { x = xi, y = yi, z = zi, w = wi;  }

	const float& operator[] (const int i) const { return (&x)[i]; }
	      float& operator[] (const int i)       { return (&x)[i]; }

	Vect& operator+=(const Vect& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
};

class VectorD : public Vect<3>
{
public:
        VectorD() { }
        VectorD(const float X, const float Y, const float Z) : Vect<3>(X,Y,Z) { }
};

namespace mat {
template<class Vector>
class Matrix
{
	Vector v[3];
public:
	Matrix() { }
	Matrix(const Vector& r0, const Vector& r1, const Vector& r2)
	{
		col(0, r0);
		col(1, r1);
		col(2, r2);
	}
	const Vector& row(int i) const { return v[i]; }
	      Vector& row(int i)       { return v[i]; }
	const Vector col(int i) const { return Vector(v[0][i], v[1][i], v[2][i]); }
	const Vector& operator[](int i) const { return row(i); }
	      Vector& operator[](int i)       { return row(i); }

	void col(int i, const Vector& V)
	{
		v[0][i] = V[0];
		v[1][i] = V[1];
		v[2][i] = V[2];
	}

	Matrix& operator+=(const Matrix& m)
	{
		v[0] += m.v[0];
		v[1] += m.v[1];
		v[2] += m.v[2];
		return *this;
	}
};
}

typedef reaper::Matrix MatR;

MatR mult_rp(const MatR& m1, const MatR& m2)
{
	return m1 * m2;
}


template<class Vector>
float dot2(const Vector& v1, const Vector& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

template<class M, class V>
V operator*(const M& m, const V& v)
{
	return V(dot2(m.row(0), v), dot2(m.row(1), v), dot2(m.row(2), v));
}

typedef mat::Matrix<VectorS> MatS;

MatS mult1(const MatS& m1, const MatS& m2)
{
	MatS res;
	res.col(0, VectorS(dot2(m1.row(0), m2.col(0)),
			  dot2(m1.row(1), m2.col(0)),
			  dot2(m1.row(2), m2.col(0))));
	res.col(1, VectorS(dot2(m1.row(0), m2.col(1)),
			  dot2(m1.row(1), m2.col(1)),
			  dot2(m1.row(2), m2.col(1))));
	res.col(2, VectorS(dot2(m1.row(0), m2.col(2)),
			  dot2(m1.row(1), m2.col(2)),
			  dot2(m1.row(2), m2.col(2))));
        return res;
}

MatS mult1_2(const MatS& m1, const MatS& m2)
{
	MatS res;
	res.col(0, VectorS(m1 * m2.col(0)));
	res.col(1, VectorS(m1 * m2.col(1)));
	res.col(2, VectorS(m1 * m2.col(2)));
        return res;
}

typedef mat::Matrix<VectorD> MatD;

MatD mult1_5(const MatD& m1, const MatD& m2)
{
	MatD res;
	res.col(0, VectorD(dot2(m1.row(0), m2.col(0)),
			  dot2(m1.row(1), m2.col(0)),
			  dot2(m1.row(2), m2.col(0))));
	res.col(1, VectorD(dot2(m1.row(0), m2.col(1)),
			  dot2(m1.row(1), m2.col(1)),
			  dot2(m1.row(2), m2.col(1))));
	res.col(2, VectorD(dot2(m1.row(0), m2.col(2)),
			  dot2(m1.row(1), m2.col(2)),
			  dot2(m1.row(2), m2.col(2))));
        return res;
}

MatD mult1_22(const MatD& m1, const MatD& m2)
{
	MatD res;
	res.col(0, VectorD(m1 * m2.col(0)));
	res.col(1, VectorD(m1 * m2.col(1)));
	res.col(2, VectorD(m1 * m2.col(2)));
        return res;
}


MatS mult2(const MatS& m1, const MatS& m2)
{
	MatS retVal;
	for (int x=0;x<3;++x) { 
		retVal[0][x] = m1[0][0]*m2[0][x] + m1[0][1]*m2[1][x] + m1[0][2]*m2[2][x];
		retVal[1][x] = m1[1][0]*m2[0][x] + m1[1][1]*m2[1][x] + m1[1][2]*m2[2][x];
		retVal[2][x] = m1[2][0]*m2[0][x] + m1[2][1]*m2[1][x] + m1[2][2]*m2[2][x];
	}
	return retVal;
}

MatS mult2_5(const MatS& m1, const MatS& m2)
{
	MatS retVal;
	for (int x=0;x<3;++x) { 
		retVal[0][x] = m1[0][0]*m2[0][x] + m1[0][1]*m2[1][x] + m1[0][2]*m2[2][x];
		retVal[1][x] = m1[1][0]*m2[0][x] + m1[1][1]*m2[1][x] + m1[1][2]*m2[2][x];
		retVal[2][x] = m1[2][0]*m2[0][x] + m1[2][1]*m2[1][x] + m1[2][2]*m2[2][x];
	}
	return retVal;
}

MatS mult3(const MatS& m1, const MatS& m2)
{
	MatS retVal;
	retVal[0][0] = m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0] + m1[0][2]*m2[2][0];
	retVal[1][0] = m1[1][0]*m2[0][0] + m1[1][1]*m2[1][0] + m1[1][2]*m2[2][0];
	retVal[2][0] = m1[2][0]*m2[0][0] + m1[2][1]*m2[1][0] + m1[2][2]*m2[2][0];
                                                                                
	retVal[0][1] = m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1] + m1[0][2]*m2[2][1];
	retVal[1][1] = m1[1][0]*m2[0][1] + m1[1][1]*m2[1][1] + m1[1][2]*m2[2][1];
	retVal[2][1] = m1[2][0]*m2[0][1] + m1[2][1]*m2[1][1] + m1[2][2]*m2[2][1];
                                                                                
	retVal[0][2] = m1[0][0]*m2[0][2] + m1[0][1]*m2[1][2] + m1[0][2]*m2[2][2];
	retVal[1][2] = m1[1][0]*m2[0][2] + m1[1][1]*m2[1][2] + m1[1][2]*m2[2][2];
	retVal[2][2] = m1[2][0]*m2[0][2] + m1[2][1]*m2[1][2] + m1[2][2]*m2[2][2];
		
	return retVal;
}



namespace reaper2 {

class Vector;
class Point;

struct PtTag { typedef Point RealType; };
struct VecTag { typedef Vector RealType; };

template<class T>
class Vec {
public:	
	float x, y, z;
	typedef float value_type;
	Vec() { }
	Vec(float xi, float yi, float zi) : x(xi), y(yi), z(zi) { }
	Vec(const Vec<T>& v) : x(v.x), y(v.y), z(v.z) { }
	explicit Vec(const float* v) : x(v[0]), y(v[1]), z(v[2]) { }

	Vec& operator=(const Vec<T>& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	const float* get() const { return &x; }
	      float* get()       { return &x; }

	const float& operator[] (const int i) const { return (&x)[i]; }
	      float& operator[] (const int i)       { return (&x)[i]; }

//	operator const typename T::RealType& () const { return *reinterpret_cast<const typename T::RealType*>(this); }
//	operator const typename T::RealType() const { return typename T::RealType(x, y, z); }
};

class Vector : public Vec<VecTag>
{
public:
        Vector() { }
        Vector(const float X, const float Y, const float Z) : Vec<VecTag>(X,Y,Z) { }
	Vector(const Vec<VecTag>& v) : Vec<VecTag>(v) { }
	Vector(const Vector& v) : Vec<VecTag>(v) { }
        explicit Vector(const float *v) : Vec<VecTag>(v) { }
	const Point& to_pt() const { return *reinterpret_cast<const Point*>(this); }
	Vector& operator=(const Vec<VecTag>& p) { x = p.x; y = p.y; z = p.z; return *this;}
};

class Point : public Vec<PtTag> {
public:
        Point() { }
        Point(const float X, const float Y, const float Z) : Vec<PtTag>(X,Y,Z) { }
	Point(const Vec<PtTag>& v) : Vec<PtTag>(v) { }
	Point(const Point& v) : Vec<PtTag>(v) { }
        explicit Point(const float *v) : Vec<PtTag>(v) { }
	const Vector& to_vec() const { return *reinterpret_cast<const Vector*>(this); }
	Point& operator=(const Vec<PtTag>& p) { x = p.x; y = p.y; z = p.z; return *this; }
};

class Matrix {
private:
	Vector v[3];
	Point p;	
public:
	Matrix() : p(0,0,0) {}

	explicit Matrix(const Vector& v0, const Vector& v1, const Vector& v2)
	{
		col(0, v0);
		col(1, v1);
		col(2, v2);
		p = Point(0,0,0);
	}

	// The row operations works only on the 3x3 part of the matrix, not the point!
	const Vector operator[] (int i) const { return row(i); }
	      Vector& operator[](int i) { return row(i); }

	const Vector row(int i) const { return v[i]; }
	      Vector& row(int i) { return v[i]; }
	void row(int i, const Vector& V) { v[i] = V; }

	const Vector col(int i) const { return Vector(v[0][i], v[1][i], v[2][i]); } // return v[i]; }
	void col(int i, const Vector& V) { v[0][i] = V[0]; v[1][i] = V[1]; v[2][i] = V[2]; } // v[i] = V; }

	const Point&  pos() const { return p; }
	      Point&  pos()       { return p; }
};

template<class T>
struct UnaryConv { typedef int res; };

struct UnaryConv<PtTag> { typedef Point res; };
struct UnaryConv<VecTag> { typedef Vector res; };

template<class T1, class T2>
struct Conv { typedef Point res; typedef Point pres; };

struct Conv<PtTag, PtTag> { typedef Vector res; typedef Point pres; };
struct Conv<VecTag, VecTag> { typedef Vector res; typedef Vector pres; };


template<class T1, class T2>
inline float dot(const Vec<T1>& v1, const Vec<T2>& v2) {
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}


template<class T1, class T2>
inline Conv<T1, T2>::pres operator+(const Vec<T1>& v1, const Vec<T2>& v2) { 
        return Conv<T1, T2>::pres(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

inline Vector operator*(const Matrix& m, const Vector& v) {
	return Vector(dot(m.row(0), v), dot(m.row(1), v), dot(m.row(2), v));
}

inline Point operator*(const Matrix& m, const Point& p) {
	return (m * p.to_vec()).to_pt() + m.pos();
}

inline Matrix operator*(const Matrix& m1, const Matrix& m2) {
        Matrix retVal;
	retVal.col(0, m1 * m2.col(0));
	retVal.col(1, m1 * m2.col(1));
	retVal.col(2, m1 * m2.col(2));
	retVal.pos() = m1 * m2.pos();
        return retVal;
}
template<class T1, class T2>
inline Vec<T1>& operator+=(Vec<T1>& v1, const Vec<T2>& v2) { 
        v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
        return v1;
}

inline Matrix& operator+=(Matrix &m1, const Matrix& m2) {
	m1.col(0, m1.col(0) + m2.col(0));
	m1.col(1, m1.col(1) + m2.col(1));
	m1.col(2, m1.col(2) + m2.col(2));
	m1.pos() += m2.pos();
	return m1;
}


}
namespace reaper3 {

class Vector;

struct VecTag { typedef Vector RealType; };

template<class T>
class Vec {
public:	
	float x, y, z;
	typedef float value_type;
	Vec() { }
	Vec(float xi, float yi, float zi) : x(xi), y(yi), z(zi) { }
	Vec(const Vec<T>& v) : x(v.x), y(v.y), z(v.z) { }
	explicit Vec(const float* v) : x(v[0]), y(v[1]), z(v[2]) { }

	Vec& operator=(const Vec<T>& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	const float* get() const { return &x; }
	      float* get()       { return &x; }

	const float& operator[] (const int i) const { return (&x)[i]; }
	      float& operator[] (const int i)       { return (&x)[i]; }

//	operator const typename T::RealType& () const { return *reinterpret_cast<const typename T::RealType*>(this); }
//	operator const typename T::RealType() const { return typename T::RealType(x, y, z); }
};

class Vector : public Vec<VecTag>
{
public:
        Vector() { }
        Vector(const float X, const float Y, const float Z) : Vec<VecTag>(X,Y,Z) { }
	Vector(const Vec<VecTag>& v) : Vec<VecTag>(v) { }
	Vector(const Vector& v) : Vec<VecTag>(v) { }
        explicit Vector(const float *v) : Vec<VecTag>(v) { }
	Vector& operator=(const Vec<VecTag>& p) { x = p.x; y = p.y; z = p.z; return *this;}
};


class Matrix {
private:
	Vector v[3];
public:
	Matrix() { }

	explicit Matrix(const Vector& v0, const Vector& v1, const Vector& v2)
	{
		col(0, v0);
		col(1, v1);
		col(2, v2);
	}

	// The row operations works only on the 3x3 part of the matrix, not the point!
	const Vector operator[] (int i) const { return row(i); }
	      Vector& operator[](int i) { return row(i); }

	const Vector row(int i) const { return v[i]; }
	      Vector& row(int i) { return v[i]; }
	void row(int i, const Vector& V) { v[i] = V; }

	const Vector col(int i) const { return Vector(v[0][i], v[1][i], v[2][i]); } // return v[i]; }
	void col(int i, const Vector& V) { v[0][i] = V[0]; v[1][i] = V[1]; v[2][i] = V[2]; } // v[i] = V; }

};

template<class T>
struct UnaryConv { typedef int res; };

struct UnaryConv<VecTag> { typedef Vector res; };

template<class T1, class T2>
struct Conv { typedef Vector res; typedef Vector pres; };

struct Conv<VecTag, VecTag> { typedef Vector res; typedef Vector pres; };

template<class T1, class T2>
inline float dot(const Vec<T1>& v1, const Vec<T2>& v2) {
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}


template<class T1, class T2>
inline Conv<T1, T2>::pres operator+(const Vec<T1>& v1, const Vec<T2>& v2) { 
        return Conv<T1, T2>::pres(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

inline Vector operator*(const Matrix& m, const Vector& v) {
	return Vector(dot(m.row(0), v), dot(m.row(1), v), dot(m.row(2), v));
}

inline Matrix operator*(const Matrix& m1, const Matrix& m2) {
        Matrix retVal;
	retVal.col(0, m1 * m2.col(0));
	retVal.col(1, m1 * m2.col(1));
	retVal.col(2, m1 * m2.col(2));
        return retVal;
}
template<class T1, class T2>
inline Vec<T1>& operator+=(Vec<T1>& v1, const Vec<T2>& v2) { 
        v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
        return v1;
}

inline Matrix& operator+=(Matrix &m1, const Matrix& m2) {
	m1.col(0, m1.col(0) + m2.col(0));
	m1.col(1, m1.col(1) + m2.col(1));
	m1.col(2, m1.col(2) + m2.col(2));
	return m1;
}


}


namespace reaper4 {

class Vector;

struct VecTag { typedef Vector RealType; };

template<class T>
class Vec {
public:	
	float x, y, z;
	typedef float value_type;
	Vec() { }
	Vec(float xi, float yi, float zi) : x(xi), y(yi), z(zi) { }
	explicit Vec(const float* v) : x(v[0]), y(v[1]), z(v[2]) { }

	const float& operator[] (const int i) const { return (&x)[i]; }
	      float& operator[] (const int i)       { return (&x)[i]; }

};

class Vector : public Vec<VecTag>
{
public:
        Vector() { }
        Vector(const float X, const float Y, const float Z) : Vec<VecTag>(X,Y,Z) { }
        explicit Vector(const float *v) : Vec<VecTag>(v) { }
};


class Matrix {
private:
	Vector v[3];
public:
	Matrix() { }

	explicit Matrix(const Vector& v0, const Vector& v1, const Vector& v2)
	{
		col(0, v0);
		col(1, v1);
		col(2, v2);
	}

	// The row operations works only on the 3x3 part of the matrix, not the point!
	const Vector operator[] (int i) const { return row(i); }
	      Vector& operator[](int i) { return row(i); }

	const Vector row(int i) const { return v[i]; }
	      Vector& row(int i) { return v[i]; }
	void row(int i, const Vector& V) { v[i] = V; }

	const Vector col(int i) const { return Vector(v[0][i], v[1][i], v[2][i]); } // return v[i]; }
	void col(int i, const Vector& V) { v[0][i] = V[0]; v[1][i] = V[1]; v[2][i] = V[2]; } // v[i] = V; }

};

template<class T>
struct UnaryConv { typedef int res; };

struct UnaryConv<VecTag> { typedef Vector res; };

template<class T1, class T2>
struct Conv { typedef Vector res; typedef Vector pres; };

struct Conv<VecTag, VecTag> { typedef Vector res; typedef Vector pres; };

template<class T1, class T2>
inline float dot(const Vec<T1>& v1, const Vec<T2>& v2) {
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}


template<class T1, class T2>
inline Conv<T1, T2>::pres operator+(const Vec<T1>& v1, const Vec<T2>& v2) { 
        return Conv<T1, T2>::pres(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

inline Vector operator*(const Matrix& m, const Vector& v) {
	return Vector(dot(m.row(0), v), dot(m.row(1), v), dot(m.row(2), v));
}

inline Matrix operator*(const Matrix& m1, const Matrix& m2) {
        Matrix retVal;
	retVal.col(0, m1 * m2.col(0));
	retVal.col(1, m1 * m2.col(1));
	retVal.col(2, m1 * m2.col(2));
        return retVal;
}
template<class T1, class T2>
inline Vec<T1>& operator+=(Vec<T1>& v1, const Vec<T2>& v2) { 
        v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
        return v1;
}

inline Matrix& operator+=(Matrix &m1, const Matrix& m2) {
	m1.col(0, m1.col(0) + m2.col(0));
	m1.col(1, m1.col(1) + m2.col(1));
	m1.col(2, m1.col(2) + m2.col(2));
	return m1;
}


}

namespace reaper5 {

class Vector;

struct VecTag { typedef Vector RealType; };

template<class T>
class Vec {
public:	
	float x, y, z;
	typedef float value_type;
	Vec() { }
	Vec(float xi, float yi, float zi) : x(xi), y(yi), z(zi) { }
	explicit Vec(const float* v) : x(v[0]), y(v[1]), z(v[2]) { }

	const float& operator[] (const int i) const { return (&x)[i]; }
	      float& operator[] (const int i)       { return (&x)[i]; }

};

class Vector : public Vec<VecTag>
{
public:
        Vector() { }
        Vector(const float X, const float Y, const float Z) : Vec<VecTag>(X,Y,Z) { }
        explicit Vector(const float *v) : Vec<VecTag>(v) { }
};


class Matrix {
private:
	Vector v[3];
public:
	Matrix() { }

	explicit Matrix(const Vector& v0, const Vector& v1, const Vector& v2)
	{
		col(0, v0);
		col(1, v1);
		col(2, v2);
	}

	// The row operations works only on the 3x3 part of the matrix, not the point!
	const Vector operator[] (int i) const { return row(i); }
	      Vector& operator[](int i) { return row(i); }

	const Vector row(int i) const { return v[i]; }
	      Vector& row(int i) { return v[i]; }
	void row(int i, const Vector& V) { v[i] = V; }

	const Vector col(int i) const { return Vector(v[0][i], v[1][i], v[2][i]); } // return v[i]; }
	void col(int i, const Vector& V) { v[0][i] = V[0]; v[1][i] = V[1]; v[2][i] = V[2]; } // v[i] = V; }

};

template<class T1, class T2>
inline float dot(const Vec<T1>& v1, const Vec<T2>& v2) {
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}


inline Vector operator+(const Vector& v1, const Vector& v2) { 
        return Vector(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

inline Vector operator*(const Matrix& m, const Vector& v) {
	return Vector(dot(m.row(0), v), dot(m.row(1), v), dot(m.row(2), v));
}

inline Matrix operator*(const Matrix& m1, const Matrix& m2) {
        Matrix retVal;
	retVal.col(0, m1 * m2.col(0));
	retVal.col(1, m1 * m2.col(1));
	retVal.col(2, m1 * m2.col(2));
        return retVal;
}

inline Vector& operator+=(Vector& v1, const Vector& v2) { 
        v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
        return v1;
}

inline Matrix& operator+=(Matrix &m1, const Matrix& m2) {
	m1.col(0, m1.col(0) + m2.col(0));
	m1.col(1, m1.col(1) + m2.col(1));
	m1.col(2, m1.col(2) + m2.col(2));
	return m1;
}


}

namespace reaper6 {

class Vector;

struct VecTag { typedef Vector RealType; };

template<class T>
class Vec {
public:	
	float x, y, z;
	typedef float value_type;
	Vec() { }
	Vec(float xi, float yi, float zi) : x(xi), y(yi), z(zi) { }
	explicit Vec(const float* v) : x(v[0]), y(v[1]), z(v[2]) { }

	const float& operator[] (const int i) const { return (&x)[i]; }
	      float& operator[] (const int i)       { return (&x)[i]; }


	Vector& operator+=(const Vector& v2) { 
		x += v2.x;
		y += v2.y;
		z += v2.z;
		return *this;
	}

};

class Vector : public Vec<VecTag>
{
public:
        Vector() { }
        Vector(const float X, const float Y, const float Z) : Vec<VecTag>(X,Y,Z) { }
        explicit Vector(const float *v) : Vec<VecTag>(v) { }
};
inline float dot(const Vector& v1, const Vector& v2) {
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}


inline Vector operator+(const Vector& v1, const Vector& v2) { 
        return Vector(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}


class Matrix {
private:
	Vector v[3];
public:
	Matrix() { }

	explicit Matrix(const Vector& v0, const Vector& v1, const Vector& v2)
	{
		col(0, v0);
		col(1, v1);
		col(2, v2);
	}

	// The row operations works only on the 3x3 part of the matrix, not the point!
	const Vector operator[] (int i) const { return v[i]; }
	      Vector& operator[](int i) { return v[i]; }

	const Vector row(int i) const { return v[i]; }
	      Vector& row(int i) { return v[i]; }
	void row(int i, const Vector& V) { v[i] = V; }

	const Vector col(int i) const { return Vector(v[0][i], v[1][i], v[2][i]); } // return v[i]; }
	void col(int i, const Vector& V) { v[0][i] = V[0]; v[1][i] = V[1]; v[2][i] = V[2]; } // v[i] = V; }

	Matrix& operator+=(const Matrix& m2) {
		Matrix mt;
		mt.col(0, col(0) + m2.col(0));
		mt.col(1, col(1) + m2.col(1));
		mt.col(2, col(2) + m2.col(2));
		*this = mt;
		return *this;
	}
};

inline Vector operator*(const Matrix& m, const Vector& v) {
	return Vector(dot(m.row(0), v), dot(m.row(1), v), dot(m.row(2), v));
}


inline Matrix operator*(const Matrix& m1, const Matrix& m2) {
        Matrix retVal;
	retVal.col(0, m1 * m2.col(0));
	retVal.col(1, m1 * m2.col(1));
	retVal.col(2, m1 * m2.col(2));
        return retVal;
}



}

namespace reaper7 {

class Vector;

struct VecTag { typedef Vector RealType; };

template<class T>
class Vec {
public:	
	float x, y, z;
	typedef float value_type;
	Vec() { }
	Vec(float xi, float yi, float zi) : x(xi), y(yi), z(zi) { }
	explicit Vec(const float* v) : x(v[0]), y(v[1]), z(v[2]) { }

	const float& operator[] (const int i) const { return (&x)[i]; }
	      float& operator[] (const int i)       { return (&x)[i]; }


	Vec& operator+=(const Vec& v2) { 
		x += v2.x;
		y += v2.y;
		z += v2.z;
		return *this;
	}

};

class Vector : public Vec<VecTag>
{
public:
        Vector() { }
        Vector(const float X, const float Y, const float Z) : Vec<VecTag>(X,Y,Z) { }
        explicit Vector(const float *v) : Vec<VecTag>(v) { }
};
inline float dot(const Vector& v1, const Vector& v2) {
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}


inline Vector operator+(const Vector& v1, const Vector& v2) { 
	Vector r(v1);
	r += v2;
	return r;
}


class Matrix {
private:
	Vector v[3];
public:
	Matrix() { }

	explicit Matrix(const Vector& v0, const Vector& v1, const Vector& v2)
	{
		col(0, v0);
		col(1, v1);
		col(2, v2);
	}

	// The row operations works only on the 3x3 part of the matrix, not the point!
	const Vector operator[] (int i) const { return v[i]; }
	      Vector& operator[](int i) { return v[i]; }

	const Vector row(int i) const { return v[i]; }
	      Vector& row(int i) { return v[i]; }
	void row(int i, const Vector& V) { v[i] = V; }

	const Vector col(int i) const { return Vector(v[0][i], v[1][i], v[2][i]); } // return v[i]; }
	void col(int i, const Vector& V) { v[0][i] = V[0]; v[1][i] = V[1]; v[2][i] = V[2]; } // v[i] = V; }

	Matrix& operator+=(const Matrix& m2) {
		Matrix mt;
		mt.col(0, col(0) + m2.col(0));
		mt.col(1, col(1) + m2.col(1));
		mt.col(2, col(2) + m2.col(2));
		*this = mt;
		return *this;
	}
};

inline Vector operator*(const Matrix& m, const Vector& v) {
	return Vector(dot(m.row(0), v), dot(m.row(1), v), dot(m.row(2), v));
}



inline Matrix operator*=(Matrix& m1, const Matrix& m2) {
        Matrix r;
	r.col(0, m1 * m2.col(0));
	r.col(1, m1 * m2.col(1));
	r.col(2, m1 * m2.col(2));
	m1 = r;
        return r;
}




}


namespace reaper8 {

struct VecImpl {
	float x, y, z;
	enum { N = 3 };
	const float* get() const { return &x; }
	float* get() { return &x; }

};

template<class Impl>
class Vec : public Impl {
public:	
	typedef float value_type;
	Vec() { }

	const float& operator[] (const int i) const { return (&x)[i]; }
	      float& operator[] (const int i)       { return (&x)[i]; }
};

class Vector : public Vec<VecImpl>
{
public:
	Vector() { }
	Vector(const Vec<VecImpl>& v) {
		x = v.x; y = v.y; z = v.z; 
	}
	Vector(float X, float Y, float Z)
	{
		x = X; y = Y; z = Z;
	}
};

template<int N> struct ops { };

template<> struct ops<3>
{
	template<class Impl>
	static void add(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] += b[0];
		a[1] += b[1];
		a[2] += b[2];
	}


	template<class Impl>
	static float dot(const Vec<Impl>& v1, const Vec<Impl>& v2) {
		return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
	}
};



template<class Impl>
Vec<Impl>& operator+=(Vec<Impl>& v1, const Vec<Impl>& v2) {
	ops<Impl::N>::add(v1, v2);
	return v1;
}

template<class Impl>
inline float dot(const Vec<Impl>& v1, const Vec<Impl>& v2) {
	return ops<Impl::N>::dot(v1, v2);
}

template<class Impl>
inline Vec<Impl> operator+(const Vec<Impl>& v1, const Vec<Impl>& v2) { 
	Vector r(v1);
	ops<Impl::N>::add(r, v2);
	return r;
}


class Matrix {
private:
	Vector v[3];
public:
	Matrix() { }

	explicit Matrix(const Vector& v0, const Vector& v1, const Vector& v2)
	{
		col(0, v0);
		col(1, v1);
		col(2, v2);
	}

	const Vector operator[] (int i) const { return row(i); }

	const Vector row(int i) const { return v[i]; }
	      Vector& row(int i) { return v[i]; }
	void row(int i, const Vector& V) { v[i] = V; }

	const Vector col(int i) const { return Vector(v[0][i], v[1][i], v[2][i]); } // return v[i]; }
	void col(int i, const Vector& V) { v[0][i] = V[0]; v[1][i] = V[1]; v[2][i] = V[2]; } // v[i] = V; }

	Matrix& operator+=(const Matrix& m) {
		Matrix mt;
		v[0] += m.v[0];
		v[1] += m.v[1];
		v[2] += m.v[2];
		return *this;
	}
};

inline Vector operator*(const Matrix& m, const Vector& v) {
	return Vector(dot(m.row(0), v), dot(m.row(1), v), dot(m.row(2), v));
}



inline Matrix operator*=(Matrix& m1, const Matrix& m2) {
        Matrix r;
	r.col(0, m1 * m2.col(0));
	r.col(1, m1 * m2.col(1));
	r.col(2, m1 * m2.col(2));
	m1 = r;
        return r;
}




}

namespace math1 {


template<class VecImpl>
class Vect : public VecImpl {
public:	
	Vect() { }

	const float& operator[] (const int i) const { return get()[i]; }
	      float& operator[] (const int i)       { return get()[i]; }

};

struct VecImpl {
	float x, y, z;
	float *get() { return &x; }
	const float *get() const { return &x; }
	enum { N = 3 };
};

class VectorD : public Vect<VecImpl>
{
public:
        VectorD() { }
        VectorD(const float X, const float Y, const float Z)
	{ x = X; y = Y; z = Z; }
};

template<int N> struct ops { };

template<>
struct ops<3> {
template<class Impl>
static void add(Vect<Impl>& a, const Vect<Impl>& b)
{
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

	template<class Impl>
	static float dot(const Vect<Impl>& v1, const Vect<Impl>& v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}
};

template<class Impl>
Vect<Impl>& operator+=(Vect<Impl>& a, const Vect<Impl>& b)
{
	ops<Impl::N>::add(a, b);
	return a;
}


template<class Impl>
float dot2(const Vect<Impl>& v1, const Vect<Impl>& v2)
{
	return ops<Impl::N>::dot(v1, v2);
}

class MatD
{
	VectorD v[3];
public:
	MatD() { }
	MatD(const VectorD& r0, const VectorD& r1, const VectorD& r2)
	{
		col(0, r0);
		col(1, r1);
		col(2, r2);
	}
	const VectorD& row(int i) const { return v[i]; }
	      VectorD& row(int i)       { return v[i]; }
	const VectorD col(int i) const { return VectorD(v[0][i], v[1][i], v[2][i]); }
	const VectorD& operator[](int i) const { return row(i); }
	      VectorD& operator[](int i)       { return row(i); }

	void col(int i, const VectorD& V)
	{
		v[0][i] = V[0];
		v[1][i] = V[1];
		v[2][i] = V[2];
	}

	MatD& operator+=(const MatD& m)
	{
		v[0] += m.v[0];
		v[1] += m.v[1];
		v[2] += m.v[2];
		return *this;
	}
};


template<class M, class V>
V operator*(const M& m, const V& v)
{
	return V(dot2(m.row(0), v), dot2(m.row(1), v), dot2(m.row(2), v));
}


MatD mult(const MatD& m1, const MatD& m2)
{
	MatD res;
	res.col(0, VectorD(m1 * m2.col(0)));
	res.col(1, VectorD(m1 * m2.col(1)));
	res.col(2, VectorD(m1 * m2.col(2)));
        return res;
}

inline MatD operator*=(MatD& m1, const MatD& m2)
{
        MatD r;
	r.col(0, m1 * m2.col(0));
	r.col(1, m1 * m2.col(1));
	r.col(2, m1 * m2.col(2));
	m1 = r;
        return r;
}

}

template<class Mat>
float sum(const Mat& m)
{
	return m[0][0]+m[0][1]+m[0][2]
	     + m[1][0]+m[1][1]+m[1][2]
	     + m[2][0]+m[2][1]+m[2][2];
}

reaper2::Matrix mult_rp2(const reaper2::Matrix& m1, const reaper2::Matrix& m2)
{
	return m1 * m2;
}

reaper3::Matrix mult_rp3(const reaper3::Matrix& m1, const reaper3::Matrix& m2)
{
	return m1 * m2;
}

reaper4::Matrix mult_rp4(const reaper4::Matrix& m1, const reaper4::Matrix& m2)
{
	return m1 * m2;
}

reaper5::Matrix mult_rp5(const reaper5::Matrix& m1, const reaper5::Matrix& m2)
{
	return m1 * m2;
}

reaper6::Matrix mult_rp6(const reaper6::Matrix& m1, const reaper6::Matrix& m2)
{
	return m1 * m2;
}

reaper7::Matrix mult_rp7(const reaper7::Matrix& m1, const reaper7::Matrix& m2)
{
	reaper7::Matrix r(m1);
	r *= m2;
	return r;
}

reaper8::Matrix mult_rp8(const reaper8::Matrix& m1, const reaper8::Matrix& m2)
{
	reaper8::Matrix r(m1);
	r *= m2;
	return r;
}

math1::MatD mult_mt1(const math1::MatD& m1, const math1::MatD& m2)
{
	math1::MatD r(m1);
	r *= m2;
	return r;
}

/*
typedef mtl::matrix< float, mtl::rectangle<3,3>,
mtl::dense<mtl::internal>, mtl::row_major>::type MTLMatrix;
typedef mtl::dense1D<float, 3> MTLVector;

MTLMatrix mult_mtl(const MTLMatrix& m1, const MTLMatrix& m2)
{
	MTLMatrix r(3,3);
	mtl::mult(m1, m2, r);
	return r;
}

float sum(const MTLMatrix& m)
{
	return m(0,0) + m(0,1) + m(0,2)
	     + m(1,0) + m(1,1) + m(1,2)
	     + m(2,0) + m(2,1) + m(2,2);
}
*/

template<class M, class V>
void init(M& m1, M& m2, M& m, V)
{
	m1 = M( V(0, 5, 6),
		V(1, 4, 7),
		V(2, 3, 8));
	m2 = M( V(4, 3, 8),
		V(5, 2, 9),
		V(6, 1, 0));
	m = M (V(0,0,0), V(0,0,0), V(0,0,0));
}

/*
void init(MTLMatrix& m1, MTLMatrix& m2, MTLMatrix& m, MTLVector)
{
	float mi1[] = { 0, 5, 6, 1, 4, 7, 2, 3, 8 };
	float mi2[] = { 4, 3, 8, 5, 2, 9, 6, 1, 0 };
	m1 = MTLMatrix(3,3);
	m2 = MTLMatrix(3,3);
	m = MTLMatrix(3,3);
	std::copy(mi1, mi1+9, m1.data());
	std::copy(mi2, mi2+9, m2.data());
	std::fill_n(m.data(), 9, 0.0f);
}
*/

template<class M>
void add(M& m, const M& a)
{
	m += a;
}

/*
void add(MTLMatrix& m, const MTLMatrix& a)
{
	mtl::add(m, a);
}
*/

template<class Matrix, class Vector, class S, class T>
void test(int n, S s, T fn)
{
	Matrix m1;
	Matrix m2;
	Matrix m;
	init(m1, m2, m, Vector());

	reaper::os::time::Profiler perf(s);
	for (int i = 0; i < n; i++) {
		perf.start();
		for (int j = 0; j < 10000; ++j) {
			add(m, fn(m1, m2));
			add(m, fn(m1, m2));
			add(m, fn(m1, m2));
			add(m, fn(m1, m2));
			add(m, fn(m1, m2));
		}
		perf.stop();
	}
	perf.int_report();
	printf("res: %f\n", sum(m));
}

void go(int n)
{
	test<mat::Matrix<VectorS>, VectorS >	(n, "prim    ", mult3);
	test<mat::Matrix<VectorS>, VectorS >	(n, "primloop", mult2);
//	test<mat::Matrix<VectorS>, VectorS >	(n, "primloop", mult2_5);
//	test<mat::Matrix<VectorS>, VectorS >	(n, "vecdot  ", mult1);
	test<mat::Matrix<VectorS>, VectorS >	(n, "vecmul  ", mult1_2);
//	test<mat::Matrix<VectorD>, VectorD >	(n, "vecdotD ", mult1_5);
	test<mat::Matrix<VectorD>, VectorD >	(n, "vecmulD ", mult1_22);
	test<reaper::Matrix, reaper::Vector>	(n, "reaper  ", mult_rp);
//	test<reaper2::Matrix, reaper2::Vector>	(n, "reaper2 ", mult_rp2);
//	test<reaper3::Matrix, reaper3::Vector>	(n, "reaper3 ", mult_rp3);
//	test<reaper4::Matrix, reaper4::Vector>	(n, "reaper4 ", mult_rp4);
//	test<reaper5::Matrix, reaper5::Vector>	(n, "reaper5 ", mult_rp5);
//	test<reaper6::Matrix, reaper6::Vector>	(n, "reaper6 ", mult_rp6);
	test<reaper7::Matrix, reaper7::Vector>	(n, "reaper7 ", mult_rp7);
	test<reaper8::Matrix, reaper8::Vector>	(n, "reaper8 ", mult_rp8);
//	test<reaper::math::Matrix, reaper::math::Vector> (n, "mathlib ", reaper::math::mult_ml);
//	test<MTLMatrix, MTLVector>		(n, "mtl     ", mult_mtl);
	test<math1::MatD, math1::VectorD>   (n, "math1   ", math1::mult);
	test<math1::MatD, math1::VectorD>   (n, "math2   ", mult_mt1);
}

int main()
{
	go(1);
	go(3);
	return 0;
}
