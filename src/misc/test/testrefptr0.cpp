#include "os/compat.h"
#include <stdio.h>

#include "misc/refptr.h"

using reaper::misc::RefPtr;

class test
{
public:
	static int count;
	test()  { count++; }
	~test() { count--; }
	int f() { return 5; }
};

int test::count = 0;

namespace reaper {
namespace misc {
test* RefPtr<test>::inst = 0;
int RefPtr<test>::count = 0;
}
}

int main()
{
	test* t = new test;
	{
		RefPtr<test> p(t);
		if (test::count != 1)
			return 3;
		RefPtr<test> q = p;
		RefPtr<test> r = p;
		r = q;
		if (test::count != 1)
			return 2;
	}
	if (test::count != 0)
		return 1;
	return 0;
}

