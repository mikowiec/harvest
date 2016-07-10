#include "os/compat.h"
#include "main/types_ops.h"
#include "main/types_io.h"
#include "main/types4.h"
#include "main/types4_io.h"
#include "main/types_ops.h"

#include "misc/parse.h"

namespace reaper {



Matrix Matrix::id()
{
	Matrix m;
	mk_id_mtx(m.m);
	m.p = Point(0,0,0);
	return m;
}


Matrix inverse3(const Matrix& m) {
        const float a = m[0][0];
        const float b = m[0][1];
        const float c = m[0][2];
        const float d = m[1][0];
        const float e = m[1][1];
        const float f = m[1][2];
        const float g = m[2][0];
        const float h = m[2][1];
        const float i = m[2][2];
        
        const float denom = 1.0f / (-c*e*g + b*f*g + c*d*h - a*f*h -b*d*i + a*e*i);

        Matrix ret;
        ret.row(0, Vector(e*i-f*h, c*h-b*i, b*f-c*e) * denom);
	ret.row(1, Vector(f*g-d*i, a*i-c*g, c*d-a*f) * denom);
	ret.row(2, Vector(d*h-e*g, b*g-a*h, a*e-b*d) * denom);
	ret.pos() = m.pos();

        return ret;
}

void orthonormalize3(Matrix &M){
	Vector v1 = M.row(0);
	Vector v2 = M.row(1);
	Vector v3 = M.row(2);

	v1 = norm(v1);
	v3 = norm(cross(v1,v2));
	v2 = -cross(v1,v3);

	M.row(0, v1);
	M.row(1, v2);
	M.row(2, v3);

}

Matrix::Matrix(const Matrix4 &m) {
	col(0, Vector(m[0]));
	col(1, Vector(m[1]));
	col(2, Vector(m[2]));
	pos() = Point(m[3]);
}

Matrix4::Matrix4(const Matrix &m) {	

	(*this)[0][0] = m[0][0];
	(*this)[1][0] = m[0][1];
	(*this)[2][0] = m[0][2];

	(*this)[0][1] = m[1][0];
	(*this)[1][1] = m[1][1];
	(*this)[2][1] = m[1][2];

	(*this)[0][2] = m[2][0];
	(*this)[1][2] = m[2][1];
	(*this)[2][2] = m[2][2];

	(*this)[3][0] = m.pos()[0];
	(*this)[3][1] = m.pos()[1];
	(*this)[3][2] = m.pos()[2];
	
	(*this)[0][3] = 0;
	(*this)[1][3] = 0;
	(*this)[2][3] = 0;
	(*this)[3][3] = 1;
}




void mk_rot_mtx(Matrix3x3& m, const Rotation& r)
{
	mk_rot_mtx(m, r.angle, r.axis);
}

void mk_scale_mtx(Matrix3x3& m, float x, float y, float z)
{
	m[0].x = x; m[0].y = 0; m[0].z = 0;
	m[1].x = 0; m[1].y = y; m[1].z = 0;
	m[2].x = 0; m[2].y = 0; m[2].z = z;
}

void mk_id_mtx(Matrix3x3& m)
{
	mk_scale_mtx(m, 1,1,1);
}


void mk_rot_mtx(Matrix3x3& m, float angle, const Vector& axis)
{
        const float c = COSINE_FUNCTION(angle*PI / 180);
        const float s = SINE_FUNCTION(angle*PI / 180);
        const float omc = (1 - c);   // One minus c (short name for legibility of formulai)
        
        const float x = axis.x;
        const float y = axis.y;
        const float z = axis.z;
	
        const float xs = x * s;
        const float ys = y * s;
        const float zs = z * s;
        const float xyomc = x * y * omc;
        const float xzomc = x * z * omc;
        const float yzomc = y * z * omc;
        
	m.col(0, Vector(
                 x*x*omc + c,
                 xyomc + zs,
                 xzomc - ys));
        
	m.col(1, Vector(
                 xyomc - zs,
                 y*y*omc + c,
                 yzomc + xs));
        
	m.col(2, Vector(
                 xzomc + ys,
                 yzomc - xs,
                 z*z*omc + c));

}

void mtx_from_quat(Matrix3x3& m, const Quaternion& quat)
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

	m[0] = Vector( 1.0f - 2.0f * (fYY + fZZ),
			   2.0f * (x * y + w * z),
			   2.0f * (x * z - w * y));
	m[1] = Vector( 2.0f * (x * y - w * z),
			   1.0f - 2.0f * (fXX + fZZ),
			   2.0f * (y * z + w * x));
	m[2] = Vector( 2.0f * (x * z + w * y),
			   2.0f * (y * z - w * x),
			   1.0f - 2.0f * (fXX + fYY));
}


}
