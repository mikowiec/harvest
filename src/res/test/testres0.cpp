
#include "os/compat.h"
#include "os/debug.h"
#include "res/res.h"

#include <iostream>
#include <cstring>

using reaper::res::res_stream;
using reaper::res::Texture;
using reaper::res::Level;

int main()
{
	reaper::debug::ExitFail ef(1);

	res_stream t1(Texture, "main");

	if (!t1.good())
		return 7;

	if (t1.id() != "main")
		return 2;
	char buf[8];
	t1.read(buf, 8);
	if (strncmp(&buf[1], "PNG", 3) != 0)
		return 3;

	res_stream t1_1(t1.clone());

	t1_1.read(buf, 8);
	if (strncmp(&buf[1], "PNG", 3) != 0)
		return 4;


	res_stream t2(Level, "test_level");
	t2.get();

	if (!t2.good())
		return 5;

	float f;
	t2 >> f;

	if (t2.good())
		return 6;

	ef.disable();
	return 0;
}
