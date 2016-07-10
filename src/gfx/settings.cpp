#include "os/compat.h"

#include "gfx/gfx.h"
#include "gfx/settings.h"
#include "res/config.h"
#include "res/resource.h"

#include "os/gl.h"
#include "os/gl/gl_info.h"
#include "os/debug.h"

namespace reaper
{
namespace 
{
debug::DebugOutput dout("gfx::settings");
}

namespace gfx
{

Settings::Settings(bool use_extensions) :
	// Non-persistent data
	hud_type(None_HUD),		
	radar_range(2500),

	// Settings
	draw_stats(0),
	terrain_detail(1),
	object_detail(1),
	maximum_view_distance(5000),

	shadow_type(1),
	environment_mapping(1),

	test_for_errors(false),
	texture_level(2),
	use_lighting(true),
	use_fog(true),
	texture_scaling(1),
	texture_caching(true),

	// Rendering
	draw_sky(true),
	draw_terrain(true),
	draw_water(true),
	draw_effects(true),
	draw_shadows(true),
	draw_lights(true),

	alpha_bits(0),
	stencil_bits(0),

	// Extensions
	use_arb_multitexturing(use_extensions),
	use_arb_texture_compression(use_extensions),
	use_arb_texture_env_combine(use_extensions),
	use_arb_texture_cube_map(use_extensions),
        use_arb_texture_border_clamp(use_extensions),
	use_ext_compiled_vertex_array(false),
	use_ext_s3tc_texture_compression(use_extensions),
	use_ext_separate_specular_color(use_extensions),
	use_ext_secondary_color(use_extensions),
	use_nv_fence(false),
	use_nv_vertex_array_range(use_extensions),
	use_nv_vertex_array_range2(use_extensions),
	use_nv_fog_distance(use_extensions),
	use_nv_register_combiners(use_extensions),
	use_sgis_generate_mipmap(use_extensions),

