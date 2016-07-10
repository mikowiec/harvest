
#include "os/compat.h"

#include <iostream>
#include <vector>
#include <iostream>

#include "main/types.h"
#include "gfx/gfx_types.h"
#include "gfx/io.h"

#include "misc/test_main.h"

using namespace reaper;
using std::vector;

int test_main()
{
	os::time::Profiler ter;


	for (int i = 0; i < 2; i++) {
		vector<Point>    points;   	
		vector<gfx::TexCoord> texcoords; 
		vector<Vector>   normals;    
		vector<gfx::Color>    colors; 
		vector<IdxTriangle> triangles; 
		ter.start();
		gfx::lowlevel::read_terrain("terrain", points, texcoords, normals, colors, triangles);
		ter.stop();
		ter.int_report_last();
	}

	ter.int_report();
        return 0;
}

