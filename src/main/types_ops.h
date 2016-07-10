#ifndef REAPER_MAIN_TYPES_OPS_H
#define REAPER_MAIN_TYPES_OPS_H

#include "main/types.h"
#include <cmath>

namespace reaper {             
namespace {
	const float PI = 3.1415926535897932384626433832795;
}


// written in this style to allow for easy substitution with more efficient versions
inline float SINE_FUNCTION(const float x) {
	return static_cast<float>(sin(x));
}
inline float COSINE_FUNCTION(const float x) { 
        return static_cast<float>(cos(x));
}
inline float SQRT_FUNCTION (const float x) { 
        return static_cast<float>(sqrt(x));
}
        
inline bool solvequadratic (float a, float b, float c, float* x1, float* x2) {
        // If a == 0, solve a linear equation
        if (a == 0) {
                if (b == 0) return false;
                *x1 = c / b;
                *x2 = *x1;
                return true;
        } else {
                float det = b * b - 4 * a * c;
                if (det < 0) return false;
                det = SQRT_FUNCTION(det) / (2 * a);
                float prefix = -b / (2*a);
                *x1 = prefix + det;
                *x2 = prefix - det;
                return true;
        }
}


inline Matrix3x3::Matrix3x3(const Vector& c0, const Vector& c1, const Vector& c2)
{
	col(0, c0);
	col(1, c1);
	col(2, c2);
}

inline Quaternion::Quaternion(const Matrix &m) {
	w = .5f * sqrt(m[0][0] + m[1][1] + m[2][2] + 1.0f);
	const float tmp = 1.0f / (4.0f * w);
	x = (m[1][2] - m[2][1]) * tmp;
	y = (m[2][0] - m[0][1]) * tmp;
	z = (m[0][1] - m[1][0]) * tmp;
}

template<int N>
struct ops { };

template<>
struct ops<2>
{
	template<class Impl>
	static bool eq(const Vec<Impl>& a, const Vec<Impl>& b)
	{
		return a[0] == b[0] && a[1] == b[1];
	}

	template<class Impl>
	static void sub(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] -= b[0];
		a[1] -= b[1];
	}

	template<class Impl>
	static void add(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] += b[0];
		a[1] += b[1];
	}

	template<class Impl>
	static void add(Vec<Impl>& a, typename Impl::value_type k)
	{
		a[0] += k;
		a[1] += k;
	}

	template<class Impl>
	static void mult(Vec<Impl>& a, typename Impl::value_type k)
	{
		a[0] *= k;
		a[1] *= k;
	}

	template<class Impl>
	static void mult(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] *= b[0];
		a[1] *= b[1];
	}

	template<class Impl>
	static typename Impl::value_type scalar(const Vec<Impl>& a, const Vec<Impl>& b)
	{
		return a[0]*b[0] + a[1]*b[1];
	}

	template<class Impl>
	static Vec<Impl> neg(const Vec<Impl>& a)
	{
		return Vec<Impl>(-a[0], -a[1]);
	}
};

template<>
struct ops<3>
{
	template<class Impl>
	static bool eq(const Vec<Impl>& a, const Vec<Impl>& b)
	{
		return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
	}

	template<class Impl>
	static void sub(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] -= b[0];
		a[1] -= b[1];
		a[2] -= b[2];
	}

	template<class Impl>
	static void add(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] += b[0];
		a[1] += b[1];
		a[2] += b[2];
	}

	template<class Impl>
	static void add(Vec<Impl>& a, typename Impl::value_type k)
	{
		a[0] += k;
		a[1] += k;
		a[2] += k;
	}

	template<class Impl>
	static void mult(Vec<Impl>& a, typename Impl::value_type k)
	{
		a[0] *= k;
		a[1] *= k;
		a[2] *= k;
	}

	template<class Impl>
	static void mult(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] *= b[0];
		a[1] *= b[1];
		a[2] *= b[2];
	}

	template<class Impl>
	static typename Impl::value_type scalar(const Vec<Impl>& a, const Vec<Impl>& b)
	{
		return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
	}

	template<class Impl>
	static Vec<Impl> neg(const Vec<Impl>& a)
	{
		return Vec<Impl>(-a[0], -a[1], -a[2]);
	}
};

