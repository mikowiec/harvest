
#include "misc/test_main.h"
#include "misc/utility.h"

#include <algorithm>
#include <cmath>


using namespace reaper::misc;

const int I = 20;
const int N = 10000;

template<class R>
void test_rnd()
{
	R rnd(0, I);
	int counters[I+2];
	std::fill(counters, counters+I+2, 0);
	for (int i = 0; i < N*I; ++i) {
		counters[int(rnd())+1]++;
	}
	for (int i = 0; i < I+2; ++i) {
		printf("%3d: ", i-1);
		for (int j = 0; j < (counters[i]*2*I)/N; ++j)
			putchar('#');
		putchar('\n');
	}
}


int test_main()
{

	test_rnd<FakeRand>();
	test_rnd<CenterRand>();

	return 0;
}
