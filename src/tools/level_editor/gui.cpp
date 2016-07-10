#include "os/compat.h"
#include "editor.h"

namespace reaper {
namespace level_editor {

using world::LevelInfo;

void build_gui()
{
	/*** Create the side subwindow ***/
	glui = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_RIGHT );
	
	level_panel = glui->add_panel( "Level" );		
		glui->add_edittext_to_panel( level_panel, "Filename", GLUI_EDITTEXT_TEXT, level_file);
		glui->add_button_to_panel( level_panel, "Load", 0, load_level );
		glui->add_button_to_panel( level_panel, "Save", SAVE_LEVEL, control_cb );

	roll_level = glui->add_rollout_to_panel( level_panel, "Properties", false );
		sky_panel = glui->add_panel_to_panel(roll_level, "Sky");
			sky_texture   = glui->add_edittext_to_panel(sky_panel, "Texture:", GLUI_EDITTEXT_TEXT);
			sky_color     = glui->add_edittext_to_panel(sky_panel, "Color:", GLUI_EDITTEXT_TEXT);
			sky_altitude  = glui->add_spinner_to_panel(sky_panel, "Altitude:", GLUI_SPINNER_INT);
			sky_width     = glui->add_spinner_to_panel(sky_panel, "Density:", GLUI_SPINNER_INT);
		fog_panel = glui->add_panel_to_panel(roll_level, "Fog");
			fog_color     = glui->add_edittext_to_panel(fog_panel, "Color:", GLUI_EDITTEXT_TEXT);
			fog_type      = glui->add_listbox_to_panel(fog_panel, "Type:", 0, FOG_TYPE, control_cb);
			fog_length    = glui->add_spinner_to_panel(fog_panel, "Length:", GLUI_SPINNER_INT);
			fog_density   = glui->add_spinner_to_panel(fog_panel, "Density:", GLUI_SPINNER_FLOAT);

			fog_density->disable();
			fog_type->add_item(LevelInfo::Linear, "Linear");
			fog_type->add_item(LevelInfo::Exp, "Exponential");
			fog_type->add_item(LevelInfo::Exp_2, "Exp. squared");
		glui->add_button_to_panel( roll_level, "Apply", 0, apply_level);

	roll_level->disable();
	
	/******** Add some controls for lights ********/
	
	roll_lights = glui->add_rollout_to_panel( roll_level, "Lights", false );	
	        glui->add_checkbox_to_panel(roll_lights, "Apply lighting", &use_lighting, 
			                    USE_LIGHTING, control_cb);
		roll_global = glui->add_rollout_to_panel( roll_lights, "Global", false );
			global_dir     = glui->add_rotation_to_panel( roll_global, "Direction");
			global_ambient = glui->add_edittext_to_panel( roll_global, "Ambient color", GLUI_EDITTEXT_TEXT);
			global_diffuse = glui->add_edittext_to_panel( roll_global, "Diffuse color", GLUI_EDITTEXT_TEXT);
			                 glui->add_button_to_panel( roll_global, "Apply", APPLY_GLOBAL_LIGHT, control_cb);

		roll_local = glui->add_rollout_to_panel( roll_lights, "Locals", false );
			               glui->add_checkbox_to_panel(roll_local, "Draw local lights", &draw_local_lights, DRAW_LOCAL_LIGHTS, control_cb);
			               glui->add_checkbox_to_panel(roll_local, "Draw light radius", &draw_light_radius, DRAW_LIGHT_RADIUS, control_cb);
			               glui->add_button_to_panel(roll_local, "Add", ADD_LOCAL_LIGHT, control_cb);
			remove_light = glui->add_button_to_panel(roll_local, "Remove", REMOVE_LOCAL_LIGHT, control_cb);
			light_list   = glui->add_listbox_to_panel( roll_local, "Current light:", &curr_light_string );

			remove_light->disable();

			local_panel = glui->add_panel_to_panel( roll_local, "Current light");
				current_light   = glui->add_edittext_to_panel( local_panel, "Name:", GLUI_EDITTEXT_TEXT);
				local_diffuse   = glui->add_edittext_to_panel( local_panel, "Diffuse color:", GLUI_EDITTEXT_TEXT);
				local_specular  = glui->add_edittext_to_panel( local_panel, "Specular color:", GLUI_EDITTEXT_TEXT);	
				local_linear    = glui->add_spinner_to_panel( local_panel, "Linear attenuation:", GLUI_SPINNER_FLOAT);
				local_quadratic = glui->add_spinner_to_panel( local_panel, "Quadratic attenuation:", GLUI_SPINNER_FLOAT);
				                  glui->add_button_to_panel( local_panel, "Apply", APPLY_LOCAL_LIGHT, control_cb);