template<>
struct ops<4>
{
	template<class Impl>
	static bool eq(const Vec<Impl>& a, const Vec<Impl>& b)
	{
		return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3];
	}

	template<class Impl>
	static void sub(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] -= b[0];
		a[1] -= b[1];
		a[2] -= b[2];
		a[3] -= b[3];
		}

	template<class Impl>
	static void add(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] += b[0];
		a[1] += b[1];
		a[2] += b[2];
		a[3] += b[3];
	}

	template<class Impl>
	static void add(Vec<Impl>& a, typename Impl::value_type k)
	{
		a[0] += k;
		a[1] += k;
		a[2] += k;
		a[3] += k;
	}

	template<class Impl>
	static void mult(Vec<Impl>& a, typename Impl::value_type k)
	{
		a[0] *= k;
		a[1] *= k;
		a[2] *= k;
		a[3] *= k;
	}

	template<class Impl>
	static void mult(Vec<Impl>& a, const Vec<Impl>& b)
	{
		a[0] *= b[0];
		a[1] *= b[1];
		a[2] *= b[2];
		a[3] *= b[3];
	}

	template<class Impl>
	static typename Impl::value_type scalar(const Vec<Impl>& v1, const Vec<Impl>& v2)
	{
		return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] + v1[3]*v2[3];
	}

	template<class Impl>
	static Vec<Impl> neg(const Vec<Impl>& a)
	{
		return Vec<Impl>(-a[0], -a[1], -a[2], -a[3]);
	}
};

template<class Impl>
inline bool operator==(const Vec<Impl>& a, const Vec<Impl>& b)
{
	return ops<Impl::N>::eq(a, b);
}

template<class Impl>
inline bool operator!=(const Vec<Impl>& a, const Vec<Impl>& b)
{ 
        return !(a == b);
}


template<class T1, class T2>
struct Conv { typedef Point res; typedef Point pres; };

struct Conv<Point, Point> { typedef Vector res; typedef Point pres; };
struct Conv<Vector, Vector> { typedef Vector res; typedef Vector pres; };


template<class Impl>
inline Vec<Impl> operator-(const Vec<Impl>& v) {
	return ops<Impl::N>::neg(v);
}


template<class Impl>
inline Vec<Impl> operator*(const Vec<Impl>& a, typename Impl::value_type k) { 
	Vec<Impl> r(a);
	ops<Impl::N>::mult(r, k);
	return r;
}

template<class Impl>
inline Vec<Impl> operator*(typename Impl::value_type k, const Vec<Impl>& v) { 
	return v*k;
}

template<class Impl>
inline Vec<Impl> operator*(const Vec<Impl>& a, const Vec<Impl>& b) { 
	Vec<Impl> r(a);
	ops<Impl::N>::mult(r, b);
	return r;
}

template<class Impl>
inline Vec<Impl>& operator*=(Vec<Impl>& v, typename Impl::value_type k) {
	ops<Impl::N>::mult(v, k);
	return v;
}


template<class Impl>
inline Vec<Impl> operator/(const Vec<Impl>& v, typename Impl::value_type k) { 
	return v * (static_cast<typename Impl::value_type>(1) / k);
}


template<class Impl>
inline Vec<Impl>& operator/=(Vec<Impl>& v, typename Impl::value_type k) { 
        return v *= (static_cast<typename Impl::value_type>(1) / k);
}

template<class Impl>
inline Vec<Impl>& operator+=(Vec<Impl>& v, typename Impl::value_type k) {
	ops<Impl::N>::add(v, k);
        return v;
}

template<class Impl>
inline Vec<Impl>& operator-=(Vec<Impl>& v, typename Impl::value_type k) {
	ops<Impl::N>::add(v, -k);
        return v;
}

