

#include "os/compat.h"


#include "os/gfx.h"
#include "os/time.h"
#include "misc/test_main.h"
#include "main/types_io.h"

#include "game/helpers.h"

using namespace reaper;
using namespace world;


void verify_tri()
{
	WorldRef wr = World::get_ref();
	int n = 0;
	for (tri_iterator i = wr->begin_tri(); i != wr->end_tri(); ++i) {
		printf("tri %d %x\n", ++n, *i);
		derr << i->a << '\n';
	}
}

void verify_si()
{
	WorldRef wr = World::get_ref();
	int n = 0;
	for (si_iterator i = wr->begin_si(); i != wr->end_si(); ++i) {
		printf("si %d %x\n", ++n, (*i).get());
		derr << i->get_pos() << '\n';
	}
}

void verify_st()
{
	WorldRef wr = World::get_ref();
	int n = 0;
	for (st_iterator i = wr->begin_st(); i != wr->end_st(); ++i) {
		printf("st %d %x\n", ++n, (*i).get());
		derr << i->get_pos() << '\n';
	}
}

int foo()
{
	os::time::Profile p_all("all");

	os::gfx::Gfx gx;

	game::Game* go = new game::Game(gx);


	go->game_init();
	verify_tri();
	float f = World::get_ref()->get_altitude(Point2D(3080.949951, 3412.030029));
	verify_si();
	verify_st();
	go->game_start();

	go->shutdown();
	delete go;

	return 0;
}

int test_main()
{
	foo();	
	return 0;
}
 
