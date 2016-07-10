
#ifndef REAPER_GAME_DRIVER_H
#define REAPER_GAME_DRIVER_H

namespace reaper {
namespace game {

class Reaper
{
	class Impl;
	Impl* i;
public:
	Reaper();
	~Reaper();

	void run();
};

}
}

#endif
