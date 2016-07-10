#ifndef REAPER_MISC_MENU_H
#define REAPER_MISC_MENU_H

#include "os/gfx.h"
#include "msg/event.h"
#include "os/debug.h"
#include "os/abstime.h"
#include "misc/parse.h"
#include "main/exceptions.h"

#include "gfx/texture.h"
#include "misc/menu_system.h"

#include <algorithm>
#include <functional>
#include <string>


///
namespace reaper {
namespace menu {

/** MenuItem.
    Has a bounding box, active texture, inactive texture
    and a return value (to be sent back if the item is selected).
*/
template<class V>
class MenuItem
{
	Box box;
	bool passive;
	bool edit;
	V r_val;
protected:
	std::string text;

	MenuItem(Box b, std::string t, bool p, bool e)
	: box(b), passive(p), edit(e), text(t)
	{}	
	MenuItem(Box b, std::string t, bool p, bool e, V v)
	: box(b), passive(p), edit(e), r_val(v), text(t)
	{}	
public:	
	bool inside(float x, float y) const { return box.inside(x, y); }
	bool is_edit() const                { return edit; }
	bool is_passive() const             { return passive; }
	void set_box(const Box& b)	    { box = b; }
	const Box& get_box() const          { return box; }
	virtual V val()                     { return r_val; }

	// returns true if modifying is finished, false if not
	virtual bool modify(os::event::id::EventID) { return true; };

	virtual int draw_active(bool editing) {
		draw_text_box(box, text, true, passive, editing);
		return 1;
	}
	virtual int draw_inactive() {
		draw_text_box(box, text, false, passive, false);
		return 1;
	}
	virtual ~MenuItem() { }
};

/// MenuLabel, passive non-selectable label
template<class V>
class MenuItemLabel : public MenuItem<V>
{
public:
	MenuItemLabel(Box b, const std::string& s)
	 : MenuItem<V>(b, s, true, false)
	{ }
};

/// Large header text
template<class V>
class MenuItemHeader : public MenuItem<V>
{
public:
	MenuItemHeader(Box b, const std::string& s)
	: MenuItem<V>(b, s, true, false)
	{ }

	virtual int draw_inactive() {
		draw_header(get_box(), text);
		return 1;
	}
};

// Simple selectable textbox that returns a value
template <class V>
class MenuItemRetVal : public MenuItem<V>
{
public:
	MenuItemRetVal(Box b, const std::string& s, V t)
	 : MenuItem<V>(b, s, false, false, t)
	{ }
};

template<class R, class P, class T>
class MenuItemCallback : public MenuItem<R>
{
	P t;
	typedef R (T::*CB)();
	CB cb;
public:
	MenuItemCallback(Box b, const std::string& s, P tt, CB c)
	 : MenuItem<R>(b, s, false, false)
	 , t(tt), cb(c)
	{ }
	virtual R val()
	{
		return (t->*cb)();
	}
};


// Editable string box
template<class V>
class MenuItemStringBox : public MenuItem<V>
{
	std::string* s_val;
public:
	MenuItemStringBox(Box b, std::string* val)
	 : MenuItem<V>(b, *val, false, true), s_val(val)
	{ }

	virtual bool modify(os::event::id::EventID id)
	{
		if (id == os::event::id::Backspace) {
			if(text.size() > 0) {
				text.resize(text.size() - 1);
			}
		} else if (id < 128) {
			text += id;
		} else if (id == os::event::id::Enter) {
			*s_val = text;
			return true;
		} else if (id == os::event::id::Escape) {
			text = *s_val;
			return true;
		}
		return false;
	}
};

/** Menu item that selects one of a number of given items
    V -> dummy return value (not used)
    It -> iterates over std::pair<string, V2>
    V2 -> value that is written upon selection
*/
template <class V, class It, class V2>
class MenuItemMultiChoice : public MenuItem<V>
{
	It begin, current, end;
	V2* val_p;

	// set current to same as value if match is found,
	// otherwise set current to first in list
	void find() {
		current = begin;
		while(current != end && current->second != *val_p) {
			++current;
		}
		if(current == end) {
			current = begin;
		}
		text = current->first;
	}
public:
	
	MenuItemMultiChoice(Box b, It vb, It ve, V2* v) 
	 : MenuItem<V>(b, vb->first, false, true),
	   begin(vb), current(vb), end(ve), val_p(v)
	{ 
		find();
	}

