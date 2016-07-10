/*
 * $Author: pstrand $
 * $Date: 2003/06/11 15:16:48 $
 * $Log: phys.cpp,v $
 * Revision 1.90  2003/06/11 15:16:48  pstrand
 * småfixar..
 *
 * Revision 1.89  2003/06/04 17:43:07  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.88  2002/12/29 17:04:40  pstrand
 * hw/ moved to os/
 *
 * Revision 1.87  2002/09/27 18:05:04  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.86  2002/09/23 12:08:44  fizzgig
 * Moved all anonymous namespaces into the reaper-namespace
 *
 * Revision 1.85  2002/05/22 21:30:52  pstrand
 * kill projectiles properly
 *
 * Revision 1.84  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.83  2002/04/06 20:13:31  pstrand
 * the matrix encapsulated one more level, use SillyData::set_mtx to change (or objectaccessor)
 *
 * Revision 1.82  2002/02/08 11:27:57  macke
 * rendering reorg and silhouette shadows!
 *
 */

#include "os/compat.h"

#include <cfloat>
#include <iostream>

#include "main/types.h"
#include "main/types_ops.h"
#include "main/types_io.h"
#include "main/types_param.h"
#include "phys/phys.h"
#include "phys/engine.h"
#include "os/debug.h"
#include "world/world.h"
#include "object/base_data.h"
#include "object/phys.h"
#include "object/controls.h"
#include "object/current_data.h"
#include "object/collide.h"

using namespace reaper::phys;
using namespace reaper::world;




