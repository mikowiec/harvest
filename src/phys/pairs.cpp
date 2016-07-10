
#include "os/compat.h"

#include "os/debug.h"

#include "main/types_ops.h"
#include "main/types_io.h"
#include "phys/pairs.h"

#include "phys/phys.h"
#include "float.h"

#include <iostream>

using std::cout;
using std::endl;

using std::min;
using std::max;

using namespace reaper;
using namespace reaper::phys;


namespace reaper{
namespace phys{
namespace {
	debug::DebugOutput dout("pairs", 5);
}


Point closestPoint(const Point& a, const Point& b, const Vector& edge, const Point& r);

Pair::Pair() : 
lower_bound(0),
id1(0),
id2(0)
{
	
};


Pair::~Pair()
{ }

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

double tri_point_distance(const world::Triangle* tri, Point pos, float rad)
{
	//Formel: dist = (R-P) dot norm
	//R är punkt i planet, norm är normal, P är punkten att testa

	double dist = dot(tri->normal, pos - tri->a);


	//Check if inside all edges;
	if( dot( tri->norms[0], pos - tri->a) < 0 &&
		dot( tri->norms[1], pos - tri->b) < 0 &&
		dot( tri->norms[2], pos - tri->c) < 0 ) {
		return dist - rad;
	}
	
	Vector Vab = closestPoint(tri->a,tri->b,tri->edges[0],pos) - pos;
	Vector Vbc = closestPoint(tri->b,tri->c,tri->edges[1],pos) - pos;
	Vector Vca = closestPoint(tri->c,tri->a,tri->edges[2],pos) - pos;
	float lab(length_sq(Vab)), lbc(length_sq(Vbc)), lca(length_sq(Vca));

	float closest_dist_sq = (lab < lbc) ? min(lab, lca) : min(lbc, lca);

	return sqrt(closest_dist_sq) - rad;
}

double DynTriPair::check_distance()
{
	// FIXME, abs should not be necessary... -peter
	return fabs(tri_point_distance(tri, obj->get_mtx().pos(), obj->get_radius()));
}


CollisionData* DynTriPair::get_collision(){			

	return 0;
}

bool DynTriPair::to_insert(const double& fs)
{
	return lower_bound < fs;
}

struct ObjDynInfo {
	float r;  // radius
	double a; // max acceleration
	Vector v; // velocity vector
	double l; // length of velocity vector
	Vector rv; // Rotation velocity
	float mv; // Max velocity

	ObjDynInfo(DynAccessor* d)
	 : r(d->get_radius()),
	   a(d->max_acc()),
	   v(d->get_velocity()),
	   l(length(v)),
	   rv(d->get_rotvel()),
	   mv(d->max_vel())
	{ }
};




void DynTriPair::calc_lower_bound(){
	ObjDynInfo di(obj);
	Point pos = obj->get_mtx().pos();
//	const double d = fabs(dot(obj->get_mtx().pos() - tri->a, tri->normal) - di.r);
	double d = check_distance();
	double t = 0.0;
	const double a = di.a;
	const double v = di.l;
	
	t = (-v+sqrt(2*a*d+v*v)) / a;
	
	t = std::max(t,Minimum_Static_step);
	lower_bound = get_sim_time() + min(t, Maximum_Lower_bound);
	if (isnan(lower_bound)) {
		dout << "dyntri! " << d << ' ' << a << ' ' << v << ' ' << t << '\n';
		dout << "dist: " << check_distance() << '\n';
		lower_bound = get_sim_time() + Minimum_Static_step;
	}
	
}

bool DynTriPair::collide(const CollisionData*)
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

	ObjDynInfo di(obj);
	const Vector r_pos2(-tri->normal*di.r);
	const Vector n1(norm(-r_pos2));
	
	//vi behöver bara en normalvektor
	//Hastigheterna i normalriktninen

	const Vector& vel_2 = di.v;
	const Vector& rotvel_2 = di.rv;
	Vector delta_v = -vel_2;

	//temporary check Are objects REALLY colliding?
	const Vector p2vel = di.v + cross(r_pos2,rotvel_2);
	
	if(dot(p2vel,n1) > 0 ){
		return false;
	}

//	double v2 = dot(n1 , vel_2);
	float m2 = obj->get_mass();
	float radius = di.r;

         double j = dot(-(1+e)*delta_v,n1);
        const double t1 = (1/m2 )*dot(n1,n1);

        //Calculate the intertia tensor and inverse
        Matrix I2(Matrix::id());
        I2 *= 2.5 * m2 * radius * radius;

