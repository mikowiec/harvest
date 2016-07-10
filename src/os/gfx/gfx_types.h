
/* $Id: gfx_types.h,v 1.3 2003/03/25 00:27:59 pstrand Exp $ */


#ifndef HW_GFX_TYPES_H
#define HW_GFX_TYPES_H



#include <queue>
#include <set>

#include "os/gfx.h"

struct _XDisplay;
typedef struct _XDisplay Display;

namespace reaper {
namespace os {
namespace lowlevel {

struct Gfx_driver_data {
	Display* display;
	
	std::queue<std::pair<int,bool> > key_queue;
	gfx::VideoMode desktop;
	gfx::VideoMode current;

	bool window_active;
	bool is_accelerated;
	bool alpha;
	bool stencil;
};


}
}
}


#endif