namespace reaper {
namespace {
	reaper::debug::DebugOutput dout("object::phys");
}
namespace object {

namespace impl
{
	double get_current_time()
	{
		return 0;
	}
}

namespace phys
{



/*
ObjectAccessor::ObjectAccessor(SillyData &sd, DynamicData &dd,
	       const ShipInfo &i, const float &r)
 : sd(sd), vel(dd.velocity), rot_vel(dd.rot_velocity),
   max_acc(i.max_acc), max_vel(i.max_vel),mass(i.mass), radius(r), damage(i.damage)
{ }

ObjectAccessor::ObjectAccessor(SillyData &sd, DynamicData &dd,
	       const ShotInfo &i, const float &r)
 : sd(sd), vel(dd.velocity), rot_vel(dd.rot_velocity),
   max_acc(0), max_vel(i.speed),mass(0), radius(r), damage(i.damage)
{ }

ObjectAccessor::ObjectAccessor(SillyData &sd, DynamicData &dd,
	       float ma, float m, float d, float r)
 : sd(sd), vel(dd.velocity), rot_vel(dd.rot_velocity),
   max_acc(ma), max_vel(500), mass(m), radius(r), damage(d)
{ }



const Matrix& ObjectAccessor::get_mtx() const {
	return sd.get_mtx();
}

void ObjectAccessor::set_mtx(const Matrix& m) {
	sd.set_mtx(m);
}
*/






Turret::Turret(const controls::Turret &t,MtxParams &p, 
	current_data::Turret &current,
//                weapon::Turret &weap,
	const TurretInfo &info)
	: tc(t), 
	  pr(p),
	  current(current),
//                weap(weap),
	info(info)
{}


void Turret::simulate(double dt) {
	
	float delta_yaw = tc.yaw-current.yaw;

	
	//Make sure the abs value of deltayaw is not larger than
	//abs(maxYaw)
	if (delta_yaw > info.max_yaw*dt ) delta_yaw = info.max_yaw*dt;
        if (delta_yaw < -info.max_yaw*dt) delta_yaw = -info.max_yaw*dt;
	

	current.yaw+=delta_yaw;


	float delta_pitch = tc.pitch - current.pitch;
	
	//Make sure the abs value of deltayaw is not larger than
	//abs(maxYaw)
	if (delta_pitch > info.max_pitch*dt ) delta_pitch = info.max_pitch*dt;
        if (delta_pitch < -info.max_pitch*dt) delta_pitch = -info.max_pitch*dt;
	
	
	current.pitch+=delta_pitch;
	if (current.pitch < 0) current.pitch = 0;
	if (current.pitch > 90) current.pitch = 90;

	pr[0] = Matrix(current.yaw,   Vector(0,1,0));
	pr[1] = Matrix(current.pitch, Vector(1,0,0));

         //Fire any weapons			

/*
        if(tc.fire){			
		ShotBase* shot = weap.fire(sim_time);
		if(shot != 0) {
			phys_ref->insert(shot,sim_time);
		}
		
        }
*/
	
}

void GroundShip::simulate(const double dt){
	Matrix cm = data.get_mtx();

	//Nollställ translation för att kunna rotera kring lokala koordinatsystemet
	cm.pos() = Point(0, 0, 0);

	Vector loc_z = cm.col(2);
	Vector loc_y = cm.col(1);
	Vector loc_x = cm.col(0);

	Point p0 = data.get_mtx().pos();

	current.eng.thrust = tc.thrust;

	Vector v = dyn->velocity();	
	const Vector a = compute_acc(v,dt);

	//Runge kutta integration
	Vector pa,pb,pc,pd;
	Vector va,vb,vc,vd;
	va = dt*a;
	pa = dt*va;
	vb = dt*compute_acc(v+0.5*va,dt);
	pb = dt*(v+0.5*vb);
	vc = dt*compute_acc(v+0.5*vb,dt);
	pc = dt*(v+0.5*vc);
	vd = dt*compute_acc(v+vc,dt);
	pd = dt*(v+vd);

	dyn->set_velocity( v + (1.0/6.0)*(va+2*vb+2*vc+vd) );
	Point pos = p0 + (1.0/6.0)*(pa+2*pb+2*pc+pd);
/*
	//Euler
	dyn_data.velocity = v + dt*compute_acc(v);
	Vector pos = p0 + v*dt + v*dt*dt*0.5;
*/
	if(length_sq(dyn->velocity()) > info.max_vel*info.max_vel)
		dyn->set_velocity( info.max_vel*norm(dyn->velocity()) );

        //The rotation
        if(dt >0){
                
                
                Vector p_norm = norm(cross(Vector(0.0,1.0,0),loc_z));		
                
                float tmp = dot(p_norm,norm(loc_x));
                float roll;		
                
                if (tmp >= 1.0 || tmp <= -1.0) {
                        roll = 0;			
                } else {
                        roll = acos(tmp) * 57.3;
                        
                        if(loc_x.y < 0)
                                roll*= -1; 
                }
                
                if(isnan(roll)) {
                        dout << "Singalong 1: Vi hatar, vi hatar flyttal, vi hatar de som flyttal skapat har.. !\n";
                        roll = 0;
                }
                
                float pitch = 57.3*acos(dot(loc_z,Vector(0.0,1.0,0.0))) - 90.0;
                
                float delta_pitch,delta_roll;
                const float pitch_rate(20), roll_rate(20);
                
                if(pitch > dt*pitch_rate)
                {
                        if(pitch > 0)
                                delta_pitch = -dt*pitch_rate;
                        else
                                delta_pitch = dt*pitch_rate;
                }
                else
                        delta_pitch = -pitch;
                
                
                
                if(roll > dt*roll_rate)
                {
                        if(roll > 0)
                                delta_roll = -dt*roll_rate;
                        else
                                delta_roll = dt*roll_rate;
                }
                else
                        delta_roll = -roll;
                
                
                loc_y = cm.col(1);
                Matrix rot_y(info.max_rot_y*dt*tc.turn, Vector(0.0,1.0,0.0));
                dyn->set_velocity( rot_y*dyn->velocity() );
                cm = rot_y*cm;
                
                if(delta_roll < 70){
                        loc_z = cm.col(2);
                        Matrix rot_z(delta_roll, loc_z);
                        dyn->set_velocity( rot_z*dyn->velocity() );
                        cm = rot_z*cm;
                }
                
                loc_x = cm.col(0);
                Matrix rot_x(delta_pitch,loc_x);
                dyn->set_velocity( rot_x*dyn->velocity() );
                cm = rot_x*cm;
                
                
        }
	//reset the translation
	cm.pos() = pos;
       
	//Apply the changes
	if(last_ortho > 64){
		last_ortho = 0;
		orthonormalize3(cm);
	}
	last_ortho++;
	data.set_mtx(cm);

	sim_time+=dt;
}

Vector GroundShip::compute_acc(const Vector v,const double& dt)
{

	const Matrix m(data.get_mtx());
        Point p0 = m.pos();
        const float inv_mass = 1/info.mass;
        WorldRef wr;
        
        //First fix the accelerations.
        //Gravity
        Vector g(0.0, -9.81, 0.0);
        //Drag
        Vector d = info.drag_coeff*(1+fabs(tc.turn))*(-v)*inv_mass;
        
        float diff = info.altitude + 30 + wr->get_altitude(Point2D(p0.x,p0.z)) - p0.y;

        //Lift			 
        Vector l(0.0, 10 * diff + 0.02*current.accum_alt,0.0);

        //Thrust                
        Vector F = (((info.max_thrust-info.min_thrust)*tc.thrust)+info.min_thrust) * Vector(0.0, 0.0, -1.0);
        F *=inv_mass;
        F = m*F;
        
        current.accum_alt += dt*diff;        

        return F + d + l + g;
}
/*
void GroundTurret::simulate(const double dt){
	Matrix cm = m;

	//Nollställ translation för att kunna rotera kring lokala koordinatsystemet
	cm[3][0] = 0;
	cm[3][1] = 0;
	cm[3][2] = 0;
	cm[3][3] = 0;

	Vector loc_z(cm[2][0], cm[2][1], cm[2][2] );
	Vector loc_y(cm[1][0], cm[1][1], cm[1][2] );
	Vector loc_x(cm[0][0], cm[0][1], cm[0][2] );

	Vector p0 = Vector(m[3][0],m[3][1],m[3][2]);

	current.eng.thrust = tc.thrust;

	const Vector &v = dyn_data.velocity;	
	const Vector a = compute_acc(v,dt);

	//Runge kutta integration
	Vector pa,pb,pc,pd;
	Vector va,vb,vc,vd;
	va = dt*a;
	pa = dt*va;
	vb = dt*compute_acc(v+0.5*va,dt);
	pb = dt*(v+0.5*vb);
	vc = dt*compute_acc(v+0.5*vb,dt);
	pc = dt*(v+0.5*vc);
	vd = dt*compute_acc(v+vc,dt);
	pd = dt*(v+vd);

	dyn_data.velocity = v + (1.0/6.0)*(va+2*vb+2*vc+vd);
	Vector pos = p0 + (1.0/6.0)*(pa+2*pb+2*pc+pd);

	if(length_sq(dyn_data.velocity) > info.max_vel*info.max_vel)
		dyn_data.velocity = info.max_vel*norm(dyn_data.velocity);

	dyn_data.velocity.w = 0;	

        //The rotation
        if(dt >0){
                
                
                Vector p_norm = norm(cross(Vector(0.0,1.0,0),loc_z));		
                
                float tmp = dot(p_norm,norm(loc_x));
                float roll;		
                
                if (tmp >= 1.0 || tmp <= -1.0) {
                        roll = 0;			
                } else {
                        roll = acos(tmp) * 57.3;
                        
                        if(loc_x.y < 0)
                                roll*= -1; 
                }
                
                if(isnan(roll)) {
                        dout << "Singalong: Vi hatar, vi hatar flyttal, vi hatar de som flyttal skapat har.. !\n";
                        roll = 0;
                }
                
                float pitch = 57.3*acos(dot(loc_z,Vector(0.0,1.0,0.0))) - 90.0;
                
                float delta_pitch,delta_roll;
                const float pitch_rate(20), roll_rate(20);
                
                if(pitch > dt*pitch_rate)
                {
                        if(pitch > 0)
                                delta_pitch = -dt*pitch_rate;
                        else
                                delta_pitch = dt*pitch_rate;
                }
                else
                        delta_pitch = -pitch;
                
                
                
                if(roll > dt*roll_rate)
                {
                        if(roll > 0)
                                delta_roll = -dt*roll_rate;
                        else
                                delta_roll = dt*roll_rate;
                }
                else
                        delta_roll = -roll;
                
                
                loc_y = Vector(cm[1][0], cm[1][1], cm[1][2] );
                Matrix rot_y =  Matrix().rotate(info.max_rot_y*dt*tc.turn, Vector(0.0,1.0,0.0));
                dyn_data.velocity = rot_y*dyn_data.velocity;
                dyn_data.velocity.w = 0;
                cm = rot_y*cm;
                
                if(delta_roll < 70){
                        loc_z = Vector(cm[2][0], cm[2][1], cm[2][2] );
                        Matrix rot_z = Matrix().rotate(delta_roll, loc_z);
                        dyn_data.velocity = rot_z*dyn_data.velocity;
                        dyn_data.velocity.w = 0;
                        cm = rot_z*cm;
                }
                
                loc_x = Vector(cm[0][0], cm[0][1], cm[0][2] );
                Matrix rot_x = Matrix().rotate(delta_pitch,loc_x);
                dyn_data.velocity = rot_x*dyn_data.velocity;
                dyn_data.velocity.w = 0;
                cm = rot_x*cm;
                
                
        }
	//reset the translation
	cm[3][0] = pos.x;
	cm[3][1] = pos.y;
	cm[3][2] = pos.z;
	cm[3][3] = 1.0;
       
	//Apply the changes
	if(last_ortho > 64){
		last_ortho = 0;
		orthonormalize3(cm);
	}
	last_ortho++;
	m = cm;

	sim_time+=dt;
}

Vector GroundTurret::compute_acc(const Vector v,const double& dt)
{

        Vector p0 = Vector( m[3][0], m[3][1], m[3][2]);
        const float inv_mass = 1/info.mass;
        WorldRef wr(reaper::world::World::get_ref());
        
        //First fix the accelerations.
        //Gravity
        Vector g(0.0, -9.81, 0.0);
        //Drag
        Vector d = info.drag_coeff*(1+fabs(tc.turn))*(-v)*inv_mass;
        
        float diff = info.altitude + 30 + wr.inst().get_altitude(Point2D(p0.x,p0.z)) - p0.y;
//                float diff = 500 - p0.y;
//                dout << "Altitude : " << wr.inst().get_altitude(Point2D(p0.x,p0.z)) <<
//                        "Diff: " << diff << endl;

        //Lift			 
        Vector l(0.0, 15 * diff + 0.01*current.accum_alt,0.0);

        //Thrust                
        Vector F = (((info.max_thrust-info.min_thrust)*tc.thrust)+info.min_thrust) * Vector(0.0, 0.0, -1.0);
        F *=inv_mass;
        F = m*F;
        
        current.accum_alt += dt*diff;        

        return F + d + l + g;
}
*/

Vector Ship::compute_acc(const Vector v){

	const Matrix m(acc->mtx());
        Point p0 = m.pos();
        const float inv_mass = 1/info.mass;
        Vector loc_z = m.col(2);
        Vector loc_y = m.col(1);
        
        //First fix the accelerations.
        //Gravity
        Vector g(0.0, -9.81, 0.0);
        //Drag
        Vector d = info.drag_coeff*(1+fabs(tc.yaw))*(-v)*inv_mass;
        //Drag for not flying in correct direction
        Vector d2 = Vector(0.0,0.0,0.0);
        if(length_sq(v) > 0 ){
                Vector temp = norm(v);			
                d2 = 4*info.drag_coeff*(1-dot(-temp,loc_z)) * 
                        (-v) * inv_mass;
        }
        //Lift			 
        Vector l = Vector(0.0, -1.0, 0)*(dot(loc_y,g));
        //Thrust
        
        Vector F = (((info.max_thrust-info.min_thrust)*tc.thrust)+info.min_thrust) * Vector(0.0, 0.0, -1.0);
        F *=inv_mass;
        F = m*F;
        
        
        //Update shipdata and check afterburner
        if(current.eng.afterburner){
                F*= info.afterburner;
        }
        
        if(p0.y > reaper::phys::combat_cealing ){
                F = Vector(0.0, 0.0, 0.0);
                l = Vector(0,0,0);
                d2 = l;
                
        }

        return F + d + d2 + l + g;
}
float Ship::get_afterburner() const
{
	return current.afterburner_left;
}

Ship::Ship(BasicDyn* a,
     const controls::Ship &tc,
     current_data::Ship &current,
     const ShipInfo &info)
      : tc(tc),
	info(info),
	current(current),
	acc(a),
	last_ortho(0)
	 {}


void Ship::simulate(double dt)
{
	Matrix cm = acc->mtx();

	//Nollställ translation för att kunna rotera kring lokala koordinatsystemet
	cm.pos() = Point(0, 0, 0);

	Vector loc_z = cm.col(2);
	Vector loc_y = cm.col(1);
	Vector loc_x = cm.col(0);
	Point p0 = acc->pos();


	if(current.afterburner_left < info.afterburner) {
		current.afterburner_left = std::min<double>( info.afterburner, current.afterburner_left + dt*info.ab_recharge_rate);
	}
	current.eng.thrust = tc.thrust;
	current.eng.afterburner = false;

	//Update shipdata and check afterburner
	if(tc.afterburner && current.afterburner_left > dt*info.burn_rate ){
		current.afterburner_left -= dt*info.burn_rate;
		current.eng.afterburner = true;
	}

	Vector v = acc->velocity();
	const Vector a = compute_acc(v);


	//Runge kutta integration
	Vector pa,pb,pc,pd;
	Vector va,vb,vc,vd;
	va = dt*a;
	pa = dt*va;
	vb = dt*compute_acc(v+0.5*va);
	pb = dt*(v+0.5*vb);
	vc = dt*compute_acc(v+0.5*vb);
	pc = dt*(v+0.5*vc);
	vd = dt*compute_acc(v+vc);
	pd = dt*(v+vd);


	Vector new_vel = v + (1.0/6.0)*(va+2*vb+2*vc+vd);
	acc->set_velocity(new_vel);
	Point pos = p0 + (1.0/6.0)*(pa+2*pb+2*pc+pd);
/*
	//Euler
	dyn_data.velocity = v + dt*compute_acc(v);
	Vector pos = p0 + v*dt + v*dt*dt*0.5;
*/
	if(length_sq(new_vel) > info.max_vel*info.max_vel)
		acc->set_velocity(info.max_vel*norm(new_vel));

	//The rotations

	//Dampen the rotations
	//Dampen with 70 degrees/second
	
	//Rotate  only if dt > 0. Trying to make
	//the simulation more stable
	Vector rot_vel = acc->rot_velocity();
	if(dt > 0){
	Vector damp = norm(rot_vel) * 70 * dt;

	if(length(rot_vel) > length(damp) )
	 rot_vel -= damp;
	else
	 rot_vel = Vector(0, 0, 0);

	if(length(rot_vel) > 0){
		cm = Matrix(dt*length(rot_vel),norm(rot_vel))*cm;			 
		current.roll+=dot(dt*rot_vel,loc_z);
	}

	bool isUpsideDown;
	if (dot(loc_y,Vector(0,1,0)) > 0 )
		isUpsideDown = false;
	else{
		isUpsideDown = true;
		//dout << "Is upside down!" << std::endl;
	}	


	//Rotate the ship
	Vector p_norm = norm(cross(Vector(0.0,1.0,0),loc_z));		

	float tmp = dot(p_norm,norm(loc_x));

	float angle;		
	
	if (tmp >= 1.0 || tmp <= -1.0) {
		angle = 0;			
	} else {
		angle = acos(tmp) * 57.3;

		if(loc_x.y < 0)
			angle*= -1; 
	}

	if(isnan(angle)) {
		dout << "Singalong 2: Vi hatar, vi hatar flyttal, vi hatar de som flyttal skapat har.. !\n";
		angle = 0;
	}

	float pitch = 57.3*acos(dot(loc_z,Vector(0.0,1.0,0.0))) - 90.0;
	if(isUpsideDown)
		pitch = 180-pitch;

	//dout << "Rolling: " << angle << std::endl;
	//dout << "Pitch: " << pitch << std::endl;
	//cout << "p norm: " << p_norm << " loc_x " << loc_x << endl;
	//cout << "dotp: " << dot(p_norm,loc_x) << endl;
	//cout << "acos " << acos(dot(p_norm,loc_x)) << endl;
	
	loc_y = cm.col(1);
	Matrix rot_y;
	if(!isUpsideDown)
		rot_y = Matrix(info.max_rot_y*dt*tc.yaw, Vector(0.0,1.0,0.0));
	else
		rot_y = Matrix(-info.max_rot_y*dt*tc.yaw, Vector(0.0,1.0,0.0));
	
	acc->set_velocity(rot_y*acc->velocity());
	cm = rot_y*cm;
	//			 rot_vel = rot_y*rot_vel;

	//Dont roll if the pitch is to steep
	float delta_roll =0.0;
	if(fabs(pitch) < 70){
		delta_roll = tc.yaw*50 - angle;

		if(fabs(delta_roll) > info.max_rot_z*dt){
			if(delta_roll < 0)
				delta_roll = -info.max_rot_z*dt;
			else
				delta_roll = info.max_rot_z*dt;
		}

		loc_z = cm.col(2);
		Matrix rot_z(delta_roll, loc_z);
		acc->set_velocity( rot_z*acc->velocity() );
		cm = rot_z*cm;
		//			 rot_vel =rot_z*rot_vel;
	}

	loc_x = cm.col(0);
	Matrix rot_x(info.max_rot_x*tc.pitch*dt,loc_x);
	acc->set_velocity( rot_x*acc->velocity() );
	cm = rot_x*cm;
	//			 rot_vel =rot_x*rot_vel;

	acc->set_rot_velocity(rot_vel);
	
	}//End rotation

	//reset the translation
	cm.pos() = pos;

       
	//Apply the changes
	if(last_ortho > 64){
		last_ortho = 0;
		orthonormalize3(cm);
	}
	last_ortho++;
	acc->set_mtx(cm);

	
/*
	//Fire any weapons
	if(tc.fire){
		ShotBase* shot= weap.fire(sim_time);
		if(shot != 0){
			phys_ref->insert(shot,sim_time);
		}
	}

	if(tc.missile_fire){
		ShotBase* shot= weap_missile.fire(sim_time, tc.current_missile);
		if(shot != 0){
			phys_ref->insert(shot,sim_time);
		}
	}
*/
}




}
/*
void DummyTurret::simulate(double dt) {
	pr[0] = Rotation(tc.yaw,0,1,0);
	pr[1] = Rotation(tc.pitch,0,0,1);
}
*/


}

}
