#ifndef REAPER_MAIN_TYPES4_OPS_H
#define REAPER_MAIN_TYPES4_OPS_H

#include "main/types.h"
#include "main/types_ops.h"
#include "main/types4.h"
#include <cmath>

namespace reaper {                    

inline Matrix4::Matrix4(const Quaternion &quat)
{
	/* Code taken from CQuat class made by 
	   Tim C. Schröder (tcs) tcs@thereisnofate.net 
	   http://glvelocity.gamedev.net
	*/

	// Normalize
	float fFactor = quat.x * quat.x +
			  quat.y * quat.y +
			  quat.z * quat.z +
			  quat.w * quat.w;

	float fScaleBy = 1.0f / (float) sqrt(fFactor);

	const float x = quat.x * fScaleBy;
	const float y = quat.y * fScaleBy;
	const float z = quat.z * fScaleBy;
	const float w = quat.w * fScaleBy;	

	// Build matrix
	const float fXX = x * x;
	const float fYY = y * y;
	const float fZZ = z * z;

	(*this)[0][0] = 1.0f - 2.0f * (fYY + fZZ);
	(*this)[1][0] = 2.0f * (x * y + w * z);
	(*this)[2][0] = 2.0f * (x * z - w * y);
	(*this)[3][0] = 0.0f;

	(*this)[0][1] = 2.0f * (x * y - w * z);
	(*this)[1][1] = 1.0f - 2.0f * (fXX + fZZ);
	(*this)[2][1] = 2.0f * (y * z + w * x);
	(*this)[3][1] = 0.0f;

	(*this)[0][2] = 2.0f * (x * z + w * y);
	(*this)[1][2] = 2.0f * (y * z - w * x);
	(*this)[2][2] = 1.0f - 2.0f * (fXX + fYY);
	(*this)[3][2] = 0.0f;

	(*this)[0][3] = 0.0f;
	(*this)[1][3] = 0.0f;
	(*this)[2][3] = 0.0f;
	(*this)[3][3] = 1.0f;
}

/// Returns false if there are 0 solutions
inline bool SolveQuadratic (float a, float b, float c, float* x1, float* x2);

/**@name Vector operators */
//@{
        inline bool operator== (const Vector4&, const Vector4&);
        inline bool operator< (const Vector4&, const Vector4&);

        inline Vector4 operator- (const Vector4&);
        inline Vector4 operator* (const Vector4&, float);
        inline Vector4 operator* (float, const Vector4&);
        inline Vector4& operator*= (Vector4&, float);
        inline Vector4 operator/ (const Vector4&, float);
        inline Vector4& operator/= (Vector4&, float);
        inline Vector4& operator+= (Vector4&, float);

        inline Vector4 operator+(const Vector4&, float);
        inline Vector4 operator-(const Vector4&, float);

        inline Vector4 operator+ (const Vector4&, const Vector4&);
        inline Vector4& operator+= (Vector4&, const Vector4&);
        inline Vector4 operator- (const Vector4&, const Vector4&);
        inline Vector4& operator-= (Vector4&, const Vector4&);
        inline Vector4 operator* (const Matrix4&, const Vector4&);     
//@}
/**@name Vector functions 
         X3 is the 3 element version of a function, X4 is four element
*/
//@{
        inline float length_squared3 (const Vector4&);
        inline float length_squared4 (const Vector4&);
        inline float length3 (const Vector4&);
        inline float length4 (const Vector4&);
        inline Vector4 normalize3 (const Vector4&);
        inline Vector4 normalize4 (const Vector4&);
        inline float dot_product3 (const Vector4&, const Vector4&);
        inline float dot_product4 (const Vector4&, const Vector4&);
        /// Cross product (only works on 3 elements of the vector)
        inline Vector4 cross_product (const Vector4&, const Vector4&);
//@}

/**@name Matrix operators */
//@{
        inline bool operator== (const Matrix4&, const Matrix4&);
        inline bool operator< (const Matrix4&, const Matrix4&);

        inline Matrix4  operator* (const Matrix4&, const Matrix4&);

        inline Matrix4  operator*  (const Matrix4&, float);
        inline Matrix4& operator*= (Matrix4&, float);
        inline Matrix4  operator/  (const Matrix4&, float);
        inline Matrix4& operator/= (Matrix4&, float);

