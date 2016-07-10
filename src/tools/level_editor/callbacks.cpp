#include "os/compat.h"

#include <cfloat>

#include "editor.h"
#include "main/types_io.h"

#include "res/res.h"
#include "misc/stlhelper.h"
#include "misc/sequence.h"

#include "gfx/io.h"
#include "gfx/settings.h"

#include "misc/free.h"
#include "misc/parse.h"		
#include "main/types4_ops.h"

using reaper::misc::zero_delete;

namespace reaper {
namespace level_editor {

void control_cb( int control )
{
	switch( control ) {
	case SAVE_LEVEL: 
		level_info.save(&level_file[0]); 
		break;
		
	case FOG_TYPE:
		if(fog_type->get_int_val() == world::LevelInfo::Linear) {
			fog_density->disable();
			fog_length->enable();
		} else {
			fog_density->enable();
			fog_length->disable();
		}			
		glutPostRedisplay();
		break;
		
	case USE_LIGHTING: 
		gfx::Settings::current.use_lighting = (use_lighting == 1);
		glutPostRedisplay();			
		break;
		
	case APPLY_GLOBAL_LIGHT: break;
		
	case DRAW_LOCAL_LIGHTS:  break;
	case DRAW_LIGHT_RADIUS:  break;
	case ADD_LOCAL_LIGHT:    break;
	case REMOVE_LOCAL_LIGHT: break;
	case APPLY_LOCAL_LIGHT:  break;
		
	case CALC_TERRAIN: break;
		
	case DRAW_WATER: 
		gfx::Settings::current.draw_water = (draw_water == 1);
		glutPostRedisplay();
		break;
		
	case REDRAW: 
		glutPostRedisplay(); 
		break;
		
	case OBJ_RESET_ROT: 
		selected_object->reset_rotation();
		obj_rotate.load_identity();
		glutPostRedisplay();
		break;
		
	case CAM_RESET_ROT: 
		cam_rotate.load_identity();
		glutPostRedisplay();
		break;
		
	default: 
		cout << "Unknown callback id: " << control << "\n"; 
		exit(0);
	}
}

void load_renderer();

void clear_objects(int)
{
	if(wr != 0) {
		using namespace reaper::misc;

		std::deque<DynamicBase*> ptrs;

		for(dyn_iterator i = wr->begin_dyn(); i != wr->end_dyn(); ++i) {
			i->kill();
			//ptrs.push_back(*i);
			//(*wr)->erase(i);
		}

		//for_each(seq(ptrs), delete_it);
		
		selected_object = 0;
	}
}

void load_level(int)
{
	clear_objects(0);

	roll_object->disable();
	//roll_level->disable();
	roll_object_file->enable();

	zero_delete(rr);
	zero_delete(wr);
	try {
		static WorldRef wrr = world::World::create(&level_file[0]); 
		wrr = world::World::create(&level_file[0]); 
	 	wr = &(*wrr);
		level_info = wr->get_level_info();
	}
	catch(world::world_init_error &e) {
		cout << "Error reading level: " << e.what() << "\n";
		zero_delete(wr);
		return;
	}

	load_renderer();
}

void load_terrain(int)
{
	load_level(0); //FIXME:

	/*
	clear_objects(0);

	roll_object->disable();
	//roll_level->disable();
	roll_object_file->enable();

	zero_delete(rr);
	zero_delete(wr);
	try {
		wr = world::World::create(&level_file[0]); 
		level_info = wr->get_level_info();
	}
	catch(world::world_init_error &e) {
		cout << "Error reading level: " << e.what() << "\n";
		wr.release();
	}

	load_renderer();
	*/
}

void load_renderer()
{
	try {
		static gfx::RendererRef rrr;
		rrr = gfx::Renderer::create();
		rr = &(*rrr);
		rr->load();
		rr->start();
	}
	catch(fatal_error_base &e) {
		cout << e.what();
	}
	
	roll_object->enable();
	//roll_level->enable();
	roll_object_file->enable();

	terrain_file->set_text(level_info.terrain_mesh.c_str());

	sky_texture ->set_text(level_info.sky_texture.c_str());
	sky_color   ->set_text(write(level_info.sky_color).c_str());
	sky_altitude->set_int_val(level_info.sky_altitude);
	sky_width   ->set_int_val(level_info.sky_width);

	fog_color  ->set_text(write(level_info.fog_color).c_str());
	fog_density->set_float_val(level_info.fog_density);
	fog_length ->set_int_val(level_info.fog_length);
	fog_type   ->set_int_val(level_info.fog_type);

	cam_zoom = 1;
	cam_altitude = 200;
	cam_control  = wr->get_altitude(Point2D(0,0)) + cam_altitude;

	camera_pos.x = 0;
	camera_pos.y = cam_control;
	camera_pos.x = 0;

	glutPostRedisplay();
}

void apply_level(int)
{
	world::LevelInfo li(level_info);

	try {
		li.sky_texture  = sky_texture->get_text();
		li.sky_altitude = sky_altitude->get_int_val();
		li.sky_width    = sky_width->get_int_val();
		li.sky_color    = read<Color>(sky_color->get_text());
	} catch( io_error & ) {
		cout << "Illegal format for sky color!\n";
		return;
	}
	
	try {
		li.fog_density = fog_density->get_float_val();
		li.fog_length  = fog_length->get_int_val();
		li.fog_color   = read<Color>(fog_color->get_text());
		li.fog_type    = static_cast<world::LevelInfo::FogType>(fog_type->get_int_val());
	} catch ( io_error & ) {
		cout << "Illegal format for fog color!\n";
		return;
	}

	level_info = li;
}

void obj_moved(int i)
{
	if(selected_object != 0) {
		Point p = obj_pos;
		std::swap(p.y, p.z);
		Point2D p2(p.x, p.z);
		
		if(i==1) {  // lock_altitude updated
			if(lock_altitude) {
				obj_height = obj_altitude - wr->get_altitude(p2);
				obj_control = obj_height;
			} else {
				obj_altitude = obj_height + wr->get_altitude(p2);
				obj_control = obj_altitude;
			}
		} 
		if(lock_altitude) {
			obj_height = obj_control;
			p.y = wr->get_altitude(p2) + obj_height;
		} else {
			obj_altitude = obj_control;
			p.y = obj_altitude;
		}
		
		obj_pos = p;
		std::swap(obj_pos.y, obj_pos.z);
		selected_object->set_pos(Matrix(obj_rotate), p);
		cam_move(0);
		glutPostRedisplay();			
	}
}

void add_object(int)
{
	Point new_pos = camera_pos;
	obj_rotate.load_identity();
	
	selected_object = misc::SmartPtr<Object>(new Object(names[object_type], Matrix(new_pos), static_cast<CompanyID>(company_type)));
	if(lock_altitude) {
		new_pos.y = wr->get_altitude(Point2D(new_pos.x, new_pos.z));
		obj_control = 0;
	} else {
		Matrix rot(cam_rotate);
		new_pos += rot.row(2) * (selected_object->get_radius() * 2 + 20);
		obj_control = new_pos.y;
	}
	obj_altitude = new_pos.y;
	obj_height = obj_altitude - wr->get_altitude(Point2D(new_pos.x, new_pos.z));

	selected_object->set_pos(Matrix::id(),new_pos);

	wr->add_object((DynamicPtr)selected_object);
	sel_object();
}

void rem_object(int)
{
	if(selected_object != 0) {
		for(dyn_iterator i = wr->begin_dyn(); i != wr->end_dyn(); ++i) {
			if(*i == selected_object) {
				//(*wr)->erase(i);
				//delete selected_object;
				selected_object->kill();
				deselect_object();				
				return;
			}
		}
		cout << "Selected object does not exist in world ?!!?\n";
		exit(0);
	}
}

void upd_object(int)
{
	rem_object(0);
	add_object(0);
}

void cam_move(int val)
{
	static Point prev_pos = cam_pos;

	if(val == OBJ_SELECTED) {
		cam_zoom = selected_object->get_radius() * 2 + 20;
		cam_control = cam_zoom;
	} else if(val == OBJ_DESELECTED) {
		cam_altitude = camera_pos.y;
		cam_control = cam_altitude;
		prev_pos.z = cam_altitude;
	}

	if(selected_object.valid()) {
		cam_control = std::max(selected_object->get_radius() * 2,cam_control);
		cam_zoom = cam_control;		
		camera_pos = selected_object->get_pos() - cam_zoom * norm(Matrix(cam_rotate).col(2));
	} else {
		if(camera_safe_altitude) {
			cam_control = std::max(wr->get_altitude(Point2D(camera_pos.x, camera_pos.z)) + .5f,cam_control);
		}
		cam_altitude = cam_control;

		/////
		cam_pos.z = cam_altitude;
		Point diff = cam_pos - prev_pos;

		std::swap(diff.y,diff.z);
		diff.x *= -1;
		diff.y *= -1;

		Vector move = (Matrix(transpose(cam_rotate)) * diff).to_vec();
		float len = length(move);

		switch(val) {
		case CAM_Y: move.x = 0; move.z = 0; move.y *= len; break;
		case CAM_XZ: move.y = 0; move *= len; break;
		};

		camera_pos += move;				
	}

	if(camera_safe_altitude) {
		camera_pos.y = std::max( camera_pos.y, wr->get_altitude(Point2D(camera_pos.x, camera_pos.z)) + .5f);
		camera_pos.y = std::min( camera_pos.y, 1200.0f);
	}

	prev_pos = cam_pos;		         

	glutPostRedisplay();		
}

bool read_object(istream &is) 
{
	std::string mesh;
	CompanyID company;
	Matrix m;

	try {
		is.ignore(256,':') >> mesh;
		is.ignore(256,':') >> company;
		is.ignore(256,':') >> m;
	}
	catch (...) {
		return false;
	}

	wr->add_object((DynamicPtr)new Object(mesh, m, company));
	return true;
}

void load_objects(int)
{
	if(wr == 0) {
		return;
	}

	try {
		res::res_stream is(res::File,&objects_file[0]);
		while(read_object(is));
	}
	catch(res::resource_not_found &e) {
		cout << e.what() << '\n';
		return;
	}
}

void save_objects(int)
{
	if(wr != 0) {
		res::res_out_stream os(res::File,&objects_file[0],true);
		os << "# Reaper scenario object positions\n\n";		

		for(dyn_iterator i = wr->begin_dyn(); i != wr->end_dyn(); ++i) {
			if(!i->is_dead()) {
				Object& o = dynamic_cast<Object&>(*(*i).get());
				os << "object: "  << o.get_mesh() << '\n';
				os << "company: " << o.get_company() << '\n';
				os << "matrix: "  << o.get_mtx() << "\n\n";
			}			
		}
	}
}


}
}