template<class Impl>
inline Vec<Impl> operator+(typename Impl::value_type k, const Vec<Impl>& v) {
	Vec<Impl> r(v);
	r += k;
	return r;
}

template<class Impl>
inline Vec<Impl> operator-(typename Impl::value_type k, const Vec<Impl>& v) {
	Vec<Impl> r(v);
	r -= k;
	return r;
}
	
template<class Impl>
inline Vec<Impl> operator+(const Vec<Impl>& v, typename Impl::value_type k) {
	Vec<Impl> r(v);
	r += k;
	return r;
}

template<class Impl>
inline Vec<Impl> operator-(const Vec<Impl>& v, typename Impl::value_type k) {
	Vec<Impl> r(v);
	r -= k;
	return r;
}
	

template<class Impl>
inline typename Impl::value_type length_sq(const Vec<Impl>& v) { 
        return ops<Impl::N>::scalar(v,v);
}

template<class Impl>
inline typename Impl::value_type length(const Vec<Impl>& v) { 
        return SQRT_FUNCTION(length_sq(v));
}



template<class Impl>
inline void normd(Vec<Impl>& v) {  
	const typename Impl::value_type len = length(v);
	if (len == static_cast<typename Impl::value_type>(0) ||
	    len == static_cast<typename Impl::value_type>(1)) {
		return;
	} else {
		v = (v / len);
	}
}

template<class Impl>
inline Vec<Impl> norm(const Vec<Impl>& v) {  
	Vec<Impl> r = v;
	normd(r);
	return r;
}


template<class Impl>
inline Vec<Impl> operator+(const Vec<Impl>& v1, const Vec<Impl>& v2) { 
	Vec<Impl> r(v1);
	r += v2;
	return r;
}

template<class Impl>
inline Vec<Impl>& operator+=(Vec<Impl>& v1, const Vec<Impl>& v2) {
	ops<Impl::N>::add(v1, v2);
        return v1;
}


template<class Impl>
inline Vec<Impl> operator-(const Vec<Impl>& v1, const Vec<Impl>& v2) { 
	Vec<Impl> r(v1);
	r -= v2;
	return r;
}


template<class Impl>
inline Vec<Impl>& operator-=(Vec<Impl>& v1, const Vec<Impl>& v2) { 
	ops<Impl::N>::sub(v1, v2);
	return v1;
}


template<class Impl>
inline typename Impl::value_type dot(const Vec<Impl>& v1, const Vec<Impl>& v2) {
        return ops<Impl::N>::scalar(v1, v2);
}


inline Vector cross(const Vec<GetX<Vec3> >& v1, const Vec<GetX<Vec3> >& v2)
{
        return Vector(  v1.y * v2.z - v1.z * v2.y,
			v2.x * v1.z - v2.z * v1.x,
		       	v1.x * v2.y - v1.y * v2.x);
}

/*
template<class T>
inline Matrix3x3 mul(const Vec<T>& v, const Vec<T>& u_trans){

	return Matrix3x3(Vector(v.x*u_trans.x, v.x*u_trans.y, v.x*u_trans.z),
		         Vector(v.y*u_trans.x, v.y*u_trans.y, v.y*u_trans.z),
		         Vector(v.z*u_trans.x, v.z*u_trans.y, v.z*u_trans.z));
}
*/

inline Vector operator*(const Matrix3x3& m, const Vector& v) {
	return Vector(dot(m[0], v), dot(m[1], v), dot(m[2], v));
}

inline bool operator== (const Matrix3x3& m1, const Matrix3x3& m2) {
	return m1[0] == m2[0]
	    && m1[1] == m2[1]
	    && m1[2] == m2[2];
}

inline Matrix3x3 operator*(const Matrix3x3& m1, const Matrix3x3& m2) {
        Matrix3x3 retVal;
	retVal.col(0, m1 * m2.col(0));
	retVal.col(1, m1 * m2.col(1));
	retVal.col(2, m1 * m2.col(2));
	return retVal;
}

