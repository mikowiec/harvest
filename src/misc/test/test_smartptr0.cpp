
#include "misc/smartptr.h"

#include <stdio.h>

using namespace reaper;


using misc::SmartPtr;

class Test
{
	int n;
public:
	Test(int i) : n(i)  { }
	~Test() { printf("deleted: %d\n", n); }
};

class Test2
{
	int count;
	int n;
	friend int main();
public:
	Test2(int i) : n(i) { }
	~Test2() { printf("deleted: %d\n", n); }
};

int main()
{
	Test* p1 = new Test(1);
	Test* p2 = new Test(2);
	
	{
		SmartPtr<Test> s1(p1);
		SmartPtr<Test> s2(p2);
		SmartPtr<Test> s3(s1);

		SmartPtr<Test> s4;
		
		s4 = s1;

		s2 = s1;

		printf("%s\n", (s4 == s3) ? "ok" : "nah");
	}

	Test2* p3 = new Test2(3);
	Test2* p4 = new Test2(4);
	{
		SmartPtr<Test2, misc::Internal> s1(p3, &p3->count);
		SmartPtr<Test2, misc::Internal> s2(p4, &p4->count);
		SmartPtr<Test2, misc::Internal> s3(s1);

		SmartPtr<Test2, misc::Internal> s4;
		
		s4 = s1;

		s2 = s1;

		printf("%s\n", (s4 == s3) ? "ok" : "nah");
	}


	return 0;
}

