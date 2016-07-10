
#include "os/compat.h"
#include "misc/utility.h"
#include "math.h"

namespace reaper {
namespace misc {


/* 
 * Uses the linear congruential method,
 * Knuth, TAOCP II, 3.2.1 and 3.6
 */

const long mm = 2147483647;
const long aa = 48271;
const long qq = mm / aa; // 44488;
const long rr = mm % aa; // 3399;



FakeRand::FakeRand(float mn, float mx, long seed)
 : min(mn), len(mx-mn), x(seed)
{ }


float FakeRand::operator()()
{
	x = aa*(x%qq) - rr*(x/qq);
	if (x < 0)
		x += mm;
	return min + (static_cast<float>(x) / mm) * len;
}

CenterRand::CenterRand(float mn, float mx, long seed)
 : rnd(0, 3.14159, seed), min(mn), len(mx-mn)
{ }

// Just a hack.. ;)
float CenterRand::operator()()
{
	float r = rnd();
	float v = sin(r);
	return min + (r < 3.14/2 ? v : 2.0-v)*len/2;
}



}
}

