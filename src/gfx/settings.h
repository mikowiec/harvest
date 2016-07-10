#ifndef REAPER_GFX_SETTINGS_H
#define REAPER_GFX_SETTINGS_H

namespace reaper {
namespace gfx {

/// Holds all settings for the graphics renderer
class Settings
{
public:
	int hud_type;		
	float radar_range;

	int draw_stats;

	float terrain_detail;
	float object_detail;
	float maximum_view_distance;

	int shadow_type;
	float environment_mapping;

	bool test_for_errors;
	int texture_level;
	bool use_lighting;
	bool use_fog;
	int  texture_scaling;
	bool texture_caching;

	bool draw_sky;
	bool draw_terrain;
	bool draw_water;
	bool draw_effects;
	bool draw_shadows;
	bool draw_lights;

	int alpha_bits;
	int stencil_bits;

	bool use_arb_multitexturing;
	bool use_arb_texture_compression;
	bool use_arb_texture_env_combine;
	bool use_arb_texture_cube_map;
        bool use_arb_texture_border_clamp;
	bool use_ext_compiled_vertex_array;
	bool use_ext_s3tc_texture_compression;
	bool use_ext_separate_specular_color;
	bool use_ext_secondary_color;
	bool use_nv_fence;
	bool use_nv_vertex_array_range;
	bool use_nv_vertex_array_range2;
	bool use_nv_fog_distance;
	bool use_nv_register_combiners;
	bool use_sgis_generate_mipmap;

	bool use_opengl_1_2;

	static       Settings current;
	static const Settings defaults;
	static const Settings defaults_using_extensions;

	static Settings read(bool use_extensions = true);
	static void read(Settings &);
	static void write(const Settings &);

	Settings(bool use_extensions = false);
};

void init_gl_settings(Settings& s);


}
}
#endif

/*
 * $Author: fizzgig $
 * $Date: 2002/09/23 11:48:58 $
 * $Log: settings.h,v $
 * Revision 1.31  2002/09/23 11:48:58  fizzgig
 * Build/Link cleanup
 *
 * Revision 1.30  2002/09/20 23:46:59  pstrand
 * use_texturing->texture_level, minor event fix.
 *
 * Revision 1.29  2002/09/10 17:36:09  fizzgig
 * Misc fixes
 *
 * Revision 1.28  2002/06/06 07:59:38  pstrand
 * gl init, settings, fix
 *
 * Revision 1.27  2002/05/21 03:07:51  fizzgig
 * envmap update options (gfx_renderer::environment_mapping: 0->off 1->incremental 2->instant)
 * preliminary support for shadow volumes (now requests stencil buffer as well)
 *
 * Revision 1.26  2002/05/19 17:36:49  fizzgig
 * glh-lib added (for extensions detect & init)
 * nvparse added
 * extgen added
 * texture blending example added
 *
 * Revision 1.25  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.24  2002/02/07 00:02:51  macke
 * STLPort 4.5.1 Win32-fix + shadow & render reorg
 *
 * Revision 1.23  2002/01/08 19:40:21  macke
 * Texturdetaljnivå, samt lite småfix..
 *
 * Revision 1.22  2002/01/07 14:00:26  peter
 * resurs och ljudmeck
 *
 * Revision 1.21  2001/12/17 16:28:33  macke
 * div bök
 *
 * Revision 1.20  2001/12/04 23:01:24  picon
 * menu system now working ... barely. :)
 *
 * Revision 1.19  2001/09/24 02:33:25  macke
 * Meckat lite med fulbuggen i grafikmotorn.. verkar funka att deallokera nu.. ?
 *
 * Revision 1.18  2001/08/06 12:16:16  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.17.4.1  2001/08/03 13:43:54  peter
 * pragma once obsolete...
 *
 * Revision 1.17  2001/07/06 01:47:14  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.16  2001/06/09 01:58:52  macke
 * Grafikmotor reorg
 *
 * Revision 1.15  2001/06/07 05:14:20  macke
 * Reaper v0.9
 *
 * Revision 1.14  2001/05/14 20:00:53  macke
 * bugfix och rök på missiler..
 *
 * Revision 1.13  2001/05/10 11:40:16  macke
 * häpp
 *
 */

