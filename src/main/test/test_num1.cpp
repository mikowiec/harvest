
#include "os/compat.h"

#include "main/types.h"
#include "main/types_ops.h"
#include "main/types_io.h"

#include <iostream>

using namespace reaper;

int main()
{
	Vector v1(1, 2, 3);
	Vector v2(4, 5, 6);
	Vector v3 = v1 + v2;
	Vector v4(v3);
	Point pt1(2,3,4), pt2(5,6,7);
	v3 += v1;
	std::cout << v1 << ' ' << v2 << ' ' << v3 << ' ' << v4 << ' ' << (v2 == v4) << '\n';
	std::cout << Point(pt1 + pt2) << ' ' << dot(v1, v3) << '\n';
	return 0;
}

