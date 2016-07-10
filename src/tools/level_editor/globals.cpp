#include "os/compat.h"
#include "editor.h"

namespace reaper {
namespace level_editor {

gfx::Renderer *rr = 0;
world::World  *wr = 0;

world::LevelInfo level_info;

misc::SmartPtr<Object> selected_object;

/** These are the live variables passed into GLUI ***/
int         main_window;

GLUI_String level_file = "test_level";

GLUI_EditText *sky_texture;
GLUI_EditText *sky_color;
GLUI_Spinner  *sky_altitude;
GLUI_Spinner  *sky_width;

GLUI_Listbox  *fog_type;
GLUI_EditText *fog_color;
GLUI_Spinner  *fog_length;
GLUI_Spinner  *fog_density;

int            use_lighting = 0;
GLUI_Rotation *global_dir;
GLUI_EditText *global_ambient;
GLUI_EditText *global_diffuse;

int draw_local_lights = 1;
int draw_light_radius = 0;
int curr_light_string = 0;

GLUI_Listbox  *light_list;
GLUI_EditText *current_light;
GLUI_EditText *local_diffuse;
GLUI_EditText *local_specular;
GLUI_Spinner  *local_linear;
GLUI_Spinner  *local_quadratic;
GLUI_Button   *remove_light;

GLUI_EditText *terrain_file;
int            draw_water = 1;

GLUI_Listbox     *object_list;
int               object_type = 0;
GLUI_Listbox     *company_list;
int               company_type = 0;

GLUI_Rotation    *obj_rot;
GLUI_Translation *obj_trans_xz;
GLUI_Translation *obj_trans_y;
GLUI_Button      *remove_object;
GLUI_Button      *update_object;
int              lock_altitude = 1;
float            obj_control = 0;
Matrix4          obj_rotate( true );
Point            obj_pos( 0.0, 0.0, 0.0 );
float            obj_altitude = 0;
float            obj_height = 0;

GLUI_String      objects_file;

GLUI             *glui, *glui2;

GLUI_Rotation    *cam_rot;
GLUI_Translation *cam_trans_xz;
GLUI_Translation *cam_trans_y;
int              lock_camera = 0;
Matrix4          cam_rotate( true );
Point            camera_pos(0,800,0);
Point            cam_pos(0,0,0); 
float            cam_control = 0;
float            cam_altitude = 200;
int              camera_safe_altitude = 1;
int              camera_follow_ground = 0;
float            cam_zoom = 20;

int draw_object_finders;
int filter_objects;
int filter_company;

GLUI_Panel   *level_panel;
GLUI_Panel   *sky_panel;
GLUI_Panel   *fog_panel;
GLUI_Panel   *local_panel;
GLUI_Panel   *object_mod_panel;

GLUI_Rollout *roll_level;
GLUI_Rollout *roll_lights;
GLUI_Rollout *roll_global; 
GLUI_Rollout *roll_local;
GLUI_Rollout *roll_terrain;
GLUI_Rollout *roll_options;
GLUI_Rollout *roll_object;
GLUI_Rollout *roll_object_file;

float xy_aspect;

char *names[] = { 
	"ammobox",
	"base",
	"building1",
	"building2",
	"building3",
	"bush1",
	"cactus1",
	"cactus2",
	"fuelbarrel",
	"fuelbarrel-group",
	"ground-container",
	"ground-missile",
	"ground-turret",
	"ground-vehicle",
	"launcher",
	"mothership",
	"ship",
	"ship2",
	"ship3",
	"ship4",
	"shipX",
	"tower",
	"tree",
	"tree1",
	"turret",
	"wall",
	"" 
};

char *companies[] = {
	"nature",
	"projectile",
        "slick",
        "horny",
        "shell",
        "punkrocker",
	"neutral",
	""
};

}
}