	virtual bool modify(os::event::id::EventID id) {
		if(id == ' ' ||
		   id == os::event::id::Right ||
		   id == os::event::id::Btn1) 
		{
			if(++current == end) {
				current = begin;
			}
		} else if(id == os::event::id::Left) {
			if(current == begin) {
				current = end;
			}
			--current;
		} else if(id == os::event::id::Enter ||
			  id == os::event::id::Btn0) {
			*val_p = current->second;
			return true;
		} else if(id == os::event::id::Escape) {
			find();
			return true;
		}
			
		text   = current->first;
		return false;
	}
};

/// Menu item with two possible choices, updates a boolean variable
template <class V>
class MenuItemTwoChoice : public MenuItem<V>
{
	std::string text2;
	bool* b_val;
public:
	MenuItemTwoChoice(Box b, bool* val, const std::string& on, const std::string& off)
	 : MenuItem<V>(b, on, false, true), text2(off), b_val(val)
	{
		if(!*val) {
			std::swap(text, text2);
		}
	}

	virtual bool modify(os::event::id::EventID id)
	{
		if (id == ' ' ||
		    id == os::event::id::Btn1 ||
		    id == os::event::id::Left ||
		    id == os::event::id::Right) {
			std::swap(text, text2);
			*b_val = !(*b_val);
		} else if (id == os::event::id::Enter 
			   || id == os::event::id::Btn0) {
			return true;
		} 
		return false;
	}
};

typedef error_tmpl<fatal_error_base> menu_error;

/** Show a menu and wait for selection.
    This function takes over the graphics and event systems, until a
    selection is made.
    @param ms     MenuSstem reference
    @param d      default return value used by dummy-items
    @param escape return value when Escape is pressed
    @param mb     Iterator to begin of menuitem list
    @param me     Iterator to end of menuitem list
    @param back   Background texture.
*/
template<class It, class T>
class Menuexec
{
	MenuSystem& ms;
	T def, esc;
	It begin, end, current; // menu items
	Texture &background;
	float x,y; // mouse position
	reaper::debug::DebugOutput derr;
	bool is_editing;
	os::event::Event e;	

	void editing();
	T    mouse_click();
	void select_next();
	void select_prev();
	T    select_current();

	void draw();
	T    process_event();
	void clear_events();
public:
	Menuexec(MenuSystem &ms, T d, T escape, It mb, It ml, Texture& back);

	T run();
};

static const long wait_time = 75;

template<class It, class T>
Menuexec<It, T>::Menuexec(MenuSystem &m, T d, T escape, It mb, It me, Texture& back)
: ms(m), def(d), esc(escape), begin(mb), end(me), current(mb), background(back),
  x(-3.14), y(-3.14), // -3.14 is better than 0
  derr("menu"), is_editing(false)
{
	// move to first non passive entry
	while(!(current == end || !(*current)->is_passive())) {
		++current;
	}
	if(current == end) {
		throw menu_error("All menu choices are passive!");
	}
}


template<class It, class T>
void Menuexec<It, T>::draw()
{
	ms.draw_background(background);
	for(It i = begin; i != current; ++i) {
		(*i)->draw_inactive();
	}
	(*current)->draw_active(is_editing);
	for(It i = current + 1; i != end; ++i) {
		(*i)->draw_inactive();
	}

	ms.draw_mouse_ptr(x, y);
	ms.gfx().update_screen();
}

template<class It, class T>
T Menuexec<It, T>::run()
{
	while(true) {
		draw();

		while (ms.event().get_event(e)) {
			T r = process_event();
			if(r != def) {
				clear_events();
				return r;
			}
		}
		ms.do_stuff();
		os::time::msleep(10);
	}
}

template<class It, class T>
T Menuexec<It, T>::process_event()
{
	using namespace os::event::id;

	if (e.is_axis()) {
		if (e.id == Axis0) {
			x = e.val;
			if(x < 0) x = 0;
			if(x > 1) x = 1;
		}

		if (e.id == Axis1) {
			y = e.val;
			if(y < 0) y = 0;
			if(y > 1) y = 1;
		}
	} else if (is_editing) {
		editing();
	} else {
		switch (e.id) {
		case Btn0:
			return mouse_click();
		case Btn2:
		case Btn3:
		case Enter:
			return select_current();
		case Down:
			select_next();
			break;
		case Up:
			select_prev();
			break;
		case Escape:
		case Backspace:
			ms.press_snd();
			return esc;
		default: 
			break;
		}
	}
	return def;
}

using namespace os::time;

template<class It, class T>
void Menuexec<It, T>::editing()
{
	using namespace os::time;

	static TimeSpan time = get_time() - TimeSpan::from_ms(wait_time*2);

	if((get_time() - time).approx().to_ms() > wait_time) {
		is_editing = !(*current)->modify(e.id);
		if(!is_editing && e.id == os::event::id::Btn0) {			
			mouse_click();
		}
		time = get_time();
	}
}

template<class It, class T>
T Menuexec<It, T>::mouse_click()
{
	for (It i = begin; i != end; ++i) {
		if(!(*i)->is_passive() && (*i)->inside(x,y)) {
			current = i;
			return select_current();
		}
	}
	return def;
}

template<class It, class T>
T Menuexec<It, T>::select_current()
{
	ms.press_snd();
	is_editing = (*current)->is_edit();
	return is_editing ? def : (*current)->val();
}

template<class It, class T>
void Menuexec<It, T>::select_next()
{
	using namespace os::time;
	static TimeSpan time = get_time() - TimeSpan::from_ms(wait_time*2);

	if((get_time() - time).approx().to_ms() > wait_time) {
		ms.sel_snd();						
		do {
			++current;
			if(current == end) {
				current = begin;
			}
		} while ((*current)->is_passive());
		time = get_time();
	}
}

template<class It, class T>
void Menuexec<It, T>::select_prev()
{
	using namespace os::time;
	static TimeSpan time = get_time() - TimeSpan::from_ms(wait_time*2);

	if((get_time() - time).approx().to_ms() > wait_time) {
		ms.sel_snd();
		do {
			if(current == begin) {
				current = end;
			} 
			--current;		
		} while ((*current)->is_passive());
		time = get_time();
	}
}

template<class It, class T>
void Menuexec<It, T>::clear_events()
{
	while (ms.event().get_event(e));
}

}
}

