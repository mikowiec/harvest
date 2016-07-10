
#include "os/compat.h"

#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>

#include "misc/iostream_helper.h"
#include "misc/test_main.h"

#include "zlib.h"

#include "res/zstream.h"

using namespace reaper;


int test_main()
{
	std::fstream fs("test.gz", std::ios::in | std::ios::binary);
	res::izstream iz(fs);
	char c;
	int n = 0;
	while (iz.get(c)) {
		derr << c;
		n++;
	}
	derr << "|---: " << n << '\n';
	return 0;
}




