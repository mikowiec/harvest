#ifndef WORLD_LEVEL_H
#define WORLD_LEVEL_H

#include "world/exceptions.h"
#include "gfx/gfx_types.h"
#include "main/types.h"

#include <string>
#include <vector>

namespace reaper {
namespace world {

define_reaper_exception(level_error);


/// Fog calculation methods
enum FogType { Linear, Exp, Exp_2 };


class LevelInfo
{
public:
	/// Loads level info from specified file
	LevelInfo(const std::string &levelfile);
//	LevelInfo() {}

	std::string name;
	/// Saves level info
	void save(const std::string &file) const;


	/// Lakes that exists in level
	struct Lake
	{
		std::string mesh;
		std::string texture;
		Vector wave_dir;
		float amplitude;
	};

	std::vector<Lake> lakes;

	/**@name Sky parameters */
	//@{      
		/// Sky color (not used if sky_texture is valid)
		gfx::Color sky_color;
		/// Sky texture file name
		std::string sky_texture;
		/// Sky altitude
		float sky_altitude;
		/// Texture 'size' (number of repeats across a whole sphere)
		float sky_width;
	//@}

	/**@name Fog parameters */
	//@{
		/// Fog color
		gfx::Color fog_color;
		/// Fog length (fraction of view distance) use with linear fog
		float fog_length;
		/// Fog density (magic number) use with exponential fog
		float fog_density;
		/// Fog type
		FogType fog_type;
	//@}

	/**@name Terrain */
	//@{
		/// Geometry data file
		std::string terrain_mesh; 
		/// Main texture file
		std::string terrain_main_texture;
		/// Detail texture file
		std::string terrain_detail_texture;
		/// Detail texture size (meters)
		float terrain_detail_size;

		/// Terrain size (x-coord)
		float terrain_max_x;
		float terrain_min_x;

		/// Terrain size (z-coord)
		float terrain_max_z;
		float terrain_min_z;
	//@}

	std::vector<std::string> objectgroups;
	std::string scenario;
};

/// Returns level information
const LevelInfo& get_level_info();

}
}

#endif


/*
 * $Author: pstrand $
 * $Date: 2002/04/15 16:15:55 $
 * $Log: level.h,v $
 * Revision 1.16  2002/04/15 16:15:55  pstrand
 * linjesökningsfix
 *
 * Revision 1.15  2002/04/08 21:10:09  pstrand
 * fog-typer lokaliserade...
 *
 * Revision 1.14  2002/04/08 21:01:54  pstrand
 * fog-typer lokaliserade...
 *
 * Revision 1.13  2002/03/18 13:34:08  pstrand
 * rect search & level fix
 *
 * Revision 1.12  2002/02/27 18:08:42  pstrand
 * release-0.96
 *
 * Revision 1.11  2002/02/19 21:55:02  peter
 * *** empty log message ***
 *
 * Revision 1.10  2002/02/18 11:52:06  peter
 * obj.ladd. och reorg.
 *
 * Revision 1.9  2002/01/01 23:32:22  peter
 * alloc_id och level
 *
 * Revision 1.8  2001/12/02 17:40:49  peter
 * objmgrmeck
 *
 * Revision 1.7  2001/08/06 12:16:45  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.6.2.1  2001/08/03 13:44:16  peter
 * pragma once obsolete...
 *
 * Revision 1.6  2001/07/30 23:43:35  macke
 * Häpp, då var det kört.
 *
 * Revision 1.5  2001/07/06 01:47:38  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.4  2001/05/05 03:15:28  macke
 * nolla världspekaren vid destruktion, samt konstruktion via ref till levelinfo..
 *
 * Revision 1.3  2001/04/24 23:21:46  macke
 * Vattenytor... och bättre profiling på grafikmotorn..
 *
 * Revision 1.2  2001/04/08 23:34:47  macke
 * Ljusinläsning och allmänt städ..
 *
 * Revision 1.1  2001/04/05 10:05:12  peter
 * LevelInfo
 *
 */

