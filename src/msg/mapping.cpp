
/* $Id: mapping.cpp,v 1.3 2003/06/04 16:36:32 pstrand Exp $  */

#include "os/compat.h"

#include <string>
#include <iostream>

#include "msg/event.h"
#include "msg/mapping.h"
#include "misc/parse.h"
#include "res/config.h"
#include "os/debug.h"

#include <vector>

namespace reaper {
namespace os {
namespace event {

namespace {
	debug::DebugOutput derr("mapping");
	debug::DebugOutput derr2("mapping (event)", 5);
}


typedef std::map<std::string, id::EventID> KeyMap;

KeyMap mk_keymap()
{
	KeyMap km;
	for (char c = '0'; c != 127; c++) {
		char buf[6] = {c, 0};
		km[buf] = id::mk(c);
	}
	km["Space"] = id::mk(' ');
	km["Left"] = id::Left;
	km["Right"] = id::Right;
	km["Up"] = id::Up;
	km["Down"] = id::Down;
	km["Enter"] = id::Enter;
	km["Escape"] = id::Escape;
	km["Home"] = id::Home;
	km["End"] = id::End;
	km["PageUp"] = id::PageUp;
	km["PageDown"] = id::PageDown;
	km["Insert"] = id::Insert;
	km["Delete"] = id::Delete;
	km["F1"] = id::F1;
	km["F2"] = id::F2;
	km["F3"] = id::F3;
	km["F4"] = id::F4;
	km["F5"] = id::F5;
	km["F6"] = id::F6;
	km["F7"] = id::F7;
	km["F8"] = id::F8;
	km["F9"] = id::F9;
	km["F10"] = id::F10;
	km["F11"] = id::F11;
	km["F12"] = id::F12;
	return km;
}

id::EventID str2key(const std::string& s)
{
	static KeyMap map = mk_keymap();
	return map[s];
}

id::EventID str2id(const std::string& s)
{
	misc::stringpair k = misc::split(s, ' ');
	if (k.first == "key")
		return str2key(k.second);
	return id::mk(((k.first == "btn") ? id::Btn0 : id::Axis0)	
			+ misc::stoi(k.second));
}

class GameMap
 : public reaper::os::event::Mapping
{
	struct To {
		enum Op { None, Rel, Abs, Pass, Rev, Repeat };
		id::EventID id;
		Receiver player;
		Op op;
		float val;
		To()
		 : id(id::Key), player(false), op(None), val(0.0)
		{ }

		To(id::EventID k, Receiver p, Op o, float v)
		 : id(k), player(p), op(o), val(v)
		{ }

		To(const std::string& ply, const std::string& to)
		{
			if (ply == "system")
				player = 0;
			else if (ply == "player")
				player = 1;
			else if (ply == "player2")
				player = 2;
			std::vector<std::string> tov;
			if (misc::split(to, std::back_inserter(tov), 4) < 2)
				return;
			val = 0.0;
			op = Pass;
			if (tov[0] == "axis" && tov.size() == 3) {
				id = id::mk(id::Axis0 + misc::stoi(tov[1]));
				val = misc::stof(tov[2].c_str() + 1);
				switch (tov[2][0]) {
				case '=': op = Abs; break;
				case '-': val = -val;
				case '+': op = Rel; break;
				case 'n': op = Pass; break;
				case 'r': op = Rev; break;
				default: derr << "ERROR!\n"; break;
				}
			} else if (tov[0] == "key" && tov.size() >= 2) {
				id = str2key(tov[1]);
				if (tov.size() == 3 && tov[2][0] == 'r') {
					op = Repeat;
				}
			} else if (tov[0] == "btn") {
				id = id::mk(id::Btn0 + misc::stoi(tov[1]));
			} else {
				id = id::Key;
				derr << "To mapping parse error: "
				     << ply << " - " << to << '\n';
			}
		}
	};
	std::map<id::EventID, To> emap;
	std::map<id::EventID, float> values;
public:
	GameMap(const std::string& name)
	{
		res::ConfigEnv env(name);
		res::ConfigEnv::Env::const_iterator c, e = env.end();
		for (c = env.begin(); c != e; ++c) {
			misc::stringpair id = misc::split(c->first, '_');
			emap[str2id(id.second)] = To(id.first, c->second);
		}
	}
	Event operator()(const Event& ev)
	{
		Event e(ev);
		To to = emap[ev.id];
//		derr2 << "Incoming: " << e << " ->  ";
		if (to.op == To::None) {
//			derr2 << "passthrough\n";
			e.recv = Player;
			return e;
		}

		e.id = to.id;
		if (to.player)
			e.recv = to.player;
		if (to.id < id::Axis0) {
			e.val = (e.val <= 0.0) ? -1.0 : 1.0;
			if (to.op != To::Repeat && values[to.id] + e.val > 1.0)
				e.id = id::Unknown;
			values[to.id] = e.val;
//			derr2 << e << '\n';
			return e;
		}
		float val = values[to.id];

		switch (to.op) {
		case To::Rel:  val += to.val; break;
		case To::Abs:  val = to.val; break;
		case To::Pass: val = e.val; break;
		case To::Rev:  val = -e.val; break;
		default: derr << "err in gamemap, unexpected op: " << to.op << '\n'; break;
		}
		if (val >  1.0) val =  1.0;
		if (val < -1.0) val = -1.0;
		e.val = val;
		values[to.id] = val;
//		derr2 << e << '\n';
		return e;
	}
	bool is_repeat(const Event& e)
	{
		To to = emap[e.id];
		return to.op == To::Repeat
		    || (e.id < id::Axis0 && to.id >= id::Axis0);
	}
};


Mapping* create_game_map(const std::string& name)
{
	return new GameMap(name);
}


}
}
}
