#ifndef REAPER_HW_GFXPLUGIN_H
#define REAPER_HW_GFXPLUGIN_H

#include "misc/plugin.h"

namespace reaper {
namespace os {
namespace ifs { struct Gfx; }
namespace lowlevel { class Gfx_driver; }
namespace gfx {


typedef misc::Plugin<lowlevel::Gfx_driver, ifs::Gfx> GfxPlugin;


}
}
}

#endif