        const Matrix inv_I2 = inverse3 (I2);
        const Vector t3 = cross(inv_I2*(cross(r_pos2,n1)),r_pos2);

        j = j/(t1+dot(t3,n1));

        obj->set_velocity(vel_2 - (j/m2)*n1);

        obj->set_rotvel(rotvel_2 + inv_I2*(cross(r_pos2,-j*n1)));
	
	delta_v = delta_v - vel_2;

	// change the rotation
	Matrix3x3 m(obj->get_mtx().rot());

	float angle = 57.3*acos(dot(n1, m.col(1)));
	m = Matrix3x3(angle+20, m.col(0)) * m;
	
	obj->set_mtx(Matrix(m, obj->get_mtx().pos()));


	//Make some damage
	float damage = 8*fabs(dot(-n1,obj->get_mtx().col(2)));

        //damage += (length(obj->get_velocity())/obj_data.max_vel);

        if(di.l < 4) {
                damage = 0.1;
	}
        //dout << "Damage: " << damage << std::endl;
        obj->collide(damage, 0);


	return true;
}

CollisionData* ShotTriPair::get_collision()
{
	return 0;
}

void ShotTriPair::simulate(const double& to_time)
{		
        const double delta = to_time - obj->get_sim_time();
	double d1 = check_distance();
	if(delta > 0)
		obj->simulate(delta);
	double d2 = check_distance();
}

bool ShotTriPair::to_insert(const double& fs)
{
	return lower_bound < fs;
}

double ShotTriPair::check_distance()
{
	return tri_point_distance(tri, obj->get_mtx().pos(), obj->get_radius());
}


void ShotTriPair::calc_lower_bound() {
        double min_step = ( 2*obj->get_radius() ) / length(obj->get_velocity());

        const Point p1 = obj->get_mtx().pos();
        const Vector d = norm(obj->get_velocity());
        const Vector s = p1 - tri->a;
        
        const Vector e1 = tri->edges[0];
        const Vector e2 = -tri->edges[2];
        
        const Vector p = cross(d,e2);
        const double a = dot(p,e1);
        const double f = 1/a;

        const double u = f*dot(p,s);
        if (u < 0 || u > 1){
                lower_bound = 10e10;
                return;
        }

        const Vector s2 = cross(s,e1);  
        const double v = f*dot(s2,d);
        if (v < 0 || (u+v) > 1){
                lower_bound = 10e10;
                return;
        }

        // Else we will hit the triangle, calculate when.
        double t = f*dot(s2,e2);

        t = t / length(obj->get_velocity());
        t = max(min_step,t);
        lower_bound = get_sim_time() + min(t,Maximum_Lower_bound);
	if (isnan(lower_bound)) {
		dout << "shottri! (nan) " << obj->get_velocity()
		     << ' ' << d << ' ' << a << ' ' << v << ' ' << t << '\n';
		lower_bound = get_sim_time() + Minimum_Static_step;
	}
}

bool ShotTriPair::collide(const CollisionData*)
{
        obj->collide(1, 0);
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
        float dist = length(obj1->get_mtx().pos() - obj2->get_mtx().pos() ) -
			(obj1->get_radius() + obj2->get_radius() ) ;
	return dist;
}

CollisionData* DynDynPair::get_collision(){
	Vector n1 = norm(obj2->get_mtx().pos() - obj1->get_mtx().pos());
	Vector n2 = norm(obj1->get_mtx().pos() - obj2->get_mtx().pos());

	n1 *= obj1->get_radius();
	n1 += obj1->get_mtx().pos();
	n2 *= obj2->get_radius();
	n2 += obj2->get_mtx().pos();
			
	return new CollisionData( 0.5*(n1+n2) );
}



void DynDynPair::calc_lower_bound(){

	double d = length(obj1->get_mtx().pos() - obj2->get_mtx().pos());
	double t = 0.0;


	if(d == 0)
		lower_bound = get_sim_time() + 0.01;
	else {
		ObjDynInfo d1(obj1), d2(obj2);
		double r1 = d1.r, r2 = d2.r;
		double a1 = d1.a, a2 = d2.a;
		double v1 = d1.l, v2 = d2.l;

		t  =   v1 +v2 - 
			sqrt( 2*(a1+a2)*( d - r1 - r2) + pow( v1 + v2 , 2) );
		t  = t / (-(a1+a2));

                //Also calculate the alternativ, using max_vel all the time
                double t2 = (d-r1-r2) / (d1.mv + d2.mv);

		t = std::min(t,t2);
		t = max(t2,5*Minimum_Dynamic_step);
		
//		cout << "Calculating for " << id1 << " " << id2 << " : " << t << endl;

		lower_bound = get_sim_time() + min(t, Maximum_Lower_bound);
	}

	if (isnan(lower_bound)) {
		dout << "dyndyn! " << d << ' ' << t << '\n';
		lower_bound = get_sim_time() + Minimum_Static_step;
	}
}

bool DynDynPair::collide(const CollisionData* col)
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