	inline Matrix4& operator*=(Matrix4 &, const Matrix4 &); 
	inline Matrix4& operator+=(Matrix4 &, const Matrix4 &); 

//@}
/**@name Matrix functions */
//@{
        inline Matrix4 transpose (const Matrix4&);
        float        determinant (const Matrix4&);
        Matrix4        adjoint (const Matrix4&);
        inline Matrix4 inverse (const Matrix4&);
        inline Matrix4 inverse3 (const Matrix4&);
//@}

inline bool operator== (const Vector4& v1, const Vector4& v2) 
{ 
        return (v1.x==v2.x&&v1.y==v2.y&&v1.z==v2.z&&v1.w==v2.w);
}

inline bool operator!= (const Vector4& v1, const Vector4& v2) 
{ 
        return !(v1==v2);
}

inline bool operator< (const Vector4& v1, const Vector4& v2)
{
        for (int i=0;i<4;++i) 
                if (v1.get()[i] < v2.get()[i]) 
                        return true;
                else if (v1.get()[i] > v2.get()[i]) 
                        return false;
                return false;
}

inline Vector4 operator- (const Vector4& v) 
{
        return Vector4(-v.x, -v.y, -v.z, -v.w); 
}

inline Vector4 operator* (const Vector4& v, float k)
{ 
        return Vector4(k*v.x, k*v.y, k*v.z, k*v.w);
}

inline Vector4 operator* (float k, const Vector4& v)
{ 
        return v*k;
}

inline Vector4& operator*= (Vector4& v, float k)
{
        v.x*=k;
        v.y*=k;
        v.z*=k;
        v.w*=k; 
        return v;
}

inline Vector4 operator/ (const Vector4& v, float k)
{ 
        return v*(1/k);
}

inline Vector4& operator/= (Vector4& v, float k)
{ 
        return v*=(1/k);
}

inline Vector4& operator+= (Vector4& v, float k)
{
        v.x+=k;
        v.y+=k;
        v.z+=k;
        v.w+=k; 
        return v;
}
	
inline Vector4 operator+(const Vector4& v, float k)
{
	return Vector4(v.x + k, v.y + k, v.z + k, v.w + k);
}
	
inline Vector4 operator-(const Vector4& v, float k)
{
	return Vector4(v.x - k, v.y - k, v.z - k, v.w - k);
}
	
inline float length_squared3 (const Vector4& v)
{ 
        return dot_product3(v,v);
}
inline float length_squared4 (const Vector4& v)
{ 
        return dot_product4(v,v);
}

inline float length3 (const Vector4& v)
{ 
        return SQRT_FUNCTION(length_squared3(v));
}
inline float length4 (const Vector4& v)
{ 
        return SQRT_FUNCTION(length_squared4(v));
}

inline Vector4 normalize3 (const Vector4& v)
{  
	const float len = length3(v);
	if (len == 0) {
		return v;
	} else {
		Vector4 retVal = v / len;
		retVal.w = 1;
		return retVal;
	}
}

inline Vector4 normalize4 (const Vector4& v)
{  
	const float len = length4(v);
	if (len == 0)
		return v;
	else
		return v / len; 
}

inline Vector4 operator+ (const Vector4& v1, const Vector4& v2)
{ 
        return Vector4(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z, v1.w+v2.w);
}

inline Vector4& operator+= (Vector4& v1, const Vector4& v2)
{ 
        v1.x+=v2.x; v1.y+=v2.y; v1.z+=v2.z; v1.w+=v2.w;
        return v1;
}

inline Vector4 operator- (const Vector4& v1, const Vector4& v2)
{ 
        return Vector4(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, v1.w-v2.w);
}

inline Vector4& operator-= (Vector4& v1, const Vector4& v2)
{ 
        v1.x-=v2.x;v1.y-=v2.y;v1.z-=v2.z;v1.w-=v2.w; 
        return v1;
}

inline float dot_product3 (const Vector4& v1, const Vector4& v2)
{
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

inline float dot_product4 (const Vector4& v1, const Vector4& v2)
{ 
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w; 
}

inline Vector4 cross_product (const Vector4& v1, const Vector4& v2) 
{
        return Vector4( v1.y * v2.z - v1.z * v2.y
						   ,v2.x * v1.z - v2.z * v1.x
						   ,v1.x * v2.y - v1.y * v2.x
                       ,1);
}

inline Vector4 operator* (const Matrix4& m, const Vector4& v)
{
        return Vector4( v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0] + v.w*m[3][0],
                        v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1] + v.w*m[3][1],
                        v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2] + v.w*m[3][2],
                        v.x*m[0][3] + v.y*m[1][3] + v.z*m[2][3] + v.w*m[3][3]);
        
}

