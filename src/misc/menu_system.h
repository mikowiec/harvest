#ifndef REAPER_MISC_MENU_SYSTEM_H
#define REAPER_MISC_MENU_SYSTEM_H

#include <memory>
#include <cmath>

namespace reaper {
namespace menu {

using reaper::gfx::texture::Texture;

class Box
{
public:
	float x1,y1,x2,y2;
	Box() : x1(0), y1(0), x2(0.1), y2(0.1) { }
	Box(float xx1, float yy1, float xx2, float yy2)
	 : x1(xx1), y1(yy1), x2(xx2), y2(yy2) { }
	bool inside(float x, float y) const
	{
		return (x1 <= x && x <= x2 &&
			y1 <= y && y <= y2);
	}
};

/// Menu system class, active once and use Menuexec's for each menu-selection
class MenuSystem
{
	class Impl;
	Impl* i;
public:
	MenuSystem();
	~MenuSystem();

	void draw_background(Texture&);
	void draw_mouse_ptr(float x, float y);

	os::event::EventProxy& event();
	os::gfx::Gfx& gfx();

	void sel_snd();
	void press_snd();

	void do_stuff();
};

void draw_texture(Box b, Texture&);
void draw_text_box(Box b, const std::string& text, bool active, bool passive, bool editing);
void draw_header(Box b, const std::string& text);


template<class Iter>
void vertical_layout(Box b, Iter it, int n)
{
	const float cell_height = 0.05;
	const float y_delim = 0.05;
	const float cell_width = 0.25;
	const float y_add = cell_height + y_delim;

	float height = b.y2 - b.y1;
	float width = b.x2 - b.x1;

	int rows = int(floor(height / y_add));
	int cols = int(ceil(float(n) / rows));

	float x_sep = (width - cols*cell_width) / (cols > 1 ? (cols-1) : 1);

	float real_width = cols * cell_width + (cols-1) * x_sep;
	float x = b.x1 + width/2 - real_width/2;
	float y = b.y1;
	for (int i = 1; i < n+1; ++i) {
		(*it++)->set_box(Box(x, y, x + cell_width, y+cell_height));
		y += y_add;

		if ((i % rows) == 0) {
			y = b.y1;
			x += cell_width + x_sep;
		}
	}
}


}
}

#endif
