#include "os/compat.h"

#ifndef VERSION
#define VERSION "snapshot "__DATE__" "__TIME__
#endif

namespace reaper {
const char *version = VERSION;
}