inline Matrix3x3& operator*=(Matrix3x3& m1, const Matrix3x3& m2) {
	return m1 = m1*m2;
}

inline Matrix3x3& operator+=(Matrix3x3 &m1, const Matrix3x3& m2) {
	m1.col(0, m1.col(0) + m2.col(0));
	m1.col(1, m1.col(1) + m2.col(1));
	m1.col(2, m1.col(2) + m2.col(2));
	return m1;
}

inline Matrix3x3& operator-=(Matrix3x3 &m1, const Matrix3x3& m2) {
	m1.col(0, m1.col(0) - m2.col(0));
	m1.col(1, m1.col(1) - m2.col(1));
	m1.col(2, m1.col(2) - m2.col(2));
	return m1;
}

inline Matrix3x3& operator*=(Matrix3x3 &m1, const float n) {
	m1.col(0, m1.col(0) * n);
	m1.col(1, m1.col(1) * n);
	m1.col(2, m1.col(2) * n);
	return m1;
}

inline Matrix3x3& operator*(Matrix3x3 &m1, const float n) {
	m1.col(0, m1.col(0) * n);
	m1.col(1, m1.col(1) * n);
	m1.col(2, m1.col(2) * n);
	return m1;
}

inline Matrix3x3 operator+(const Matrix3x3& m1, const Matrix3x3& m2){
	Matrix3x3 retval(m1);
	retval+=m2;
	return retval;
}

inline Matrix3x3 operator-(const Matrix3x3& m1, const Matrix3x3& m2){
	Matrix3x3 retval(m1);
	retval-=m2;
	return retval;
}

inline Matrix3x3 operator*(const Matrix3x3& mi, float n) {
	Matrix3x3 m(mi);
	m *= n;
	return m;
}

inline Matrix3x3 operator*(float n, const Matrix3x3& mi) {
	Matrix3x3 m(mi);
	m *= n;
	return m;
}



template<class T>
inline Matrix mul(const Vec<T>& v, const Vec<T>& u_trans){

	return Matrix(Vector(v.x*u_trans.x, v.x*u_trans.y, v.x*u_trans.z),
		      Vector(v.y*u_trans.x, v.y*u_trans.y, v.y*u_trans.z),
		      Vector(v.z*u_trans.x, v.z*u_trans.y, v.z*u_trans.z));
}

inline Vector operator*(const Matrix& m, const Vector& v) {
	return Vector(dot(m.row(0), v), dot(m.row(1), v), dot(m.row(2), v));
}

inline Point operator*(const Matrix& m, const Point& p) {
	return (m * p.to_vec()).to_pt() + m.pos();
}

inline bool operator== (const Matrix& m1, const Matrix& m2) {
	if(m1.pos()  != m2.pos())  return false;
	if(m1.col(0) != m2.col(0)) return false;
	if(m1.col(1) != m2.col(1)) return false;
	if(m1.col(2) != m2.col(2)) return false;
        return true;
}

inline Matrix operator*(const Matrix& m1, const Matrix& m2) {
        Matrix retVal;
	retVal.col(0, m1 * m2.col(0));
	retVal.col(1, m1 * m2.col(1));
	retVal.col(2, m1 * m2.col(2));
	retVal.pos() = m1 * m2.pos();
        return retVal;
}

inline Matrix& operator*=(Matrix& m1, const Matrix& m2) {
	return m1 = m1*m2;
}

inline Matrix& operator+=(Matrix &m1, const Matrix& m2) {
	m1.col(0, m1.col(0) + m2.col(0));
	m1.col(1, m1.col(1) + m2.col(1));
	m1.col(2, m1.col(2) + m2.col(2));
	m1.pos() += m2.pos();
	return m1;
}

