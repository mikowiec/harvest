
#include <iostream>

#include "misc/uniqueptr.h"


using namespace reaper;
using namespace misc;

class Test
{
public:
	static int count;
	Test()  { count++; }
	~Test() { count--; if (count == 0) printf("die!\n"); }
};

int Test::count = 0;



int main()
{
	{
		UniquePtr<Test> p(new Test);
		printf("count %d\n", Test::count);

		UniquePtr<Test> q = p;
		printf("count %d\n", Test::count);

		{
			UniquePtr<Test> r = UniquePtr<Test>();
			printf("count %d\n", Test::count);
		}
		printf("count %d\n", Test::count);
		p.release();
		printf("count %d\n", Test::count);
	}
	{
		UniquePtr<Test> p(new Test);
		printf("count %d\n", Test::count);

		UniquePtr<Test> q = p;
		printf("count %d\n", Test::count);

		{
			UniquePtr<Test> r = UniquePtr<Test>();
			printf("count %d\n", Test::count);
		}
		printf("count %d\n", Test::count);
		p.release();
		printf("count %d\n", Test::count);
	}
	printf("count %d\n", Test::count);
	return 0;
}

