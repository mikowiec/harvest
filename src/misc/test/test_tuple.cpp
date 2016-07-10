
#include "os/compat.h"

#include "os/time.h"
#include "misc/tuple.h"
#include "misc/test_main.h"

#include <iostream>
#include <string>

using namespace reaper;
using misc::Tuple;
using misc::tuple;
using misc::bind;

static int g_n;

Tuple<int, double> ft(Tuple<double, int> t)
{
	++g_n;
	return tuple(47 + t.get<1>(), t.get<1>() + t.get<0>());
}

void fd(double di, int ii, int& iu, double& du)
{
	++g_n;
	iu = 47 + ii;
	du = ii + di;
}


int test_main()
{
	Tuple<int, float, std::string> t(3, 0.14159265, "foo");
	int i = t.get<0>();
	float f = t.get<1>();
	std::string s = t.get<2>();
	derr << t.get<0>() << ' ' << t.get<1>() << ' ' << t.get<2>() << '\n';
	derr << i << ' ' << f << ' ' << s << '\n';

	derr << tuple(1, 1.41, "tjipp").get<1>() << '\n';
	
	{ os::time::Profile tp("tuple");
	  int iacc = 0; double dacc = 0.0;
	  g_n = 0;
	  for (int j = 0; j < 2; j++) {
		int i; double d;
		bind(i, d, ft(tuple(dacc, iacc)));
		iacc += i;
		dacc += d;
	  }
	  derr << "res: " << g_n << ' ' << iacc << ' ' << dacc << '\n';
	}
	{ os::time::Profile tp("direct");
	  int iacc = 0; double dacc = 0.0;
	  g_n = 0;
	  for (int j = 0; j < 2; j++) {
		int i; double d;
		fd(dacc, iacc, i, d);
		iacc += i;
		dacc += d;
	  }
	  derr << "res: " << g_n << ' ' << iacc << ' ' << dacc << '\n';
	}
	return 0;
}