	use_opengl_1_2(true)
{}

Settings Settings::current(false);
const Settings Settings::defaults(false);
const Settings Settings::defaults_using_extensions(true);

Settings Settings::read(bool use_extensions)
{
	Settings s(use_extensions);
	read(s);
	return s;
}

#define READ_CONF(x) s.x = res::withdefault(env, pfx+'_'+#x, s.x)

void Settings::read(Settings &s)
{
	const res::ConfigEnv& env = res::resource<res::ConfigEnv>("gfx_renderer");

	std::string pfx = "settings";
	
	READ_CONF(draw_stats);
	READ_CONF(terrain_detail);
	READ_CONF(object_detail);
	READ_CONF(maximum_view_distance);
	READ_CONF(shadow_type);
	READ_CONF(environment_mapping);

	READ_CONF(test_for_errors);
	READ_CONF(texture_level);
	READ_CONF(use_lighting);
	READ_CONF(use_fog);
	READ_CONF(texture_scaling);
	READ_CONF(texture_caching);

	pfx = "rendering";

	READ_CONF(draw_sky);
	READ_CONF(draw_terrain);
	READ_CONF(draw_water);
	READ_CONF(draw_effects);
	READ_CONF(draw_shadows);
	READ_CONF(draw_lights);

	pfx = "extensions";

	READ_CONF(use_arb_multitexturing);
	READ_CONF(use_arb_texture_compression);
	READ_CONF(use_arb_texture_env_combine);
	READ_CONF(use_arb_texture_cube_map);
	READ_CONF(use_arb_texture_border_clamp);
	//READ_CONF(use_ext_compiled_vertex_array);
	READ_CONF(use_ext_s3tc_texture_compression);
	READ_CONF(use_ext_separate_specular_color);
	READ_CONF(use_ext_secondary_color);
	//READ_CONF(use_nv_fence);
	READ_CONF(use_nv_vertex_array_range);
	READ_CONF(use_nv_vertex_array_range2);
	READ_CONF(use_nv_fog_distance);
	READ_CONF(use_nv_register_combiners);
	READ_CONF(use_sgis_generate_mipmap);
	READ_CONF(use_opengl_1_2);

	s.use_nv_fence = false;
	s.use_ext_compiled_vertex_array = false;

}

#define WRITE_CONF(x) env[pfx+"_"+#x] = s.x;

void Settings::write(const Settings &s)
{
	res::ConfigEnv& env = res::resource<res::ConfigEnv>("gfx_renderer");

	std::string pfx = "settings";

	WRITE_CONF(draw_stats);
	WRITE_CONF(terrain_detail);
	WRITE_CONF(object_detail);
	WRITE_CONF(maximum_view_distance);
	WRITE_CONF(shadow_type);
	WRITE_CONF(environment_mapping);

	WRITE_CONF(test_for_errors);
	WRITE_CONF(texture_level);
	WRITE_CONF(use_lighting);
	WRITE_CONF(use_fog);
	WRITE_CONF(texture_scaling);
	WRITE_CONF(texture_caching);

	pfx = "rendering";

	WRITE_CONF(draw_sky);
	WRITE_CONF(draw_terrain);
	WRITE_CONF(draw_water);
	WRITE_CONF(draw_effects);
	WRITE_CONF(draw_shadows);
	WRITE_CONF(draw_lights);

	pfx = "extensions";

	WRITE_CONF(use_arb_multitexturing);
	WRITE_CONF(use_arb_texture_compression);
	WRITE_CONF(use_arb_texture_env_combine);
	WRITE_CONF(use_arb_texture_cube_map);	
	WRITE_CONF(use_arb_texture_border_clamp);	
	//WRITE_CONF(use_ext_compiled_vertex_array);
	WRITE_CONF(use_ext_s3tc_texture_compression);
	WRITE_CONF(use_ext_separate_specular_color);
	WRITE_CONF(use_ext_secondary_color);	
	//WRITE_CONF(use_nv_fence);
	WRITE_CONF(use_nv_vertex_array_range);
	WRITE_CONF(use_nv_vertex_array_range2);
	WRITE_CONF(use_nv_fog_distance);
	WRITE_CONF(use_nv_register_combiners);
	WRITE_CONF(use_sgis_generate_mipmap);	
	WRITE_CONF(use_opengl_1_2);

	res::save<res::ConfigEnv>("gfx_renderer");

}

void init_gl_settings(Settings& s)
{
	s = Settings::read(true);
	
	if(int err = glGetError() != GL_NO_ERROR) {
		throw reaper_error(std::string("enable_extensions begin:") + (char*)gluErrorString(err));
	}
        
	const os::gfx::OpenGLinfo& gl = os::gfx::opengl_info();

        s.alpha_bits = gl.info().alpha_bits;
        s.stencil_bits = gl.info().stencil_bits;

	typedef std::map<std::string, bool*> ExtCont;
	ExtCont exts;

	exts["GL_ARB_multitexture"]             = &s.use_arb_multitexturing;
	exts["GL_ARB_texture_compression"]      = &s.use_arb_texture_compression;
	exts["GL_ARB_texture_env_combine"]      = &s.use_arb_texture_env_combine;
	exts["GL_ARB_texture_cube_map"]         = &s.use_arb_texture_cube_map;
	exts["GL_ARB_texture_border_clamp"]     = &s.use_arb_texture_border_clamp;
	//exts["GL_EXT_compiled_vertex_array"]    = &s.use_ext_compiled_vertex_array;
	exts["GL_EXT_texture_compression_s3tc"] = &s.use_ext_s3tc_texture_compression;
	exts["GL_EXT_separate_specular_color"]  = &s.use_ext_separate_specular_color;
	exts["GL_EXT_secondary_color"]          = &s.use_ext_secondary_color;
	//exts["GL_NV_fence"]                     = &s.use_nv_fence;
	exts["GL_NV_vertex_array_range"]        = &s.use_nv_vertex_array_range;
	exts["GL_NV_vertex_array_range2"]       = &s.use_nv_vertex_array_range2;
	exts["GL_NV_fog_distance"]              = &s.use_nv_fog_distance;
	exts["GL_NV_register_combiners"]        = &s.use_nv_register_combiners;
	exts["GL_SGIS_generate_mipmap"]         = &s.use_sgis_generate_mipmap;

	for (ExtCont::iterator i = exts.begin(); i != exts.end(); ++i) {
		if (!gl.is_supported(i->first)) {
			*i->second = false;
			dout << i->first << " not supported\n";
		}
	}

	if (!gl.is_supported("GL_VERSION_1_2")) {
		dout << "OpenGL 1.2" << " not supported\n";
		s.use_opengl_1_2 = false;
	}

	if(s.use_arb_texture_compression) {
		glHint(GL_TEXTURE_COMPRESSION_HINT_ARB, GL_NICEST);
	}

	if(s.use_ext_separate_specular_color) {
		glLightModeli((GLenum)GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);
	}

	if(s.use_nv_fog_distance) {
		glFogi((GLenum)GL_FOG_DISTANCE_MODE_NV, GL_EYE_RADIAL_NV);
	}
	
	if(int err = glGetError() != GL_NO_ERROR) {
		throw reaper_error(std::string("enable_extensions end:") + (char*)gluErrorString(err));
	}
}

}
}

