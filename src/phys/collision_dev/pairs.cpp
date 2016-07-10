#include "os/compat.h"
#include "main/types_ops.h"
#include "object/phys.h"
#include "object/collide.h"
#include "pairs.h"

#include <iostream>
using std::cout;
using std::endl;

using std::min;
using std::max;

using namespace reaper;
using namespace reaper::phys_dev;
using reaper::object::phys::ObjectAccessor;
using reaper::object::CollisionInfo;

namespace reaper{
namespace phys_dev{

Point closestPoint(const Point& a, const Point& b, const Vector& edge, const Point& r);

Pair::Pair() : 
lower_bound(0),
id1(0),
id2(0)
{
	
};


bool Pair::operator<(const Pair& rh) const
{
	if(id1 < rh.id1)
		return true;
	else if(id1 == rh.id1)
		return id2 < rh.id2;
	else
		return false;
}

void DynTriPair::simulate(const double& to_time)
{
	double delta = to_time - obj->get_sim_time();      
	if(delta> 0 )
		obj->simulate(delta);
}


double DynTriPair::check_distance()
{
	//Formel: dist = (R-P) dot norm
	//R är punkt i planet, norm är normal, P är punkten att testa
	const Point& pos = obj->get_pos();

	double dist = dot(tri->normal, pos - tri->a);

	//Check if inside all edges;
	if( dot( tri->norms[0], pos - tri->a) < 0 &&
		dot( tri->norms[1], pos - tri->b) < 0 &&
		dot( tri->norms[2], pos - tri->c) < 0 )
		return dist - obj->get_radius();
	
	Point Rab = closestPoint(tri->a,tri->b,tri->edges[0],pos);
	Point Rbc = closestPoint(tri->b,tri->c,tri->edges[1],pos);
	Point Rca = closestPoint(tri->c,tri->a,tri->edges[2],pos);
	
	Point closest = length(Rab) < length(Rbc) ? Rab : Rbc;
	closest = length(closest) < length(Rca) ? closest : Rca;
	
	
	return length(closest - pos) - obj->get_radius();
	
}

CollisionData* DynTriPair::get_collision(){			

	return 0;
}

void DynTriPair::calc_lower_bound(){
	ObjectAccessor &obj_data = obj->get_physics();

	const double d = fabs(dot(obj->get_pos() - tri->a, tri->normal)) - obj_data.radius;            
	double t = 0.0;

	const double& a = obj_data.max_acc;                
	const double& v = length(obj_data.vel);
	
	t = (-v+sqrt(2*a*d+v*v)) / a;
	
	t = std::max(t,0.01);
	lower_bound = get_sim_time() + min(t, Maximum_Lower_bound);
	
}

bool DynTriPair::collide(const CollisionData&)
{
	const double& e = 0.6;  //stötcoefficient

	//Vid sned central stöt kommer följande att ske. 
	//De individuella objekten kommer att ha samma hastighet i tangentplanet 
	//före som efter stöt. Enbart hastigheten i normalriktingen kommer att ändras.

	//tag först reda på hastigheterna i normalriktningen. Räkna därefer ut de nya 
	//hastigheterna. Addera en hastighetsvektor i normalriktningen som gör att 
	//objekten får de korrekta hastigheterna

	//I detta enkla fall är normalriktningen samma som vektorn från centrum till
	//punkten där kollisionen har skett.
	const ObjectAccessor &obj_data = obj->get_physics() ;

	const Vector r_pos2(-tri->normal*obj->get_radius());
	const Vector n1(norm(-r_pos2));
	
	//vi behöver bara en normalvektor
	//Hastigheterna i normalriktninen

	const Vector& vel_2 = obj_data.vel;
         Vector delta_v = -vel_2;

	//temporary check Are objects REALLY colliding?
	const Vector p2vel = obj_data.vel + cross(r_pos2,obj_data.rot_vel);
	
	if(dot(p2vel,n1) > 0 ){
		return false;
	}

//	double v2 = dot(n1 , vel_2);
	const double& m2 = obj_data.mass;

         double j = dot(-(1+e)*delta_v,n1);
        const double t1 = (1/m2 )*dot(n1,n1);

        //Calculate the intertia tensor and inverse
        Matrix I2(Matrix::id());
        I2 *= 2.5*obj_data.mass*obj_data.radius*obj_data.radius;

        const Matrix inv_I2 = inverse3 (I2);
        const Vector t3 = cross(inv_I2*(cross(r_pos2,n1)),r_pos2);

        j = j/(t1+dot(t3,n1));

        obj_data.vel -= (j/m2)*n1;

        obj_data.rot_vel += inv_I2*(cross(r_pos2,-j*n1));
	
	delta_v = delta_v - obj_data.vel;

	//change the rotation
	//Save the old translation values, rotate aroiund local origin
	Point old_trans(obj_data.m.pos());
	obj_data.m.pos() = Point(0,0,0);

	float angle = 57.3*acos(dot(n1,obj_data.m.col(1)));
	obj_data.m = Matrix(angle+20,obj_data.m.col(0)) * obj_data.m;
	
	//Reset the translation
	obj_data.m.pos() = old_trans;
		
	//Make some damage
	float damage = 8*fabs(dot(-n1,obj_data.m.col(2)));

        //damage += (length(obj_data.vel)/obj_data.max_vel);

        if(length_sq(obj_data.vel) < 16) {
                damage = 0.1;
	}
        //dout << "Damage: " << damage << std::endl;
        obj->collide(CollisionInfo(damage,0));


	return true;
}

CollisionData* ShotTriPair::get_collision()
{
	return 0;
}

void ShotTriPair::simulate(const double& to_time)
{		
        const double delta = to_time - obj->get_sim_time();
	if(delta > 0)
		obj->simulate(delta);
}


double ShotTriPair::check_distance()
{
		//Formel: dist = (R-P) dot norm
	//R är punkt i planet, norm är normal, P är punkten att testa
	const Point& pos = obj->get_pos();

	double dist = dot(tri->normal, pos - tri->a);

	//Check if inside all edges;
	if( dot( tri->norms[0], pos - tri->a) < 0 &&
		dot( tri->norms[1], pos - tri->b) < 0 &&
		dot( tri->norms[2], pos - tri->c) < 0 )
		return dist - obj->get_radius();
	
	Point Rab = closestPoint(tri->a,tri->b,tri->edges[0],pos);
	Point Rbc = closestPoint(tri->b,tri->c,tri->edges[1],pos);
	Point Rca = closestPoint(tri->c,tri->a,tri->edges[2],pos);
	
	Point closest = length(Rab) < length(Rbc) ? Rab : Rbc;
	closest = length(closest) < length(Rca) ? closest : Rca;
	
	
	return length(closest-pos) - obj->get_radius();
}

void ShotTriPair::calc_lower_bound(){

	const Point& p1 = obj->get_pos();
	const Point p2(p1 + obj->get_velocity());

	
	float &A = tri->normal.x;
	float &B = tri->normal.y;
	float &C = tri->normal.z;
	float D = -(tri->normal.x * tri->a.x + 
				 tri->normal.y * tri->a.y + 
				 tri->normal.z * tri->a.z );

	double my;

	double denom = A*(p1.x-p2.x) + B*(p1.y-p2.y) + C*(p1.z-p2.z);
	double nom = D + A*p1.x + B*p1.y + C*p1.z;
	if(denom == 0)
		my = -10e10;  //Parallel med planet, kommer då inte att träffa
	else
		my = nom/denom;
            
	double t = 0.0;
			
	t = my/length(obj->get_velocity()); 
	
	if(my<0)
		lower_bound = 10e10; //Negativ line, will intersect behind.
	
	else{
		t = std::max(t,0.01);
		lower_bound = get_sim_time() + min(t, Maximum_Lower_bound);
	}
}

bool ShotTriPair::collide(const CollisionData&)
{
        obj->collide(object::CollisionInfo(1,0));//Dummy damage just to kill
	return true;
}



double DynDynPair::get_sim_time() const
{
	return std::min(obj1->get_sim_time(), obj2->get_sim_time());
}

void DynDynPair::simulate(const double& to_time)
{
	double  delta;
	delta = to_time - obj1->get_sim_time();
	

	if(delta > 0) 
		obj1->simulate(delta);
	
	delta = to_time - obj2->get_sim_time();

	if(delta > 0)
		obj2->simulate(delta);
}

//Temporary sphereical approx!
double DynDynPair::check_distance()
{
        return  length(obj1->get_pos() - obj2->get_pos() ) -
		(obj1->get_radius() + obj2->get_radius() ) ;
}

CollisionData* DynDynPair::get_collision(){
	Vector n1 = norm(obj2->get_pos()-obj1->get_pos());
	Vector n2 = norm(obj1->get_pos()-obj2->get_pos());
			
	n1 *= obj1->get_radius();
	n1 += obj1->get_pos();
	n2 *= obj2->get_radius();
	n2 += obj2->get_pos();
			
	return new CollisionData( 0.5*(n1+n2) );
}

using reaper::object::phys::ObjectAccessor;

void DynDynPair::calc_lower_bound(){
	ObjectAccessor &obj1_data = obj1->get_physics() ;
	ObjectAccessor &obj2_data = obj2->get_physics() ;

	double d = length(obj1->get_pos() - obj2->get_pos());
	double t = 0.0;

		
	if(d == 0)
		lower_bound = get_sim_time();
	else {
		double r1 = obj1_data.radius;
		double r2 = obj2_data.radius;
			
		double a1 = obj1_data.max_acc;
		double a2 = obj2_data.max_acc;
		
			
		double v1 = length(obj1_data.vel);
		double v2 = length(obj2_data.vel);
			
		t  =   v1 +v2 - 
			sqrt( 2*(a1+a2)*( d - r1 - r2) + pow( v1 + v2 , 2) );
		t  = t / (-(a1+a2));

                //Also calculate the alternativ, using max_vel all the time
                double t2 = (d-r1-r2- Collision_Distance/4)/(obj1_data.max_vel + obj2_data.max_vel);
		
		t = std::min(t,t2);
		t = max(t,Minimum_Dynamic_step);


		lower_bound = get_sim_time() + min(t, Maximum_Lower_bound);
	}
}

bool DynDynPair::collide(const CollisionData& col)
{
	double e = 0.4;  //stötcoefficient

	//Vid sned central stöt kommer följande att ske. 
	//De individuella objekten kommer att ha samma hastighet i tangentplanet 
	//före som efter stöt. Enbart hastigheten i normalriktingen kommer att ändras.

	//tag först reda på hastigheterna i normalriktningen. Räkna därefer ut de nya 
	//hastigheterna. Addera en hastighetsvektor i normalriktningen som gör att 
	//objekten får de korrekta hastigheterna

	//I detta enkla fall är normalriktningen samma som vektorn från centrum till
	//punkten där kollisionen har skett.
	ObjectAccessor &obj1_data = obj1->get_physics() ;
	ObjectAccessor &obj2_data = obj2->get_physics() ;


	Vector r_pos1 = col.pos - obj1->get_pos();
	Vector r_pos2 = col.pos - obj2->get_pos();

	//temporary check Are objects REALLY colliding?
	Vector p2vel = obj2_data.vel + cross(r_pos2,obj2_data.rot_vel);
	Vector p1vel = obj1_data.vel + cross(r_pos1,obj1_data.rot_vel);

	if( -dot(p2vel,r_pos1) > dot(p1vel,r_pos1) ){
		return false;
	}

	Vector n1 = norm(r_pos1);
	//vi behöver bara en normalvektor

	//Hastigheterna i normalriktninen

	Vector vel_1 = obj1_data.vel;
	Vector vel_2 = obj2_data.vel;
        Vector delta_v = vel_1 - vel_2;

//	double v1 = dot(n1 , vel_1);
//	double v2 = dot(n1 , vel_2);

	double m1 = obj1_data.mass;
	double m2 = obj2_data.mass;

        double j = dot(-(1+e)*delta_v,n1);
        double t1 = ( 1/m1 + 1/m2 )*dot(n1,n1);

        Matrix I1(Matrix::id()),I2(Matrix::id());

        I1 *= (float)2.5*obj1_data.mass*obj1_data.radius*obj1_data.radius;
        I2 *= 2.5*obj2_data.mass*obj2_data.radius*obj2_data.radius;

        Matrix inv_I1 = inverse3 (I1);
        Matrix inv_I2 = inverse3 (I2);

        Vector t2 = cross(inv_I1*(cross(r_pos1,n1)),r_pos1);
        Vector t3 = cross(inv_I2*(cross(r_pos2,n1)),r_pos2);

        j = j/(t1+dot(t2+t3,n1));

        obj1_data.vel += (j/m1)*n1;
        obj2_data.vel -= (j/m2)*n1;

        obj1_data.rot_vel += inv_I1*(cross(r_pos1,j*n1));
        obj2_data.rot_vel += inv_I2*(cross(r_pos2,-j*n1));


	float delta_v_1 = length( vel_1 - obj1_data.vel);
	float delta_v_2 = length( vel_2 - obj2_data.vel);



	 //Make som damage to the ships
	float damage1 =  delta_v_1 *0.1 + obj2_data.damage;
	float damage2 =  delta_v_2 *0.1 + obj1_data.damage;

	obj1->collide(CollisionInfo(damage1,0));
	obj2->collide(CollisionInfo(damage2,0));

	return true;
}

double SillyDynPair::get_sim_time() const
{
	return obj2->get_sim_time();
}

void SillyDynPair::simulate(const double& to_time)
{
        double delta = to_time - obj2->get_sim_time();
	if(delta > 0)
		obj2->simulate(delta);
}

double SillyDynPair::check_distance()
{
	return length(obj1->get_pos()-obj2->get_pos()) - 
	       (obj1->get_radius()+obj2->get_radius() );
}

CollisionData* SillyDynPair::get_collision(){
	Vector n1 = norm(obj2->get_pos()-obj1->get_pos());
	Vector n2 = norm(obj1->get_pos()-obj2->get_pos());
			
	n1 *= obj1->get_radius();
	n1 += obj1->get_pos();
	n2 *= obj2->get_radius();
	n2 += obj2->get_pos();
			
	return new CollisionData(0.5*(n1+n2));
}

void SillyDynPair::calc_lower_bound()
{

	ObjectAccessor &obj2_data =obj2->get_physics();

	double d = length(obj1->get_pos() - obj2->get_pos());
	double t = 0.0;
		
	if (d == 0)
		lower_bound = get_sim_time();
	else{
		double r1 = obj1->get_radius();
		double r2 = obj2_data.radius;
			
//		double a1 = 0;
		double a2 = obj2_data.max_acc;
			
//		double v1 = 0;
		double v2 = length(obj2_data.vel);
			
		t  =   v2 - 
			sqrt( 2*(a2)*( d - r1 - r2) + v2*v2 );
		t  = t / (-(a2));

                //Also calculate the alternativ, using max_vel all the time
                double t2 = (d-r1-r2)/(obj2_data.max_vel);

                t = std::max(t,t2);
		t = std::max(t,0.01);
		lower_bound = get_sim_time() + min(t, Maximum_Lower_bound);
	}
}

bool SillyDynPair::collide(const CollisionData& col)
{
	double e = 0.4;  //stötcoefficient

	//Vid sned central stöt kommer följande att ske. 
	//De individuella objekten kommer att ha samma hastighet i tangentplanet 
	//före som efter stöt. Enbart hastigheten i normalriktingen kommer att ändras.

	//tag först reda på hastigheterna i normalriktningen. Räkna därefer ut de nya 
	//hastigheterna. Addera en hastighetsvektor i normalriktningen som gör att 
	//objekten får de korrekta hastigheterna

	//I detta enkla fall är normalriktningen samma som vektorn från centrum till
	//punkten där kollisionen har skett.

	ObjectAccessor &obj2_data = obj2->get_physics() ;


	Vector r_pos1 = col.pos - obj1->get_pos();
	Vector r_pos2 = col.pos - obj2->get_pos();


	//temporary check Are objects REALLY colliding?
	Vector p2vel = obj2_data.vel + cross(r_pos2,obj2_data.rot_vel);
	
	if(dot(p2vel,r_pos1) > 0 ){
		return false;
	}

	Vector n1 = norm(r_pos1);
	//vi behöver bara en normalvektor


	//Hastigheterna i normalriktninen

        Vector delta_v = -obj2_data.vel;


//	double v2 = dot(n1 , delta_v);

	double m2 = obj2_data.mass;

        double j = dot(-(1+e)*delta_v,n1);
        double t1 = (1/m2 )*dot(n1,n1);

        Matrix I2(Matrix::id());

        I2 *= 2.5*obj2_data.mass*obj2_data.radius*obj2_data.radius;

        Matrix inv_I2 = inverse3 (I2);

        Vector t3 = cross(inv_I2*(cross(r_pos2,n1)),r_pos2);

        j = j/(t1+dot(t3,n1));

        obj2_data.vel -= (j/m2)*n1;

        obj2_data.rot_vel += inv_I2*(cross(r_pos2,-j*n1));


	//Skada
	float delta_v_2 = length(delta_v - obj2_data.vel);
	float damage1 =  delta_v_2 *0.15 + obj2_data.damage;
	float damage2 =  delta_v_2 *0.15;

	obj1->collide(CollisionInfo(damage1, 0));
	obj2->collide(CollisionInfo(damage2, 0));

	return true;
}

void StaticDynPair::simulate(const double& to_time)
{
	double  delta;
	delta = to_time - obj1->get_sim_time();
	if(delta > 0)
		obj1->simulate(delta);
	
	delta = to_time - obj2->get_sim_time();
	if(delta >0)
		obj2->simulate(delta);
}

double ShotSillyPair::get_sim_time() const
{
	return shot->get_sim_time();
}

double ShotStaticPair::get_sim_time() const
{
	return std::min(shot->get_sim_time(), obj->get_sim_time());
}

void ShotSillyPair::simulate(const double& to_time)
{
	const double delta = to_time - shot->get_sim_time();
	if(delta > 0)
		shot->simulate(delta);
}

void ShotStaticPair::simulate(const double& to_time)
{
	double  delta;
	delta = to_time - shot->get_sim_time();
	if(delta > 0)
		shot->simulate(delta);
	
	delta = to_time - obj->get_sim_time();
	if(delta >0)
		obj->simulate(delta);
}

double ShotSillyPair::check_distance(){

	return  length(shot->get_pos()-obj->get_pos() ) - 
                (shot->get_radius() + obj->get_radius() ) ;
}

CollisionData* ShotSillyPair::get_collision(){
	Vector n1 = norm(obj->get_pos()-shot->get_pos());
	Vector n2 = norm(shot->get_pos()-obj->get_pos());
			
	n1 *= shot->get_radius();
	n1 += shot->get_pos();
	n2 *= obj->get_radius();
	n2 += obj->get_pos();
			
	return new CollisionData(0.5*(n1+n2));
}	

void ShotSillyPair::calc_lower_bound(){
	
	
	double d = length(shot->get_pos() - obj->get_pos());
	double t = 0.0;
		
	if (d == 0)
		lower_bound = get_sim_time();
	else{
		double r1 = shot->get_radius();
		double r2 = obj->get_radius();
						
		double v1 = length(shot->get_velocity());
//		double v2 = 0;
			
		t  =   -(-d + r1 +r2)/v1;
		
		t = max(t,0.01);
		lower_bound = get_sim_time() + min(t,Maximum_Lower_bound);

	}
	
}

void ShotDynPair::calc_lower_bound(){

	const ObjectAccessor &obj_data = obj->get_physics();

	double d = length(shot->get_pos() - obj->get_pos());
	double t = 0.0;
		
	if (d == 0)
		lower_bound = get_sim_time();
	else{
		const double& r1 = shot->get_radius();
		const double& r2 = obj_data.radius;
			
		const double& a2 = obj_data.max_acc;
		
			
		const double& v1 = length(shot->get_velocity());
		const double& v2 = length(obj_data.vel);
			
		t  =   v1 +v2 - 
			sqrt( 2*(a2)*( d - r1 - r2) + pow( v1 + v2 , 2) );
		t  = t / (-a2);
         

                //Also calculate the alternativ, using max_vel all the time
                double t2 = (d-r1-r2)/(v1 + obj_data.max_vel);

                t = std::max(t,t2);
		t = std::max(0.0, t);

		lower_bound = get_sim_time() + std::min(t,Maximum_Lower_bound);
	}
        
}

bool ShotSillyPair::collide(const CollisionData&)
{	
        object::CollisionInfo ci(shot->get_damage(),0);
        obj->collide(ci);
        shot->collide(ci);
	return true;
}



//a är punkt 1, b är punkt 2, edge är b-1, r är punkten vi vill ha
Point closestPoint(const Point& a, const Point& b, const Vector& edge, const Point& r)
{
  double t = dot((r - a),edge) / dot(edge,edge);
  
  //Check boundarys
  if(t > 1) return b;
  if(t < 0) return a;
  
  return a + t*edge;
}

}
}
