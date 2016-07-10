
#include "os/compat.h"

#include <stdio.h>

#include "os/gfx.h"
#include "os/time.h"
#include "os/debug.h"

#include "main/types.h"

#include "gfx/gfx.h"
#include "gfx/managers.h"

#include "phys/phys.h"
#include "game/scenario_mgr.h"
#include "res/resource.h"
#include "snd/sound_system.h"

#include "world/world.h"

#include "misc/test_main.h"

namespace reaper {
namespace object {
	int get_obj_count();
}
}
using namespace reaper;


int test_main()
{
	derr << "os::gfx begin\n";
	{
		os::gfx::Gfx gx;
	}
	derr << "---\n";
	{
		os::gfx::Gfx gx;
	}
	derr << "os::gfx end\n";

	derr << "World begin\n";
	{
		world::WorldRef wr = world::World::create();
		wr->load("test_level");
	}	
	derr << "---\n";
	{
		world::WorldRef wr = world::World::create();
		wr->load("test_level");
	}	
	derr << "World end\n";


	derr << "Renderer begin\n";
	{
		os::gfx::Gfx gx;
		world::WorldRef wr = world::World::create();
		wr->load("test_level");
		gfx::RendererRef rr = gfx::Renderer::create();
	}
	derr << "---\n";
	{
		os::gfx::Gfx gx;
		world::WorldRef wr = world::World::create();
		wr->load("test_level");
		gfx::RendererRef rr = gfx::Renderer::create();
	}
	derr << "Renderer end\n";

	derr << "Scenario begin\n";
	{
		os::gfx::Gfx gx;
		world::WorldRef wr = world::World::create();
		wr->load("test_level");
		phys::PhysRef pr = phys::Engine::create();
		gfx::RendererRef rr = gfx::Renderer::create();
		rr->load();
		gfx::MeshRef mr = gfx::MeshMgr::create();
		sound::SoundRef sm = sound::Manager::create();
		game::scenario::ScenarioRef sr = game::scenario::ScenarioMgr::create();
		sr->update(10);
		pr->update_world(0.01, 0.01);
		sr->shutdown();
		pr->shutdown();
		sm->shutdown();
		wr->shutdown();
	}
	derr << "Objs: " << object::get_obj_count() << '\n';
	derr << "Objs: " << object::get_obj_count() << '\n';
	derr << "Scenario end\n";
	derr << "Scenario begin 2\n";
	{
		os::gfx::Gfx gx;
		world::WorldRef wr = world::World::create();
		wr->load("test_level");
		phys::PhysRef pr = phys::Engine::create();
		gfx::RendererRef rr = gfx::Renderer::create();
		rr->load();
		rr->start();
		gfx::MeshRef mr = gfx::MeshMgr::create();
		sound::SoundRef sm = sound::Manager::create();
		game::scenario::ScenarioRef sr = game::scenario::ScenarioMgr::create();
		sr->update(10);
		sr->restart();
		sr->update(10);
		wr->shutdown();
		
	}
	derr << "Scenario end 2\n";
	derr << "Objs: " << object::get_obj_count() << '\n';

	derr << "Phys&scen begin\n";
	{
		os::gfx::Gfx gx;
		world::WorldRef wr = world::World::create();
		wr->load("test_level");
		phys::PhysRef pr = phys::Engine::create();
		gfx::RendererRef rr = gfx::Renderer::create();
		rr->load();
		rr->start();
		gfx::MeshRef mr = gfx::MeshMgr::create();
		sound::SoundRef sm = sound::Manager::create();
		game::scenario::ScenarioRef sr = game::scenario::ScenarioMgr::create();
		sr->update(10);
		pr->update_world(10, 20);
		wr->update();
		sr->update(10);
		pr->update_world(30, 50);
		wr->update();
		wr->shutdown();
		
	}
	derr << "Phys&scen end 2\n";
	derr << "Objs: " << object::get_obj_count() << '\n';

	res::mgr().flush();
	return 0;
}

