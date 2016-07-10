
#include "os/compat.h"

#include "world/level.h"
#include "gfx/io.h"

#include "res/res.h"

#include <functional>
#include <iostream>

#include "misc/parse.h"
#include "res/config.h"
#include "res/resource.h"
#include "os/debug.h"


namespace reaper {
namespace {
reaper::debug::DebugOutput dout("world::level",5);
}
namespace world {

using namespace std;
using namespace res;
using gfx::Color;

class LevelCreate : public NodeConfig<ConfigEnv>
{
	Ptr load(IdentRef id) {
		return resource_ptr<ConfigEnv>("levels/" + id);
	}

public:
	Ptr create(IdentRef id) {
		Ptr p = load(id + ".rl");
		if (p->empty())
			p = load(id);
		return p->empty() ? Ptr(0) : p;
	}
};


LevelInfo::LevelInfo(const string &n)
 : name(n)
{
	try {
		const ConfigEnv& env = resource<ConfigEnv>("levels/" + n + ".rl");

		if (env.empty())
			throw resource_not_found("level");

		sky_texture  = env["SkyTexture"].str();
		sky_color    = read<Color>(env["SkyColor"]);
		sky_altitude = env["SkyAltitude"];
		sky_width    = env["SkyDensity"];
		
		fog_color    = read<Color>(env["FogColor"]);
		fog_length   = env["FogLength"];
		fog_density  = env["FogDensity"];
		string ftype = env["FogType"];
		if (ftype == "LINEAR")
			fog_type = Linear;
		else if (ftype == "EXP")
			fog_type = Exp;
		else if (ftype == "EXP2")
			fog_type = Exp_2;
		else
			throw level_error("invalid fog type: " + ftype);
		
		terrain_mesh           = env["TerrainMesh"].str();
		terrain_main_texture   = env["TerrainMainTexture"].str();
		terrain_detail_texture = env["TerrainDetailTexture"].str();
		terrain_detail_size    = env["TerrainDetailRepeats"];
		dout << terrain_main_texture << ' ' << terrain_detail_texture << '\n';
		for (int i = 0;; ++i) {
			string pfx = "lake" + misc::ltos(i) + "_";
			Lake lake;
			lake.mesh     = env[pfx+"LakeMesh"].str();
			if (lake.mesh.empty())
				break;
			lake.texture  = env[pfx+"LakeTexture"].str();
			lake.wave_dir = env[pfx+"LakeAmplitude"];
			lake.amplitude = 0; // FIXME ??? (amp vs. wave_dir)
			lakes.push_back(lake);
		}

		terrain_min_x = env["TerrainMinX"];
		terrain_min_z = env["TerrainMinZ"];
		terrain_max_x = env["TerrainMaxX"];
		terrain_max_z = env["TerrainMaxZ"];

		std::string ogs = env["ObjectGroups"];
		misc::split(ogs, std::back_inserter(objectgroups));
		scenario = env["Scenario"].str();
	}
	catch (const resource_not_found& e) {
		throw level_error(string("LevelInfo: ") + name + " not found!  " + e.what());
	}
}

void LevelInfo::save(const std::string &file) const
{
	res_out_stream os(Level,file,true);

	os << "# Reaper level file\n\n";

	os << "sky_texture:  " << sky_texture << '\n';
	os << "sky_color:    " << sky_color << '\n';
	os << "sky_altitude: " << sky_altitude << '\n';
	os << "sky_width:    " << sky_width << "\n\n";

	os << "fog_color:   " << fog_color << '\n';
	os << "fog_length:  " << fog_length << '\n';
	os << "fog_density: " << fog_density << '\n';
	os << "fog_type:    ";
	
	switch(fog_type) {
	case Linear: os << "LINEAR\n\n"; break;
	case Exp:    os << "EXP\n\n"; break;
	case Exp_2:  os << "EXP2\n\n"; break;
	}

	os << "terrain_mesh:          " << terrain_mesh << '\n'; 
	os << "terrain_main_texture   " << terrain_main_texture << '\n';
	os << "terrain_detail_texture " << terrain_detail_texture << '\n';
	os << "terrain_detail_size    " << terrain_detail_size << "\n\n";

	for(std::vector<Lake>::const_iterator i = lakes.begin(); i != lakes.end(); ++i) {
		os << "begin_lake\n";
		os << "\tlake_mesh:      " << i->mesh << '\n';
		os << "\tlake_texture:   " << i->texture << '\n';
		os << "\tlake-wave_dir:  " << i->wave_dir << '\n';
		os << "\tlake_amplitude: " << i->amplitude << '\n';
	}
}

}
}

