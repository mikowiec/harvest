
#include "os/compat.h"

#include "os/debug.h"

#include "game/scenario_mgr.h"
#include "os/gl.h"
#include "misc/font.h"
#include "misc/free.h"
#include "misc/parse.h"
#include "res/config.h"
#include "res/resource.h"
#include "game/mission.h"

namespace reaper {
namespace misc {
	UniquePtr<game::scenario::ScenarioMgr>::I UniquePtr<game::scenario::ScenarioMgr>::inst;
}


namespace game {
namespace scenario {	
debug::DebugOutput dout("scenario");

	ScenarioMgr::ScenarioMgr()
	{
		current = "start";

		time = 0.0;
		draw_flag = false;
		drawtime = 0.0;
		count = 0;
	}

	void ScenarioMgr::init(const std::string& scen)
	{
		scenario = res::resource_ptr<Scenario>(scen);

		Missions::iterator c, e = scenario->missions.end();
		for (c = scenario->missions.begin(); c != e; ++c) {
			states[c->first] = new MissionState(c->second, om, messages);
		}
	}

	ScenarioMgr::~ScenarioMgr(void)
	{
		shutdown();

//		ScenarioRef::destroy();
		dout << "manager destroyed" << endl;
	}

	void ScenarioMgr::update(double timediff)
	{
		time += timediff;

		if (current != "end") {
			std::string next = states[current]->update(time);
			if (next != current) {
				states[current]->on_exit();
				dout << "changing states: " << current << " -> " << next << '\n';
				current = next;
				if (current != "end") {
					std::string msg = states[current]->on_enter();
					if (!msg.empty())
						draw_file(msg);
				}
			}
		}
			
	//	om.think();
	}

	void ScenarioMgr::receive(Message msg)
	{
		messages.push(msg);
	}

	void ScenarioMgr::set_info(std::string& i)
	{
		info = i;
		drawtime = time;
		draw_flag = true;
	}

	void ScenarioMgr::draw_file(const std::string& name)
	{
		std::string info;

		try {
			res::res_stream is(res::Scenario, name, res::throw_on_error);

			while (!is.eof()) {
				std::string junk;
				getline(is, junk);
				info += junk+"\n";
			}
		} catch (res::resource_not_found) {
			info = "Dialog \"" + name + "\" missing!";
		}

		set_info(info);
	}

	void ScenarioMgr::draw_info(void)
	{
		if(draw_flag){
			using gfx::font::glPutStr;
			
			StateKeeper s1(GL_DEPTH_TEST, false);
			StateKeeper s2(GL_LIGHTING, false);
			StateKeeper s3(GL_BLEND, true);

			glColor4f(1,1,1,1);
			glPushMatrix();
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(0,1,0,80);
			glMatrixMode(GL_MODELVIEW);
			
			float y=66;
			std::string temp = info;
			std::string::size_type pos;

			while(!temp.empty()) {
				pos = temp.find_first_of("\n");
				glPutStr(0.13, y, temp.substr(0,pos), gfx::font::Medium, 0.0090, 2.0);
				temp.erase(0, pos+1);
				y-=2.5;
			}

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		
			if((time-drawtime) > 20.0) // Display for 20 sec
				draw_flag = false;
		}
	}

	void ScenarioMgr::shutdown()
	{
		om.shutdown();
		misc::for_each(misc::seq(states), misc::delete_it);
		states.clear();
	}


	void ScenarioMgr::get_objectnames(std::set<std::string>& s)
	{
		om.get_objectnames(s);
	}
}
}
}

