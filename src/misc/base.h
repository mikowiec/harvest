
#ifndef REAPER_MISC_BASE_H
#define REAPER_MISC_BASE_H

namespace reaper {
namespace misc {


class NoCopy {
	NoCopy(const NoCopy&);
public:
	NoCopy() { }
};


}
}

#endif


