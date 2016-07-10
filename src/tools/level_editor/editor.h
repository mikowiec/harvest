#ifndef REAPER_LVLED_EDITOR_H
#define REAPER_LVLED_EDITOR_H

#include <GL/glut.h>
#include "ext/glui/glui.h"
#undef min
#undef max

#include "main/types.h"
#include "gfx/gfx.h"
#include "gfx/camera.h"
#include "gfx/settings.h"
#include "world/world.h"
#include "game/level.h"
#include "object/object_impl.h"
#include "object/renderer.h"
#include "object/sound.h"

using namespace reaper;
using namespace reaper::object;
using reaper::object::phys::ObjectAccessor;

using namespace std;

namespace reaper {
namespace level_editor {

extern gfx::Renderer *rr;
extern world::World  *wr;
extern world::LevelInfo level_info;

/// Stupid object only for positioning/rendering
class Object : public object::ObjImpl<object::DynamicBase>
{
	object::renderer::Silly rd;
	std::string mesh_id;
	std::string company;
public:
	Object(const std::string& mesh, const Matrix& mtx, CompanyID c);

	void collide(const CollisionInfo& cI)        {}
	void render() const            { rd.render(); }
	void gen_sound(reaper::sound::Interface& si) {}
	void simulate(double dt)                     {}		
	void think()				     {}
	void add_waypoint(const Point& p)	     {}
	void del_waypoint(Point& p)		     {}
	void receive(const reaper::ai::Message& msg) {}

	void set_pos(const Matrix&, const Point &p);
	void reset_rotation();
	void kill() { data.kill(); }
	const std::string & get_mesh() { return mesh_id; }
	const std::string & get_company() { return company; }

	      ObjectAccessor& get_physics()       { return *static_cast<ObjectAccessor*>(0); }
	const ObjectAccessor& get_physics() const { return *static_cast<ObjectAccessor*>(0); }
};

extern misc::SmartPtr<Object> selected_object;

extern int         main_window;

extern GLUI_String level_file;

extern GLUI_EditText *sky_texture;
extern GLUI_EditText *sky_color;
extern GLUI_Spinner  *sky_altitude;
extern GLUI_Spinner  *sky_width;

extern GLUI_Listbox  *fog_type;
extern GLUI_EditText *fog_color;
extern GLUI_Spinner  *fog_length;
extern GLUI_Spinner  *fog_density;

extern int           use_lighting;
extern GLUI_Rotation *global_dir;
extern GLUI_EditText *global_ambient;
extern GLUI_EditText *global_diffuse;

extern int         draw_local_lights;
extern int         draw_light_radius;
extern int         curr_light_string;

extern GLUI_Listbox  *light_list;
extern GLUI_EditText *current_light;
extern GLUI_EditText *local_diffuse;
extern GLUI_EditText *local_specular;
extern GLUI_Spinner  *local_linear;
extern GLUI_Spinner  *local_quadratic;
extern GLUI_Button   *remove_light;

extern GLUI_EditText *terrain_file;
extern int            draw_water;

extern GLUI_Listbox     *object_list;
extern int               object_type;
extern GLUI_Listbox     *company_list;
extern int               company_type;
extern GLUI_Rotation    *obj_rot;
extern GLUI_Translation *obj_trans_xz;
extern GLUI_Translation *obj_trans_y;
extern Matrix4           obj_rotate;
extern Point             obj_pos;
extern float             obj_control;
extern float             obj_height; // Absolute
extern float             obj_altitude; // Absolute
extern GLUI_Button      *remove_object;
extern GLUI_Button      *update_object;
extern int              lock_altitude;

extern GLUI_String      objects_file;

extern int draw_object_finders;
extern int filter_objects;
extern int filter_company;

extern GLUI            *glui, *glui2;

extern GLUI_Rotation    *cam_rot;
extern GLUI_Translation *cam_trans_xz;
extern GLUI_Translation *cam_trans_y;
extern Matrix4           cam_rotate;
extern Point             camera_pos;
extern Point             cam_pos; 
extern float             cam_control;
extern float             cam_zoom;
extern float             cam_altitude;
extern int		 camera_safe_altitude;

extern GLUI_Panel   *level_panel;
extern GLUI_Panel   *sky_panel;
extern GLUI_Panel   *fog_panel;
extern GLUI_Panel   *local_panel;
extern GLUI_Panel   *object_mod_panel;

extern GLUI_Rollout *roll_lights;
extern GLUI_Rollout *roll_global; 
extern GLUI_Rollout *roll_local;
extern GLUI_Rollout *roll_level;
extern GLUI_Rollout *roll_terrain;
extern GLUI_Rollout *roll_options;
extern GLUI_Rollout *roll_object;
extern GLUI_Rollout *roll_object_file;

extern float xy_aspect;

/********** User IDs for callbacks ********/
enum Callbacks {
	LOAD_LEVEL,
	SAVE_LEVEL,

	FOG_TYPE,
	APPLY_LEVEL,

	USE_LIGHTING,
	APPLY_GLOBAL_LIGHT,

	DRAW_LOCAL_LIGHTS,
	DRAW_LIGHT_RADIUS,
	ADD_LOCAL_LIGHT,
	REMOVE_LOCAL_LIGHT,
	APPLY_LOCAL_LIGHT,

	LOAD_TERRAIN,
	CALC_TERRAIN,
	DRAW_WATER,
	REDRAW,

	OBJ_RESET_ROT,

	OBJ_SELECTED,
	OBJ_DESELECTED,

	CAM_ROT,
	CAM_XZ,
	CAM_Y,
	CAM_RESET_ROT,

	CLEAR_OBJECTS
};

extern char *names[];
extern char *companies[];

// callbacks
void load_level(int);
void load_terrain(int);
void apply_level(int);
void control_cb(int);
void obj_moved(int);

void add_object(int);
void rem_object(int);
void upd_object(int);

void cam_move(int);

void load_objects(int);
void save_objects(int);

void clear_objects(int);

// other funcs
void build_gui();
void myExit();
void select_object(int x, int y);
void deselect_object();
void sel_object();
void draw();

gfx::Camera set_cam();

}
}
#endif