inline Matrix& operator-=(Matrix &m1, const Matrix& m2) {
	m1.col(0, m1.col(0) - m2.col(0));
	m1.col(1, m1.col(1) - m2.col(1));
	m1.col(2, m1.col(2) - m2.col(2));
	m1.pos() -= m2.pos();
	return m1;
}

inline Matrix& operator*=(Matrix &m1, const float n) {
	m1.col(0, m1.col(0) * n);
	m1.col(1, m1.col(1) * n);
	m1.col(2, m1.col(2) * n);
	m1.pos() *= n;
	return m1;
}


inline Matrix operator+(const Matrix& m1, const Matrix& m2){
	Matrix retval(m1);
	retval+=m2;
	return retval;
}

inline Matrix operator-(const Matrix& m1, const Matrix& m2){
	Matrix retval(m1);
	retval-=m2;
	return retval;
}

inline Matrix operator*(const Matrix& mi, float n) {
	Matrix m(mi);
	m *= n;
	return m;
}



inline Matrix operator*(float n, const Matrix& mi) {
	Matrix m(mi);
	m *= n;
	return m;
}

/// Inverse that only inverts the 3x3 "rotation" part
inline Matrix fake_inverse3(const Matrix &m) {
	Matrix ret;

	ret.row(0, m.col(0));
	ret.row(1, m.col(1));
	ret.row(2, m.col(2));
	ret.pos() = m.pos();

	return ret;
}

/// Inverse that only inverts the 3x3 "rotation" part but is correct for scaled objects
Matrix inverse3(const Matrix& m);
void orthonormalize3(Matrix &M);


inline
int itrunc(float f)
{
	return static_cast<int>(f);
}
}
#endif

/*
 * $Author: pstrand $
 * $Date: 2002/06/18 12:19:25 $
 * $Log: types_ops.h,v $
 * Revision 1.19  2002/06/18 12:19:25  pstrand
 * upprensning
 *
 * Revision 1.18  2002/04/19 21:29:42  le_picon
 * mult,sub,add added
 *
 * Revision 1.17  2002/04/19 20:10:17  pstrand
 * *** empty log message ***
 *
 * Revision 1.16  2002/04/19 19:36:07  pstrand
 * *** empty log message ***
 *
 * Revision 1.15  2002/02/19 21:59:44  peter
 * Matrix::id
 *
 * Revision 1.14  2002/01/31 05:34:04  peter
 * *** empty log message ***
 *
 * Revision 1.13  2002/01/29 04:09:55  peter
 * matrisfippel
 *
 * Revision 1.12  2002/01/23 04:42:50  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.11  2002/01/22 23:44:06  peter
 * reversed last two revs
 *
 * Revision 1.9  2002/01/09 13:57:36  peter
 * Yet Another Vector Implementation, this time SimpleYetFlexible(tm)  ;)
 *
 * Revision 1.8  2002/01/01 23:26:14  peter
 * no message
 *
 * Revision 1.7  2001/12/08 22:51:53  truls
 * destructive normalization on Vec
 *
 * Revision 1.6  2001/12/05 14:04:12  peter
 * matrix now in (semi)row-major
 *
 * Revision 1.5  2001/12/03 18:26:45  truls
 * vector multipication
 *
 * Revision 1.4  2001/12/02 19:47:45  peter
 * get_row -> get_col
 *
 * Revision 1.3  2001/09/10 21:54:44  peter
 * no message
 *
 * Revision 1.2  2001/08/06 12:16:29  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.1.2.5  2001/08/05 14:01:27  peter
 * objektmeck...
 *
 * Revision 1.1.2.4  2001/08/03 13:44:05  peter
 * pragma once obsolete...
 *
 * Revision 1.1.2.3  2001/08/02 16:29:10  peter
 * objfix...
 *
 * Revision 1.1.2.2  2001/08/01 00:47:27  peter
 * visual fix...
 *
 * Revision 1.1.2.1  2001/07/31 17:34:07  peter
 * testgren...
 *
 * Revision 1.1  2001/07/30 23:43:24  macke
 * Häpp, då var det kört.
 *
 */