				local_linear->set_float_limits(0,1);
			        local_quadratic->set_float_limits(0,1);

	roll_lights->disable();

	/** terrain properties */

	roll_terrain = glui->add_rollout_to_panel( roll_level, "Terrain", false );
		terrain_file = glui->add_edittext_to_panel( roll_terrain, "Filename", GLUI_EDITTEXT_TEXT);
		               glui->add_button_to_panel( roll_terrain, "Load", 0, load_terrain);
		               glui->add_checkbox_to_panel( roll_terrain, "Draw water", &draw_water, REDRAW, control_cb);
		               glui->add_button_to_panel( roll_terrain, "Calculate", CALC_TERRAIN, control_cb);

	roll_terrain->disable();

	/* object properties */

	roll_object_file = glui->add_rollout( "Objects file", false );
		glui->add_edittext_to_panel( roll_object_file, "Filename", GLUI_EDITTEXT_TEXT, objects_file);
		glui->add_button_to_panel( roll_object_file, "Load", 0, load_objects );
		glui->add_button_to_panel( roll_object_file, "Save", 0, save_objects );
		glui->add_button_to_panel( roll_object_file, "Clear", 0, clear_objects );
	roll_object_file->disable();

	roll_object = glui->add_rollout( "Object", false );
		object_list   = glui->add_listbox_to_panel( roll_object, "Type:", &object_type);
		company_list  = glui->add_listbox_to_panel( roll_object, "Company:", &company_type);
		                glui->add_button_to_panel( roll_object, "Add",    0, add_object);
		remove_object = glui->add_button_to_panel( roll_object, "Remove", 0, rem_object);
		update_object = glui->add_button_to_panel( roll_object, "Update", 0, upd_object);
		remove_object->disable();
		update_object->disable();
		object_mod_panel = glui->add_panel_to_panel(roll_object, "", GLUI_PANEL_NONE);
			obj_rot      = glui->add_rotation_to_panel( object_mod_panel, "Rotation", &obj_rotate[0][0], 0, obj_moved );
			obj_trans_xz = glui->add_translation_to_panel( object_mod_panel,  "Object XZ", GLUI_TRANSLATION_XY, &obj_pos.x, 0, obj_moved );
			obj_trans_y  = glui->add_translation_to_panel( object_mod_panel,  "Object Y", GLUI_TRANSLATION_Y, &obj_control, 0, obj_moved );
			               glui->add_button_to_panel( object_mod_panel, "Reset Rotation", OBJ_RESET_ROT, control_cb);
			obj_trans_xz->set_speed( .5 );
			obj_trans_y->set_speed( .5 );
		object_mod_panel->disable();
	roll_object->disable();


	/****** A 'quit' button *****/
	glui->add_button( "Quit", 0,(GLUI_Update_CB)myExit );	
	
	/**** Link windows to GLUI, and register idle callback ******/

	glui->set_main_gfx_window( main_window );	
	
	/*** Create the bottom subwindow ***/
	glui2 = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_BOTTOM );
	glui2->set_main_gfx_window( main_window );
	
	cam_rot = glui2->add_rotation( "Camera", &cam_rotate[0][0], CAM_ROT, cam_move );
	glui2->add_column( false );
	cam_trans_xz = glui2->add_translation( "Camera XZ", GLUI_TRANSLATION_XY, &cam_pos.x, CAM_XZ, cam_move );
	cam_trans_xz->set_speed( 2 );
	glui2->add_column( false );
	cam_trans_y = glui2->add_translation( "Camera Y", GLUI_TRANSLATION_Y, &cam_control, CAM_Y, cam_move );
	cam_trans_y->set_speed( 2 );
	glui2->add_column( false );
	glui2->add_button( "Reset camera", CAM_RESET_ROT, control_cb);
	glui2->add_checkbox( "Safe cam. alt.", &camera_safe_altitude, 0, cam_move);
        glui2->add_checkbox( "Lock obj. alt.", &lock_altitude, 1, obj_moved);
	glui2->add_column( false );
	glui2->add_checkbox( "Object finders", &draw_object_finders, REDRAW, control_cb);
	glui2->add_checkbox( "Filter company", &filter_company, REDRAW, control_cb);
	glui2->add_checkbox( "Filter type",    &filter_objects, REDRAW, control_cb);

	/*** generate object type list ***/

	int i=0;
	while(names[i][0] != 0) {
		object_list->add_item(i,names[i]);
		++i;
	};

	i=0;
	while(companies[i][0] != 0) {
		company_list->add_item(i,companies[i]);
		++i;
	};
}

}
}
