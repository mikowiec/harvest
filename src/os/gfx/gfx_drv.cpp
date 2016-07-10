

#include "os/compat.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>

#include "os/debug.h"

#include "os/gfx.h"
#include "os/gfx/gfx_types.h"
#include "os/gfx/gfx_driver.h"
#include "os/exceptions.h"
#include "os/interfaces.h"
#include "os/gfx/gfxplugin.h"
#include "os/gl.h"
#include "os/gl/gl_info.h"
#include "os/osinfo.h"

#include "res/config.h"

#include "misc/sequence.h"
#include "misc/parse.h"

#ifdef MONOLITHIC
extern "C" reaper::os::lowlevel::Gfx_driver* create_gfx_x11(reaper::os::ifs::Gfx* m);
#endif

namespace reaper
{
namespace os
{
namespace gfx
{
using std::string;

namespace { debug::DebugOutput derr("os::gfx"); }

std::ostream& operator<<(std::ostream& os , const VideoMode& m)
{
	os << m.width << 'x' << m.height
	   << (m.fullscreen ? " fullscreen" : "");
	return os;
}




/* Interface object for the plugin */

class Main
 : public ifs::Gfx
{
	debug::DebugOutput dr;
public:
	Main() : dr("gfx_plugin", 5)
	{ }
	debug::DebugOutput& derr() { return dr; }
	std::string config(const std::string& lbl) {
		res::ConfigEnv env("hw_gfx");
		return env[lbl];
	}

};

struct Gfx_data
{
	GfxPlugin* gfx_plugin;
//	std::set<VideoMode> modes;
	Main* main;
	res::ConfigEnv current_config;
	VideoMode current_mode;

	Gfx_data()
	 : gfx_plugin(new GfxPlugin()), main(new Main())
	{ }
};

VideoMode Gfx::default_mode;

Gfx::Gfx(const VideoMode& vm)
 : data(new Gfx_data()), driver(0)
{
	if (!change_mode(vm))
		throw hw_fatal_error("Initialization failed");
}

Gfx::Gfx() : data(new Gfx_data()), driver(0)
{
	change_mode();
/*
	if (! driver->internal_data()->is_accelerated || bool(cnf["noaccel"])) {
		vm = VideoMode(400, 300);
		config_mode(vm, cnf["noaccel_width"], cnf["noaccel_height"], cnf["noaccel_fullscreen"]);
		change_mode(vm);
	}
	*/
}

Gfx::~Gfx()
{
	driver->restore_mode();
	delete driver;
	delete data->gfx_plugin;
	delete data->main;
	delete data;
}

/*
void Gfx::reinit()
{
	res::ConfigEnv cnf("hw_gfx");
	string pfx("gfx_");
	string sub(cnf["subsystem"]);

	if (sub.empty()) {
		if (os_name() == "win32")
			sub = "win32";
		else
			sub = "x11";
	}
	data->main = new Main(data->modes);

	data->gfx_plugin = new GfxPlugin();
	driver = data->gfx_plugin->create(pfx+sub, data->main);

	if (driver == 0)
		throw hw_error(string("Failed to create gfx subsystem."));

	VideoMode vm(800, 600);

	res::ConfigEnv cnf("hw_gfx");
	vm.fullscreen = cnf["fullscreen"];
	int width = cnf["width"];
	int height = cnf["height"];
	if (width > 0 && height > 0) {
		vm.width = width;
		vm.height = height;
	}
	
	std::set<VideoMode>::iterator i = misc::find(misc::seq(data->modes), vm);
	if (i != data->modes.end())
		vm = *i;

	if (!change_mode(vm))
		throw hw_fatal_error("Initialization failed");
}
*/

bool sane_mode(const VideoMode& vm)
{
	return vm.width > 0 && vm.height > 0;
}

bool Gfx::change_mode(const VideoMode& mode)
{
	res::ConfigEnv old_cnf = data->current_config;
	res::ConfigEnv new_cnf("hw_gfx");
	VideoMode old_mode = data->current_mode;
	VideoMode new_mode = mode;

	if (!sane_mode(new_mode)) {
		// Defaulting to config files
		new_mode = VideoMode(new_cnf["width"],
				     new_cnf["height"],
				     new_cnf["fullscreen"]);
		if (! sane_mode(new_mode)) {
			new_mode = VideoMode(800, 600, false);
		}

	}
	bool dirty = false;


	string sub(new_cnf["subsystem"]);
	if (sub.empty()) {
		if (os_name() == "win32")
			sub = "win32";
		else
			sub = "x11";
	}
	if (sub != string(old_cnf["subsystem"])) {
		delete driver;
		string pfx("gfx_");
		driver = data->gfx_plugin->create(pfx+sub, data->main);
		if (!driver)
			throw hw_error(string("Failed to load gfx subsystem: ") + sub);
		dirty = true;
	}


	if (new_mode.fullscreen) {
		std::set<VideoMode> s;
		misc::fill_from_iter(driver->modes(), s);
		typedef std::set<VideoMode>::iterator mode_iter;
		mode_iter i = misc::find(misc::seq(s), new_mode);
		if (i == s.end()) {
			derr << "Could not find suitable fullscreen mode,"
			     << "defaulting to window!\n";
			new_mode.fullscreen = false;
		}
	}

	dirty |= !(new_mode == old_mode);


	bool res = !dirty;
	if (dirty) {
		res = driver->setup_mode(new_mode);
		if (res)
			os::gfx::reinit_opengl();
	}
	derr << "New videomode: " << new_mode;
	if(alpha())
		derr << " alpha";
	if(stencil())
		derr << " stencil";
	derr << "\n";
	data->current_config = new_cnf;
	data->current_mode = new_mode;
	return res;
}


bool Gfx::update_screen()
{
	bool rz = driver->update_screen();
	data->current_mode = current_mode();
	return rz;
}

const VideoMode& Gfx::current_mode() const
{
	return driver->internal_data()->current;
}

bool Gfx::is_accelerated() const
{
	return driver->internal_data()->is_accelerated;
}

bool Gfx::alpha() const
{
	return driver->internal_data()->alpha;
}

bool Gfx::stencil() const
{
	return driver->internal_data()->stencil;
}



}
}
}

