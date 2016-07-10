#include "os/compat.h"

#include "gfx/shadow/main.h"
#include "gfx/settings.h"

namespace reaper {
namespace gfx {

ShadowRenderer* ShadowRenderer::get(int type)
{
	using namespace shadow;

	if(type > 2 && Settings::current.alpha_bits == 0) {
		type = 2;
	}

	switch(type)
	{
	case 5:	return new SilhouetteShadowAll;
	case 4:	return new SilhouetteShadowDynamic;
	case 3:	return new SilhouetteShadow;
	case 2:	return new SimpleShadowDynamic;
	case 1:	return new SimpleShadow;

	case 0:
	default:return 0;
	}
}

}
}