inline bool operator== (const Matrix4& m1, const Matrix4& m2)
{
        for (int x=0;x<4;++x) for (int y=0;y<4;++y) 
                if (m1[x][y] != m2[x][y]) return false;
                return true;      
}

inline bool operator< (const Matrix4& m1, const Matrix4& m2)
{
        for (int x=0;x<4;++x) for (int y=0;y<4;++y) 
                if (m1[x][y] >= m2[x][y]) return false;
                return true;      
}

inline Matrix4 operator* (const Matrix4& m1, const Matrix4& m2)
{
        Matrix4 retVal;
        for (int x=0;x<4;++x){ 
                retVal[x][0] =  m1[0][0]*m2[x][0] + m1[1][0]*m2[x][1] + m1[2][0]*m2[x][2] + m1[3][0]*m2[x][3];
                retVal[x][1] =  m1[0][1]*m2[x][0] + m1[1][1]*m2[x][1] + m1[2][1]*m2[x][2] + m1[3][1]*m2[x][3];
                retVal[x][2] =  m1[0][2]*m2[x][0] + m1[1][2]*m2[x][1] + m1[2][2]*m2[x][2] + m1[3][2]*m2[x][3];
                retVal[x][3] =  m1[0][3]*m2[x][0] + m1[1][3]*m2[x][1] + m1[2][3]*m2[x][2] + m1[3][3]*m2[x][3];
        }   
        return retVal;
}

inline Matrix4 operator* (const Matrix4& m, float k) 
{
        Matrix4 retVal;
        for (int x=0;x<4;++x){
                retVal[x][0] = m[x][0] * k;
                retVal[x][1] = m[x][1] * k;
                retVal[x][2] = m[x][2] * k;
                retVal[x][3] = m[x][3] * k;
        }
        return retVal;
}

inline Matrix4& operator*= (Matrix4& m, float k) 
{
        for (int x=0;x<4;++x){
                m[x][0] *= k;
                m[x][1] *= k;
                m[x][2] *= k;
                m[x][3] *= k;
        }
        return m;
}

inline Matrix4& operator+= (Matrix4& m1, const Matrix4& m2) 
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m1[i][j] += m2[i][j];
        return m1;
}

inline Matrix4& operator*=(Matrix4& m1, const Matrix4& m2)
{
	return m1 = m1*m2;
}


inline Matrix4 operator/ (const Matrix4& m, float k)
{
        return m*(1/k);
}

inline Matrix4& operator/= (Matrix4& m, float k) 
{
        return m*=(1/k);
}

inline Matrix4 transpose (const Matrix4& m) 
{
        Matrix4 retVal;
        for (int x=0;x<4;++x){
                retVal[x][0] = m[0][x];
                retVal[x][1] = m[1][x];
                retVal[x][2] = m[2][x];
                retVal[x][3] = m[3][x];
        }
        return retVal;
}

inline Matrix4& Matrix4::rotate (float angle, const Vector4 &axis) 
{		
        float c = COSINE_FUNCTION(angle*PI / 180);
        float s = SINE_FUNCTION(angle*PI / 180);
        // One minus c (short name for legibility of formulai)
        float omc = (1 - c);
        
        float x = axis.x;
        float y = axis.y;
        float z = axis.z;   
        float xs = x * s;
        float ys = y * s;
        float zs = z * s;
        float xyomc = x * y * omc;
        float xzomc = x * z * omc;
        float yzomc = y * z * omc;
        
        e[0] = x*x*omc + c;
        e[1] = xyomc + zs;
        e[2] = xzomc - ys;
        e[3] = 0;
        
        e[4] = xyomc - zs;
        e[5] = y*y*omc + c;
        e[6] = yzomc + xs;
        e[7] = 0;
        
        e[8] = xzomc + ys;
        e[9] = yzomc - xs;
        e[10] = z*z*omc + c;
        e[11] = 0;
        
        e[12] = 0;
        e[13] = 0;
        e[14] = 0;
        e[15] = 1;
        
        return *this;  
}

