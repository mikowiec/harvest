
#include "os/compat.h"
#include "os/time.h"

#include <string>
#include <iostream>
#include <cmath>
#include <memory>

#include "ext/mtl/mtl.h"

#include "main/types.h"
#include "main/types_ops.h"

typedef float vector[4];
typedef float matrix[4][4];



vector
  A = { 1.1, 2.2, 3.3, 4.4 },
  B = { 5.5, 6.6, 7.7, 8.8 };

matrix M = {
	{ 1.2, 2.3, 3.4, 4.5 }, 
	{ 5.6, 6.7, 7.8, 8.9 }, 
	{ 0.9, 0.1, 1.2, 2.3 }, 
	{ 3.4, 4.5, 5.6, 6.7 }, 
};

/*
    tests
     1: | a+b |
     2: | diag(M+M) |
     3: dot(a*3.14 + b, a);
     4: | (M*M)*(a+b) |
*/


class CTest
{
	static void add(const vector a, const vector b, vector c)
	{
		c[0] = a[0]+b[0];
		c[1] = a[1]+b[1];
		c[2] = a[2]+b[2];
		c[3] = a[3]+b[3];
	}
	static void add(const matrix a, const matrix b, matrix c)
	{
		for (int i = 0; i < 16; ++i)
			c[0][i] = a[0][i] + b[0][i];
	}
	static float length(const vector a)
	{
		return sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2] + a[3]*a[3]);
	}
	static float dot(const vector a, const vector b)
	{
		return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
	}
	static void mult(const vector a, const float k, vector b)
	{
		b[0] = a[0] * k;
		b[1] = a[1] * k;
		b[2] = a[2] * k;
	}
	static void mult(const matrix a, const vector b, vector c)
	{
		c[0] = dot(a[0], b);
		c[1] = dot(a[1], b);
		c[2] = dot(a[2], b);
		c[3] = dot(a[3], b);
	}
	static void mult(const matrix a, const matrix b, matrix c)
	{
		c[0][0] = a[0][0]*b[0][0] + a[1][0]*b[0][1] + a[2][0]*b[0][2] + a[3][0]*b[0][3];
                c[0][1] = a[0][1]*b[0][0] + a[1][1]*b[0][1] + a[2][1]*b[0][2] + a[3][1]*b[0][3];
                c[0][2] = a[0][2]*b[0][0] + a[1][2]*b[0][1] + a[2][2]*b[0][2] + a[3][2]*b[0][3];
                c[0][3] = a[0][3]*b[0][0] + a[1][3]*b[0][1] + a[2][3]*b[0][2] + a[3][3]*b[0][3];

		c[1][0] = a[0][0]*b[1][0] + a[1][0]*b[1][1] + a[2][0]*b[1][2] + a[3][0]*b[1][3];
                c[1][1] = a[0][1]*b[1][0] + a[1][1]*b[1][1] + a[2][1]*b[1][2] + a[3][1]*b[1][3];
                c[1][2] = a[0][2]*b[1][0] + a[1][2]*b[1][1] + a[2][2]*b[1][2] + a[3][2]*b[1][3];
                c[1][3] = a[0][3]*b[1][0] + a[1][3]*b[1][1] + a[2][3]*b[1][2] + a[3][3]*b[1][3];

		c[2][0] = a[0][0]*b[2][0] + a[1][0]*b[2][1] + a[2][0]*b[2][2] + a[3][0]*b[2][3];
                c[2][1] = a[0][1]*b[2][0] + a[1][1]*b[2][1] + a[2][1]*b[2][2] + a[3][1]*b[2][3];
                c[2][2] = a[0][2]*b[2][0] + a[1][2]*b[2][1] + a[2][2]*b[2][2] + a[3][2]*b[2][3];
                c[2][3] = a[0][3]*b[2][0] + a[1][3]*b[2][1] + a[2][3]*b[2][2] + a[3][3]*b[2][3];

		c[3][0] = a[0][0]*b[3][0] + a[1][0]*b[3][1] + a[2][0]*b[3][2] + a[3][0]*b[3][3];
                c[3][1] = a[0][1]*b[3][0] + a[1][1]*b[3][1] + a[2][1]*b[3][2] + a[3][1]*b[3][3];
                c[3][2] = a[0][2]*b[3][0] + a[1][2]*b[3][1] + a[2][2]*b[3][2] + a[3][2]*b[3][3];
                c[3][3] = a[0][3]*b[3][0] + a[1][3]*b[3][1] + a[2][3]*b[3][2] + a[3][3]*b[3][3];
	}
	static vector a;
	static vector b;
	static matrix m;