/*
 * $Author: fizzgig $
 * $Date: 2003/01/11 23:58:43 $
 * $Log: settings.cpp,v $
 * Revision 1.35  2003/01/11 23:58:43  fizzgig
 * improved opengl implementation reporting
 *
 * Revision 1.34  2003/01/06 12:42:01  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.33  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.32  2002/09/23 11:48:58  fizzgig
 * Build/Link cleanup
 *
 * Revision 1.31  2002/09/20 23:46:59  pstrand
 * use_texturing->texture_level, minor event fix.
 *
 * Revision 1.30  2002/09/10 17:36:08  fizzgig
 * Misc fixes
 *
 * Revision 1.29  2002/06/19 15:08:29  fizzgig
 * fixed s3tc ext in glext.h
 *
 * Revision 1.28  2002/06/09 22:44:11  pstrand
 * duh
 *
 * Revision 1.27  2002/06/06 12:20:21  pstrand
 * 'std::'
 *
 * Revision 1.26  2002/06/06 07:59:38  pstrand
 * gl init, settings, fix
 *
 * Revision 1.25  2002/05/22 10:04:46  fizzgig
 * no message
 *
 * Revision 1.24  2002/05/21 03:07:51  fizzgig
 * envmap update options (gfx_renderer::environment_mapping: 0->off 1->incremental 2->instant)
 * preliminary support for shadow volumes (now requests stencil buffer as well)
 *
 * Revision 1.23  2002/05/19 17:36:49  fizzgig
 * glh-lib added (for extensions detect & init)
 * nvparse added
 * extgen added
 * texture blending example added
 *
 * Revision 1.22  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.21  2002/05/03 20:28:59  pstrand
 * use res/resource.h for saving...
 *
 * Revision 1.20  2002/02/27 18:08:40  pstrand
 * release-0.96
 *
 * Revision 1.19  2002/02/07 00:02:51  macke
 * STLPort 4.5.1 Win32-fix + shadow & render reorg
 *
 * Revision 1.18  2002/01/16 00:25:28  picon
 * terrängbelysning
 *
 * Revision 1.17  2002/01/08 19:40:21  macke
 * Texturdetaljnivå, samt lite småfix..
 *
 * Revision 1.16  2001/12/17 16:28:32  macke
 * div bök
 *
 * Revision 1.15  2001/12/04 23:01:23  picon
 * menu system now working ... barely. :)
 *
 * Revision 1.14  2001/10/02 15:22:27  macke
 * mhm!
 *
 * Revision 1.13  2001/09/24 13:15:08  macke
 * Inställningar för grafikmotorn läses från fil
 *
 * Revision 1.12  2001/09/24 02:33:25  macke
 * Meckat lite med fulbuggen i grafikmotorn.. verkar funka att deallokera nu.. ?
 *
 * Revision 1.11  2001/07/31 21:57:56  macke
 * Nytt försök med vertexarrays..
 *
 * Revision 1.10  2001/06/09 01:58:52  macke
 * Grafikmotor reorg
 *
 * Revision 1.9  2001/06/07 05:14:20  macke
 * Reaper v0.9
 *
 * Revision 1.8  2001/05/14 20:00:53  macke
 * bugfix och rök på missiler..
 *
 * Revision 1.7  2001/05/10 11:40:16  macke
 * häpp
 *
 */
