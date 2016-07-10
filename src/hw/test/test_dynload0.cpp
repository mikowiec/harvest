
#include "os/compat.h"

#include <math.h>

#include "os/debug.h"
#include "os/dynload.h"
#include "misc/test_main.h"

#include <iostream>

using namespace reaper;
using os::dynload::DynLoader;

int test_main()
{
#ifdef WIN32
	DynLoader dyn("msvcrt");
#else
	DynLoader dyn("m");
#endif
	double (*sine)(double);
	dyn.load("sin", sine);
	derr << "loaded: " << sine(1.2) << " int: " << sin(1.2) << '\n';
	return 0;
}