inline Matrix4& Matrix4::translate(const Vector4& translation) 
{
        load_identity();
        e[12] = translation.x;
        e[13] = translation.y;
        e[14] = translation.z;
        return *this;
}

inline Matrix4& Matrix4::scale (float x, float y, float z)
{
        load_identity();
        e[0] = x;
        e[5] = y;
        e[10] = z;
        return *this;
}

inline Matrix4& Matrix4::basis_change (const Vector4& u, const Vector4& v, const Vector4& n)
{
        e[0] = u.x;
        e[1] = v.x;
        e[2] = n.x;
        e[3] = 0;
        
        e[4] = u.y;
        e[5] = v.y;
        e[6] = n.y;
        e[7] = 0;
        
        e[8] = u.z;
        e[9] = v.z;
        e[10] = n.z;
        e[11] = 0;
        
        e[12] = 0;
        e[13] = 0;
        e[14] = 0;
        e[15] = 1;
        
        return *this;
}

inline Matrix4& Matrix4::basis_change (const Vector4& v, const Vector4& n) 
{
        Vector4 u = cross_product(v,n);
        return basis_change (u, v, n);
}

inline Matrix4& Matrix4::projection_matrix (bool perspective,   float left_plane, float right_plane, 
                                           float top_plane, float bottom_plane, 
                                           float near_plane, float far_plane)
{
        float A = (right_plane + left_plane) / (right_plane - left_plane);
        float B = (top_plane + bottom_plane) / (top_plane - bottom_plane);
        float C = (far_plane + near_plane) / (far_plane - near_plane);
        
        load_identity();
        if (perspective) {
                e[0] = 2 * near_plane / (right_plane - left_plane);
                e[5] = 2 * near_plane / (top_plane - bottom_plane);
                e[8] = A;
                e[9] = B;
                e[10] = C;
                e[11] = -1;
                e[14] =  2 * far_plane * near_plane / (far_plane - near_plane);   
        } else {
                e[0] = 2 / (right_plane - left_plane);
                e[5] = 2 / (top_plane - bottom_plane);
                e[10] = -2 / (far_plane - near_plane);
                e[12] = A;
                e[13] =  B;
                e[14] = C;
        }
        return *this;
}

inline Matrix4 inverse (const Matrix4& m) 
{  
        return adjoint(m) / determinant(m);
}

inline Matrix4 inverse3 (const Matrix4& m)
{
        Matrix4 ret(true);
        //To make for code that is easier to read
        const float a = m[0][0];
        const float b = m[1][0];
        const float c = m[2][0];
        const float d = m[0][1];
        const float e = m[1][1];
        const float f = m[2][1];
        const float g = m[0][2];
        const float h = m[1][2];
        const float i = m[2][2];

        
        const float denom = 1/ (-c*e*g + b*f*g + c*d*h - a*f*h -b*d*i + a*e*i);

        ret[0][0] = denom*( e*i-f*h);
        ret[1][0] = denom*( c*h-b*i);
        ret[2][0] = denom*( b*f-c*e);
        ret[0][1] = denom*( f*g-d*i);
        ret[1][1] = denom*( a*i-c*g);
        ret[2][1] = denom*( c*d-a*f);
        ret[0][2] = denom*( d*h-e*g);
        ret[1][2] = denom*( b*g-a*h);
        ret[2][2] = denom*( a*e-b*d);
        
        return ret;
}



}
#endif

/*
 * $Author: macke $
 * $Date: 2002/01/28 00:49:45 $
 * $Log: types4_ops.h,v $
 * Revision 1.3  2002/01/28 00:49:45  macke
 * matrix <-> matrix4 conversion fix
 *
 * Revision 1.2  2001/08/06 12:16:28  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.1.2.1  2001/08/03 13:44:05  peter
 * pragma once obsolete...
 *
 * Revision 1.1  2001/07/30 23:43:23  macke
 * Häpp, då var det kört.
 *
 * Revision 1.24  2001/07/21 23:09:36  peter
 * intel fix..
 *
 * Revision 1.23  2001/07/06 01:47:24  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.22  2001/05/14 12:49:20  peter
 * *** empty log message ***
 *
 * Revision 1.21  2001/05/07 05:35:22  macke
 * level editor ... prototyp funkar,..,
 *
 * Revision 1.20  2001/05/06 23:13:02  macke
 * no message
 *
 */

