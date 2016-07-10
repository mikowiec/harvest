#include "os/compat.h"
#include "main/types.h"
#include "main/types_ops.h"

using namespace reaper;

int main() {
	reaper::Point p(1,2,3);
	reaper::Vector v(0,2,4);

	p = p+p - dot(p,p);
	v = v+v - cross(v,v);

	return 0;
}

