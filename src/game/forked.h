
#ifndef REAPER_GAME_FORKED_H
#define REAPER_GAME_FORKED_H

#include <string>

namespace reaper {
namespace os {
namespace concurrent {
	class Thread;
}
}

class Reaper
{
	class Run;
	Run* run;
	os::concurrent::Thread* gth;
public:
	Reaper(const std::string& rootdir = "");
	bool start();
	std::string get_errmsg();
	void stop();
	bool is_done();
};

}


extern "C" reaper::Reaper* create_reaper(void*);

#endif

