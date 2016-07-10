

#ifndef REAPER_MSG_PART_H
#define REAPER_MSG_PART_H

#include "msg/message.h"

namespace reaper {
namespace msg {




class Hub {

public:
	void send(const Message&);
};

}
}

#endif

