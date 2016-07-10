
#include "os/compat.h"

#include "os/gfx.h"



namespace reaper {
namespace os {
namespace gfx {

VideoMode::VideoMode(int w, int h, bool f)
 : width(w), height(h), fullscreen(f)
{ }

VideoMode::VideoMode()
 : width(0), height(0), fullscreen(false)
{ }

bool operator<(const VideoMode& m1, const VideoMode& m2)
{
	return m1.width < m2.width || m1.height < m2.height
	    || m1.fullscreen < m2.fullscreen;
}

bool operator==(const VideoMode& m1, const VideoMode& m2)
{
	return m1.width == m2.width && m1.height == m2.height &&
	       m1.fullscreen == m2.fullscreen;
}


}
}
}
