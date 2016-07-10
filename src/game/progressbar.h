#ifndef REAPER_GAME_PROGRESSBAR_H
#define REAPER_GAME_PROGRESSBAR_H

#include "gfx/texture.h"

#include <string>

namespace reaper {
namespace os { namespace gfx { class Gfx; } }
namespace game {


class ProgressBar
{
	os::gfx::Gfx& gx;
	std::string label;
	float val, add, lbl_w;
	gfx::texture::Texture back;
public:
	ProgressBar(os::gfx::Gfx&, const std::string& lbl, int max_ticks);
	void render(const std::string& msg);
	void tick(const std::string& msg);
};




}
}

#endif

