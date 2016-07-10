
#include "os/compat.h"

#include "os/debug.h"

#include "main/types.h"
#include "main/types_ops.h"
#include "main/types_io.h"

#include "main/types4.h"
#include "main/types4_ops.h"
#include "main/types4_io.h"

#include <iomanip>
#include <sstream>
#include <typeinfo>
#include <limits>

using namespace reaper;

debug::DebugOutput derr("mathtypes");

const float vec3[] = { 1, -2, 3e2 };
const float vec4[] = { 1, -1.2, -4e-1, 2.1e3 };
const float mat3x3[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
const float mat4x4[] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 7, 6, 5, 4, 3, 2, 1};

template<class T> struct result { };

template<> struct result<Vector> {
	static float arith() { return 3779.64648; }
};

template<> struct result<Vector4> {
	static float arith() { return 26456.79883; }
};

template<> struct result<Point> {
	static float arith() { return 3779.64648; }
};

float safe_log10(float f)
{
	return (f == 0.0) ? 1e-20 : log10(fabs(f));
}

bool non_zero(float f)
{
	return fabs(f) > std::numeric_limits<float>::min();
}

template<class T1, class T2>
void diverg(float d1, float d2, T1 t1, T2 t2)
{
	float e = fabs(d1 - d2);
	float m1 = safe_log10(std::max(d1,d2)), m2 = safe_log10(e);
	if (non_zero(e) && (non_zero(d1) ? (m1-m2) < 7 : m2 < 7))
		derr << "ERROR: " << t1 << " <> " << t2 << "\n";
	else
		derr << "OK\n";
	
}

template<class T>
void test_io(T t)
{
	T t3;
	for (int i = 0; i < 10000; ++i) {
		t3 = read<T>(write<T>(t3));
	}
	T t2(read<T>(write<T>(t)));
	derr << "test_io<" << typeid(T).name() << "> ";
	diverg(t != t2, 0, t, t2);
}

template<class T>
void test_arith(T t)
{
	float k = 3.14;
	float res = result<T>::arith();
	T t2 = t + t;
	T t3 = t - t;
	T t4 = t + k;
	T t5 = k + t;
	T t6 = t - k;
	T t7 = k - t;
	T t8 = t * k;
	T t9 = k * t;
	T ta = t / k;
	float l = length(t2+t3+t4+t5+t6+t7+t8+t9+ta);
	derr << "test_arith<" << typeid(T).name() << "> ";
	diverg(l, res, l, res);
}

template<class T>
void test_mul(T t)
{
	T tr = t * t;
	float l = length(tr);
	derr << "test_mul<" << typeid(T).name() << "> ";
	diverg(l, res, l, res);
}

int main()
{
	derr.precision(10);
	derr.width(10);
	test_io<Vector>(Vector(vec3));
	test_io<Vector4>(Vector4(vec4));
	test_io<Point>(Point(vec3));
	Matrix4 m4;
	m4.opengl_matrix(mat4x4);
//	test_io<Matrix4>(m4);
//	test_io<Matrix>(Matrix(m4));
	test_arith<Vector>(Vector(vec3));
	test_arith<Vector4>(Vector4(vec4));
	test_arith<Point>(Point(vec3));
	return 0;
}


