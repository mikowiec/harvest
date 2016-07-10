

#include "os/compat.h"
#include "os/debug.h"

#include "game/helpers.h"

#include "game/driver.h"

#include <iostream>

namespace reaper {
namespace game {


debug::DebugOutput derr("game::driver");

class Reaper::Impl
{


};

Reaper::Reaper()
 : i(new Reaper::Impl())
{
	derr << "Reaper::Reaper\n";
}

Reaper::~Reaper()
{
	delete i;
	derr << "Reaper::~Reaper\n";
}

void Reaper::run()
{
	derr << "Reaper::run\n";
}

}
}
