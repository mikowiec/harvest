
#include "os/compat.h"

#include "game/progressbar.h"

#include "gfx/texture.h"
#include "gfx/misc.h"

#include "os/debug.h"
#include "os/gl.h"
#include "os/gfx.h"

#include "misc/font.h"
#include "misc/profile.h"

namespace reaper {
namespace game {

namespace {
	reaper::debug::DebugOutput dlog("progress", 5);
}

ProgressBar::ProgressBar(os::gfx::Gfx& g, const std::string& lbl, int max_ticks)
 : gx(g), label(lbl), val(0), add(1.0 / max_ticks), lbl_w(label.size() * 0.04),
   back("reaper-logo")
{
	glViewport(0, 0, gx.current_mode().width, gx.current_mode().height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void ProgressBar::render(const std::string& msg)
{
	glViewport(0, 0, gx.current_mode().width, gx.current_mode().height);
	back.use();
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0.0, 1.0);
		glTexCoord2f(1, 0);
		glVertex2f(1.0, 1.0);
		glTexCoord2f(1, 1);
		glVertex2f(1.0, 0.0);
		glTexCoord2f(0, 1);
		glVertex2f(0.0, 0.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	const float meter_y = 0.05;
	glColor3f(0.2, 0.2, 0.8);
	gfx::misc::meter(0.1, meter_y, 0.8, 0.05, 1.0, val);
	glColor3f(0.2, 0.6, 0.6);
	glBegin(GL_LINE_LOOP);
		glVertex2f(0.1, meter_y);
		glVertex2f(0.9, meter_y);
		glVertex2f(0.9, meter_y + 0.05);
		glVertex2f(0.1, meter_y + 0.05);
	glEnd();

	float pos = msg.length() * 0.01;
	gfx::font::glPutStr(0.5 - pos, meter_y + 0.01, msg, gfx::font::Medium, 0.02, 0.04);
}

void ProgressBar::tick(const std::string& msg)
{
	dlog << msg << '\n';
	val += add;
	render(msg);
	gx.update_screen();
}




}
}


