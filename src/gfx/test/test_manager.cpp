#include "os/compat.h"
#include "gfx/object_manager.h"
#include "os/gfx.h"
#include "os/debug.h"
#include <iostream>

using namespace reaper;
using reaper::gfx::object::Manager;
using reaper::gfx::object::ManagerRef;
using reaper::os::gfx::Gfx;
using namespace std;

std::vector<Matrix4> pm(2);
std::vector<Vector4> pv(2);

const char* ammo_box             = "ammo_box";
const char* turret               = "turret";
const char* turret_turret        = "turret_turret";
const char* turret_turret_barrel = "turret_turret_barrel";
const char* ship                 = "ship";

int main()
{      
        try {
		pv[0] = Vector4(45,0,1,0);	// yaw
		pv[1] = Vector4(45,0,0,1);	// pitch

		pm[0] = Matrix4(45,Vector(0,1,0));
		pm[1] = Matrix4(45,Vector(0,0,1));

		debug::DebugOutput dout("main",0);
		Gfx gx;

		ManagerRef mr = Manager::create();

		// Test object types
		if(Manager::get_obj_type(turret) != Manager::Parametric) {
			dout << "Turret is not parametric ?!?!\n";
			return 1;
		} 
		if(Manager::get_obj_type(ammo_box) != Manager::Standard) {
			dout << "AmmoBox is not standard ?!?!\n";
			return 1;
		} 

		Point p;
		dout << "-- NO PARAMETERS --\n";
		p = mr->get_matrix(turret).get_pos();
		dout << "turret matrix              : " << p << '\n';
		p = mr->get_matrix(turret_turret).get_pos();
		dout << "turret_turret matrix       : " << p << '\n';
		p = mr->get_matrix(turret_turret_barrel).get_pos();
		dout << "turret_turret_barrel matrix: " << p << '\n';

		dout << "\n-- ROTATION PARAMETERS --\n";
		p = mr->get_matrix(turret,pv).get_row(0);
		dout << "turret matrix              : " << p << '\n';
		p = mr->get_matrix(turret_turret,pv).get_row(0);
		dout << "turret_turret matrix       : " << p << '\n';
		p = mr->get_matrix(turret_turret_barrel,pv).get_row(0);
		dout << "turret_turret_barrel matrix: " << p << '\n';

		dout << "\n-- MATRIX PARAMETERS --\n";
		p = mr->get_matrix(turret,pm).get_row(0);
		dout << "turret matrix              : " << p << '\n';
		p = mr->get_matrix(turret_turret,pm).get_row(0);
		dout << "turret_turret matrix       : " << p << '\n';
		p = mr->get_matrix(turret_turret_barrel,pm).get_row(0);
		dout << "turret_turret_barrel matrix: " << p << '\n';

		if(mr->get_matrix(turret_turret_barrel,pm).get_row(0) != 
		   mr->get_matrix(turret_turret_barrel,pv).get_row(0)    ||
		   mr->get_matrix(turret_turret_barrel,pm).get_row(1) != 
		   mr->get_matrix(turret_turret_barrel,pv).get_row(1)    ||
		   mr->get_matrix(turret_turret_barrel,pm).get_row(2) != 
		   mr->get_matrix(turret_turret_barrel,pv).get_row(2)    ||
		   mr->get_matrix(turret_turret_barrel,pm).get_row(3) != 
		   mr->get_matrix(turret_turret_barrel,pv).get_row(3)) {
			dout << "get_matrix for vectors/matrices works incorrectly!\n";
			return 1;
		}

		
		if(mr->count() != 1) {
			dout << "Invalid count after get_radius(): " << mr->count() << " should be 1\n";
			return 1;
		}



		// Test get_radius
		dout << "\n-- BOUNDING SPHERES --\n";
		dout << "ammo_box bounding sphere:" << mr->get_radius(ammo_box) << '\n';
		dout << "ship bounding sphere:" << mr->get_radius(ship) << '\n';
		dout << "turret bounding sphere:" << mr->get_radius(turret) << '\n';

		// Some confusion, should not load additional objects either
		mr->get_radius(ammo_box);
		mr->get_radius(turret);

		if(mr->count() != 3) {
			dout << "Invalid count after get_radius(): " << mr->count() << " should be 3\n";
			return 1;
		}

		// test that base matrix is zero
		if(mr->get_matrix(turret).get_pos() != Point(0,0,0) ||
		   mr->get_matrix(turret,pm).get_pos() != Point(0,0,0) ||			
		   mr->get_matrix(turret,pv).get_pos() != Point(0,0,0)) {
			dout << " Error in turret matrix:" << p << p.w << '\n';
			return 1;
		}

		if(mr->count() != 3) {
			dout << "Invalid count after get_matrix(): " << mr->count() << " should be 3\n";
			return 1;
		}

		mr->purge();
		if(mr->count() != 0) {
			dout << "Purge not ok! " << mr->count() << " objects remaining!\n";
			return 1;
		}

		dout << "Successful exit\n";
                return 0;
        }
        catch(const fatal_error_base &e) {
                cout << "Reaper fatal exception:" << e.what() << endl;
                return 1;
        } 
        catch(const error_base &e) {
                cout << "Reaper exception:" << e.what() << endl;
                return 1;
        }
        catch(const exception &e) {
                cout << "Std exception: " << e.what() << endl;
                return 1;
        }
}
 
