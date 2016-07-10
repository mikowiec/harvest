
/* $Id: gfx.h,v 1.4 2003/03/28 13:48:37 pstrand Exp $ */

#ifndef REAPER_HW_GFX_H
#define REAPER_HW_GFX_H

#include "misc/prim_iter.h"

namespace reaper {
namespace os {
namespace lowlevel { class Gfx_driver; }
namespace event { class EventDispatcher; }
namespace gfx {


struct VideoMode
{
	VideoMode();
//	VideoMode(int w, int h, int d = 0, bool f = false, int fq = 0, int id = -1);
	VideoMode(int w, int h, bool f);
	int width;	  ///< Width in pixels.
	int height;	  ///< Height in pixels.
	bool fullscreen;  ///< Switch to fullscreen.
//	int depth;	  ///< %Color-depth in bits.  (only in fullscreen)
//	int frequency;	  ///< Vertical refresh rate. (only in fullscreen)
//	int id;
};

bool operator<(const VideoMode&, const VideoMode&);
bool operator==(const VideoMode&, const VideoMode&);

struct Gfx_data;


/* Creating an instance of the Gfx class initializes the (specified)
 * graphic subsystem and creates a window in the given mode and resolution.
 * Settings are read from the config/hw_gfx file.
 */

class Gfx
{
	static VideoMode default_mode;

	Gfx_data* data;
	lowlevel::Gfx_driver* driver;

	friend class event::EventDispatcher;
public:
	Gfx(const VideoMode& init_mode);
	Gfx();
	~Gfx();

	bool change_mode(const VideoMode& = default_mode);
//	void reinit();

	bool update_screen();
	
	void restore();
	const VideoMode& current_mode() const;
	bool is_accelerated() const;
	bool alpha() const;
	bool stencil() const;

	reaper::misc::prim_iter<VideoMode> modes();
};


}

}
}

#endif