public:
	static std::string name() { return "c"; }

	static void init(const vector a_, const vector b_, const matrix m_)
	{
		std::copy(a_, a_+4, a);
		std::copy(b_, b_+4, b);
		std::copy(m_[0], &m_[3][4], m[0]);

	}
	static float test1()
	{
		vector c;
		add(a, b, c);
		return length(c);
	}
	static float test2()
	{
		matrix r;
		add(m, m, r);
		return m[0][0]+m[1][1]+m[2][2]+m[3][3];
	}
	static float test3()
	{
		vector r, s;
		mult(a, 3.14, r);
		add(r, b, s);
		return dot(s, a);
	}
	static float test4()
	{
		vector vs, vr;
		matrix mp;
		add(a, b, vs);
		mult(m, m, mp);
		mult(mp, vs, vr);
		return length(vr);
	}
};

vector CTest::a;
vector CTest::b;
matrix CTest::m;

struct ReaperTest
{
	static reaper::Vector a, b;
	static reaper::Matrix m;

	static std::string name() { return "reaper"; }
	static void init(const vector a_, const vector b_, const matrix m_)
	{
		a = reaper::Vector(a_);
		b = reaper::Vector(b_);
		m = reaper::Matrix(m_[0]);
	}
	static float test1()
	{
		return a+b;
	}
	static float test2()
	{
		return m+m;
	}
	static float test3()
	{
		return reaper::dot(a*3.14 + b, a)
	}
	static float test4()
	{
		return reaper::length((m*m) * (a+b));
	}
};

reaper::Vector ReaperTest::a;
reaper::Vector ReaperTest::b;
reaper::Matrix ReaperTest::m;

struct ReaperTest2
{
	static reaper::Vector a, b;
	static reaper::Matrix m;

	static std::string name() { return "reaper2"; }
	static void init(const vector a_, const vector b_, const matrix m_)
	{
		a = reaper::Vector(a_,4);
		b = reaper::Vector(b_,4);
		m.c_matrix(m_[0]);
	}
	static float run()
	{
		reaper::Vector res_v(a);
		res_v += b;
		reaper::Matrix4 res_m(m);
		res_m *= m;
		return reaper::length4(res_m * res_v);
	}
};

reaper::Vector ReaperTest2::a;
reaper::Vector ReaperTest2::b;
reaper::Matrix ReaperTest2::m;

struct ReaperTest3
{
	static reaper::Vector a, b;
	static reaper::Matrix m;

	static std::string name() { return "reaper3"; }
	static void init(const vector a_, const vector b_, const matrix m_)
	{
		a = reaper::Vector(a_,4);
		b = reaper::Vector(b_,4);
		m.c_matrix(m_[0]);
	}
	static float run()
	{
		reaper::Vector res_v(a + b);
		reaper::Matrix res_m(m * m);
		return reaper::length4(res_m * res_v);
	}
};

reaper::Vector ReaperTest3::a;
reaper::Vector ReaperTest3::b;
reaper::Matrix ReaperTest3::m;

/*
struct MtlTest
{
	typedef mtl::matrix< float, mtl::rectangle<>,
		mtl::dense<>, mtl::row_major>::type Matrix;
	typedef mtl::matrix< float, mtl::rectangle<>,
		mtl::dense<mtl::external>, mtl::row_major>::type EMatrix;
	typedef mtl::dense1D<float, 4> Vector;
	typedef mtl::external_vec<float, 4> EVector;

	static std::string name() { return "mtl"; }


	static Vector a, b;
	static Matrix m;

	static void init(vector a_, vector b_, matrix m_)
	{
		std::copy(a_, a_+4, a.begin());
		std::copy(b_, b_+4, b.begin());
		std::copy(m_[0], m_[0]+16, m.data());
	}


	static float run()
	{
		Vector s(4), r(4);
		Matrix mp(4,4);

		mtl::add(a, b, s);
		mtl::mult(m, m, mp);
		mtl::mult(mp, s, r);
		return sqrt(mtl::dot(r, r));
	}

};

MtlTest::Vector MtlTest::a(4), MtlTest::b(4);
MtlTest::Matrix MtlTest::m(4,4);
*/

template<class Test>
void runtest(Test)
{
	reaper::os::time::Profiler perf;
	float res = 0;

	perf.start();
	Test::init(A, B, M);
	for (int j = 0; j < 10000; ++j) {
		res += Test::run();
		res += Test::run();
		res += Test::run();
		res += Test::run();
		res += Test::run();

		res += Test::run();
		res += Test::run();
		res += Test::run();
		res += Test::run();
		res += Test::run();

		res /= 10;
	}
	perf.stop();
	printf("%10s: %4.2f %8lu\n", Test::name().c_str(), res, perf.get_last());
}

int main()
{
	for (int i = 0; i < 5; ++i) {
		runtest(CTest());
		runtest(ReaperTest());
		runtest(ReaperTest2());
		runtest(ReaperTest3());
//		runtest(MtlTest());
	}

	return 0;
}