	Vector r_pos1 = col->pos - obj1->get_mtx().pos();
	Vector r_pos2 = col->pos - obj2->get_mtx().pos();

	//temporary check Are objects REALLY colliding?
	Vector p2vel = obj2->get_velocity() + cross(r_pos2,obj2->get_rotvel());
	Vector p1vel = obj1->get_velocity() + cross(r_pos1,obj1->get_rotvel());

	double d1 = dot(-p2vel,norm(r_pos1) );
	double d2 = dot(p1vel,norm(r_pos1)) ;

	if( (d1 < 0 && d1 > d2)|| (d1 < 0 && d2 < d1 )) {
		return false;
	}

	Vector n1 = norm(r_pos1);
	//vi behöver bara en normalvektor

	//Hastigheterna i normalriktninen

	Vector vel_1 = obj1->get_velocity();
	Vector vel_2 = obj2->get_velocity();
        Vector delta_v = vel_1 - vel_2;

//	double v1 = dot(n1 , vel_1);
//	double v2 = dot(n1 , vel_2);

	double m1 = obj1->get_mass();
	double m2 = obj2->get_mass();

        double j = dot(-(1+e)*delta_v,n1);
        double t1 = ( 1/m1 + 1/m2 )*dot(n1,n1);

        Matrix I1(Matrix::id()),I2(Matrix::id());

	float r1 = obj1->get_radius();
	float r2 = obj2->get_radius();
	
        I1 *= 2.5 * m1 * r1 * r1;
        I2 *= 2.5 * m2 * r2 * r2;

        Matrix inv_I1 = inverse3 (I1);
        Matrix inv_I2 = inverse3 (I2);

        Vector t2 = cross(inv_I1*(cross(r_pos1,n1)),r_pos1);
        Vector t3 = cross(inv_I2*(cross(r_pos2,n1)),r_pos2);

        j = j/(t1+dot(t2+t3,n1));

        obj1->set_velocity(vel_1 + (j/m1)*n1);
        obj2->set_velocity(vel_2 - (j/m2)*n1);

        obj1->set_rotvel(obj1->get_rotvel() + inv_I1*(cross(r_pos1,j*n1)));
        obj2->set_rotvel(obj2->get_rotvel() + inv_I2*(cross(r_pos2,-j*n1)));


	float delta_v_1 = length( vel_1 - obj1->get_velocity());
	float delta_v_2 = length( vel_2 - obj2->get_velocity());



	 //Make som damage to the ships
	float damage1 =  delta_v_1 *0.1 + obj2->get_damage();
	float damage2 =  delta_v_2 *0.1 + obj1->get_damage();

	obj1->collide(damage1, 0);
	obj2->collide(damage2, 0);

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
	return length(obj1->get_mtx().pos()-obj2->get_mtx().pos()) - 
	       (obj1->get_radius()+obj2->get_radius() );
}

CollisionData* SillyDynPair::get_collision(){
	Vector n1 = norm(obj2->get_mtx().pos()-obj1->get_mtx().pos());
	Vector n2 = norm(obj1->get_mtx().pos()-obj2->get_mtx().pos());
			
	n1 *= obj1->get_radius();
	n1 += obj1->get_mtx().pos();
	n2 *= obj2->get_radius();
	n2 += obj2->get_mtx().pos();
			
	return new CollisionData(0.5*(n1+n2));
}

void SillyDynPair::calc_lower_bound()
{

	double d = length(obj1->get_mtx().pos() - obj2->get_mtx().pos());
	double t = 0.0;
		
	if (d == 0)
		lower_bound = get_sim_time() + 0.01;
	else{
		double r1 = obj1->get_radius();
		double r2 = obj2->get_radius();
			
//		double a1 = 0;
		double a2 = obj2->max_acc();
			
//		double v1 = 0;
		double v2 = length(obj2->get_velocity());
			
		t  =   v2 - 
			sqrt( 2*(a2)*( d - r1 - r2) + v2*v2 );
		t  = t / (-(a2));

                //Also calculate the alternativ, using max_vel all the time
                double t2 = (d-r1-r2)/(obj2->max_vel());

                t = std::max(t,t2);
		t = std::max(t,Minimum_Static_step);
		lower_bound = get_sim_time() + min(t, Maximum_Lower_bound);
	}

	if (isnan(lower_bound)) {
		dout << "sillydyn! " << d << ' ' << t << '\n';
		lower_bound = get_sim_time() + Minimum_Static_step;
	}
}

