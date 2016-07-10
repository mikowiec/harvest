#include "os/compat.h"
#include "perlin.h"
#include <iostream>

	#define s_curve(t) ( t * t * (3. - 2. * t) )
	#define lerp(t, a, b) ( a + t * (b - a) )
	#define setup(i,b0,b1,r0,r1)\
		t = vec[i] + N;\
		b0 = ((int)t) & BM;\
		b1 = (b0+1) & BM;\
		r0 = t - (int)t;\
		r1 = r0 - 1.;


namespace reaper 
{
namespace gfx
{
namespace mesh
{

	using std::cout;
	using std::cin;

	double Perlin::noise1(double arg)
	{
		int bx0, bx1;
		float rx0, rx1, sx, t, u, v, vec[1];

		vec[0] = (float) arg;
		if (start) {
			start = 0;
			init();
		}

		setup(0, bx0,bx1, rx0,rx1);

		sx = s_curve(rx0);

		u = rx0 * g1[ p[ bx0 ] ];
		v = rx1 * g1[ p[ bx1 ] ];

		return lerp(sx, u, v);
	}

	float Perlin::noise2(float x, float y)
	{
		float vec[2];
		vec[0] = x;
		vec[1] = y;

		return noise2(vec);
	}


	float Perlin::noise2(float vec[2])
	{
		int bx0, bx1, by0, by1, b00, b10, b01, b11;
		float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
		register i, j;

		if (start) {
			start = 0;
			init();
		}

		setup(0, bx0,bx1, rx0,rx1);
		setup(1, by0,by1, ry0,ry1);

		i = p[ bx0 ];
		j = p[ bx1 ];

		b00 = p[ i + by0 ];
		b10 = p[ j + by0 ];
		b01 = p[ i + by1 ];
		b11 = p[ j + by1 ];

		//cout << b00 << ", " << b01 << ", " << b10 << ", " << b11 << "\n";

		sx = s_curve(rx0);
		sy = s_curve(ry0);

		//cout << "sx " << sx << " " << sy << "\n";

	#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

		q = &g2[ b00 ] ; u = at2(rx0,ry0);
		q = &g2[ b10 ] ; v = at2(rx1,ry0);
		a = lerp(sx, u, v);

		q = &g2[ b01 ] ; u = at2(rx0,ry1);
		q = &g2[ b11 ] ; v = at2(rx1,ry1);
		b = lerp(sx, u, v);

		return lerp(sy, a, b);
	}

	void Perlin::normalize2(float v[2])
	{
		float s;

		s = sqrt(v[0] * v[0] + v[1] * v[1]);
		/*
		if(s < 0.0){
			cout << "foo: " << s << "\n";
			cout << "v01: " << v[0] << ", " << v[1] << "\n";
			cin.get();
		}
		*/
		v[0] = v[0] / s;
		v[1] = v[1] / s;

		
	}


	void Perlin::init()
	{
		int i, j, k;

		// Setting up grids. Very silly in deed.
		for (i = 0 ; i < B ; i++) {
			p[i] = i;

			g1[i] = (float)(( rand()  % (B + B)) - B) / B;

			for (j = 0 ; j < 2 ; j++)
				g2[2*i + j /*(B+B+2)*j*/] = (float)((rand() % (B + B)) - B) / B;
			normalize2(&g2[i]);
		}

		// Shuffle around in a silly manor.
		while (--i) {
			k = p[i];
			p[i] = p[j = rand() % B];
			p[j] = k;
		}

		// Clone first part
		for (i = 0 ; i < B + 2 ; i++) {
			p[B + i] = p[i];
			g1[B + i] = g1[i];
			for (j = 0 ; j < 2 ; j++)
				g2[ (B + i)*2 + j /*(B+B+2)*j*/] = g2[ i*2 + j /*(B+B+2)*j*/];

		}
	}

}
}
}

