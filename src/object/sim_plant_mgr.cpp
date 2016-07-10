#include "PlantFFI_stub.h"
#include "RtsAPI.h"


#include "os/compat.h"

#include "os/debug.h"

#include "os/gl.h"
#include "gfx/misc.h"

#include "object/sim_plant_mgr.h"

extern "C" {
extern void __stginit_PlantFFI();
}

namespace reaper {

namespace haskell {

class Haskell
{

public:
	Haskell()
	{
		char *argv[] = {"reaper", 0};
		startupHaskell(0, argv, __stginit_PlantFFI);
	}
	~Haskell()
	{
		shutdownHaskell();
	}

};

}
	
namespace object {
namespace plants {

debug::DebugOutput derr("sim_plant_mgr");

class Plant : public SimPlant
{
	haskell::Haskell hs;
	HsStablePtr ptr;
public:
	Plant()
	{
		ptr = ::mk_plant((void*)"LSystem:lsystem");
	}
	void simulate()
	{
		::simul(ptr);
	}

	void render()
	{
		glColor4(Color(1,0,0));
//		gfx::misc::sphere(2,20,20);
		::render(ptr);
	}

};


SimPlant* mk_plant()
{
	return new Plant();
}


}
}
}