bool SillyDynPair::collide(const CollisionData* col)
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


	Vector r_pos1 = col->pos - obj1->get_mtx().pos();
	Vector r_pos2 = col->pos - obj2->get_mtx().pos();


	//temporary check Are objects REALLY colliding?
	Vector p2vel = obj2->get_velocity() + cross(r_pos2,obj2->get_rotvel());
	
	if(dot(p2vel,r_pos1) > 0 ){
		return false;
	}

	Vector n1 = norm(r_pos1);
	//vi behöver bara en normalvektor


	//Hastigheterna i normalriktninen

        Vector delta_v = -obj2->get_velocity();


//	double v2 = dot(n1 , delta_v);

	double m2 = obj2->get_mass();

        double j = dot(-(1+e)*delta_v,n1);
        double t1 = (1/m2 )*dot(n1,n1);

        Matrix I2(Matrix::id());

	float r2 = obj2->get_radius();
        I2 *= 2.5*obj2->get_mass() * r2 * r2;

        Matrix inv_I2 = inverse3 (I2);

        Vector t3 = cross(inv_I2*(cross(r_pos2,n1)),r_pos2);

        j = j/(t1+dot(t3,n1));

        obj2->set_velocity(-delta_v - (j/m2)*n1);

        obj2->set_rotvel(obj2->get_rotvel() + inv_I2*(cross(r_pos2,-j*n1)));


	//Skada
	float delta_v_2 = length(delta_v - obj2->get_velocity());
	float damage1 =  delta_v_2 *0.15 + obj2->get_damage();
	float damage2 =  delta_v_2 *0.15;

	obj1->collide(damage1, 0);
	obj2->collide(damage2, 0);

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

	return  length(shot->get_mtx().pos()-obj->get_mtx().pos() ) - 
                (shot->get_radius() + obj->get_radius() ) ;
}

CollisionData* ShotSillyPair::get_collision(){
	Vector n1 = norm(obj->get_mtx().pos()-shot->get_mtx().pos());
	Vector n2 = norm(shot->get_mtx().pos()-obj->get_mtx().pos());
			
	n1 *= shot->get_radius();
	n1 += shot->get_mtx().pos();
	n2 *= obj->get_radius();
	n2 += obj->get_mtx().pos();
			
	return new CollisionData(0.5*(n1+n2));
}	

void ShotSillyPair::calc_lower_bound(){
	
	
	double d = length(shot->get_mtx().pos() - obj->get_mtx().pos());
	double t = 0.0;
	
	double r1 = shot->get_radius();
	double r2 = obj->get_radius();
	
	double v1 = length(shot->get_velocity());
	
	t  =   (d - r1 - r2)/v1;
	
	t = max(t,Minimum_Static_step);
	
	lower_bound = get_sim_time() + min(t,Maximum_Lower_bound);
	

	if (isnan(lower_bound)) {
		dout << "shotsilly! " << d << ' ' << t << '\n';
		lower_bound = get_sim_time() + Minimum_Static_step;
	}
}

void ShotDynPair::calc_lower_bound(){

	double d = length(shot->get_mtx().pos() - obj->get_mtx().pos());
	double t = 0.0;
		
	if (d == 0)
		lower_bound = get_sim_time() + 0.01;
	else{
		const double& r1 = shot->get_radius();
		const double& r2 = obj->get_radius();
			
		const double& a2 = obj->max_acc();
		
		
		const double& v1 = length(shot->get_velocity());
		const double& v2 = length(obj->get_velocity());
			
		t  =   v1 +v2 - 
			sqrt( 2*(a2)*( d - r1 - r2) + pow( v1 + v2 , 2) );
		t  = t / (-a2);
         

                //Also calculate the alternativ, using max_vel all the time
                double t2 = (d-r1-r2)/(v1 + obj->max_vel());

                t = std::min(t,t2);
		t = std::max(Minimum_Shot_step, t);
		
		lower_bound = get_sim_time() + std::min(t,Maximum_Lower_bound);
	}
        

	if (isnan(lower_bound)) {
		dout << "shotdyn! " << d << ' ' << t << '\n';
		lower_bound = get_sim_time() + Minimum_Static_step;
	}
}

bool ShotSillyPair::collide(const CollisionData*)
{	
	float damage = shot->get_damage();
        obj->collide(damage, 0);
        shot->collide(damage, 0);
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
