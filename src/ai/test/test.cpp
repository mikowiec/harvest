#include <iostream>
#include "os/compat.h"
#include "ai/steering.h"
#include "main/types_io.h"

using namespace std;
using namespace reaper;
using namespace reaper::ai::steering;



void main(void)
{
	Point p(8,0,0);
	Vector v(0,0,0);

	Point t(10,0,0);
	Vector u(5,0,0);

	Steering steer(p, v, 5);

	cout << steer.arrive(t, 5) << endl;

	cout << "test complete\n";
}
