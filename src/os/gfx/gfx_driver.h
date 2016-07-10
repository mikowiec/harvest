
/* $Id: gfx_driver.h,v 1.5 2003/03/28 13:48:58 pstrand Exp $ */


#ifndef REAPER_HW_GFX_DRIVER_H
#define REAPER_HW_GFX_DRIVER_H

#include "misc/prim_iter.h"

namespace reaper {
namespace debug { class DebugOutput; }
namespace os {
namespace gfx { struct VideoMode; }

namespace lowlevel {

struct Gfx_driver_data;

class Gfx_driver
{

public:
	virtual ~Gfx_driver() { }
	virtual Gfx_driver_data* internal_data() = 0;
	virtual bool update_screen() = 0;
	virtual bool setup_mode(const gfx::VideoMode&) = 0;
	virtual void restore_mode() = 0;

	virtual misc::prim_iter_if<gfx::VideoMode>* modes() const = 0;
};


}
}
}


#endif