#endif


/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:07 $
 * $Log: menu.h,v $
 * Revision 1.54  2003/01/06 12:42:07  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.53  2002/12/29 17:04:39  pstrand
 * hw/ moved to os/
 *
 * Revision 1.52  2002/09/13 07:45:56  pstrand
 * gcc-varningar
 *
 * Revision 1.51  2002/03/11 19:07:14  pstrand
 * cleanup
 *
 * Revision 1.50  2002/03/11 10:50:49  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.49  2002/02/26 22:33:49  pstrand
 * mackefix
 *
 * Revision 1.48  2002/01/17 05:04:01  peter
 * mp3 i meny (inte igång men funkar), pluginfix..
 *
 * Revision 1.47  2002/01/07 14:00:30  peter
 * resurs och ljudmeck
 *
 * Revision 1.46  2001/12/15 17:47:23  peter
 * menymeck..
 *
 * Revision 1.45  2001/12/14 16:10:50  macke
 * powermenus!
 *
 * Revision 1.44  2001/12/11 22:03:19  macke
 * Profiler refactoring...
 *
 * Revision 1.43  2001/12/11 19:03:28  macke
 * mer bök
 *
 * Revision 1.42  2001/12/11 18:57:22  macke
 * bök
 *
 * Revision 1.41  2001/12/11 18:46:40  macke
 * Menusystem refactored..
 *
 * Revision 1.40  2001/12/08 23:23:54  peter
 * 'spara/fixar/mm'
 *
 * Revision 1.39  2001/12/07 16:25:43  picon
 * Small stringbox fix..
 *
 * Revision 1.38  2001/12/07 16:21:26  picon
 * Menu system quite stable, still ugly
 *
 * Revision 1.37  2001/12/04 23:01:25  picon
 * menu system now working ... barely. :)
 *
 * Revision 1.36  2001/08/20 17:07:52  peter
 * *** empty log message ***
 *
 * Revision 1.35  2001/08/06 12:16:31  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.34.2.1  2001/08/03 13:44:07  peter
 * pragma once obsolete...
 *
 * Revision 1.34  2001/07/27 15:48:36  peter
 * gfx..
 *
 * Revision 1.33  2001/07/06 01:47:26  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.32  2001/05/14 12:49:12  peter
 * *** empty log message ***
 *
 * Revision 1.31  2001/05/13 23:52:41  peter
 * *** empty log message ***
 *
 * Revision 1.30  2001/05/13 07:15:36  peter
 * *** empty log message ***
 *
 *
 */


