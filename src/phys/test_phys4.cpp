#include "main/types.h"
#include "main/3d_types.h"
#include <queue>
//#include <locale>
#include <GL/glut.h>
#include <time.h>
#include <iostream.h>
#include <math.h>

using namespace reaper;

namespace phys{

	void update_world(double frame_start,double dt);


	inline float timer() {return (float)clock()/(float)CLOCKS_PER_SEC;}
	double frameTime = 1/20.0, min_dt = 0.005, current_time = 0.0;


	class Object
	{
	public:
		Object() {};
	
		Object(Vector p, Vector v, double r ) 
			: rad(r), pos(p), vel(v), max_acc(5), 
			mass(1), rot_vel(Vector(0.0, 0.0, 0.0)),
			sim_time(0.0) 
			{
				dir.load_identity();	
			}
		Object(Vector p, Vector v, double r, double ma, double m,Vector rot_vel = Vector(0.0, 0.0, 0.0) ) 
			: rad(r), pos(p), vel(v),
			  max_acc(ma),mass(m),rot_vel(rot_vel),
			  sim_time(0.0)
			{
				dir.load_identity();	
			}
	
		double rad;
		Vector pos;
		Vector vel;
		Vector rot_vel;
		Matrix4 dir;
		double max_acc;
		double mass;
		void simulate(double dt);
		double sim_time;

		//Prestanda parametrar, ala krafter, rate of fire o.s.v.

		//Reglage inställningar
        
	};

	class Collision
	{
	public:
		Collision(Object& o1,Object& o2, Vector p) : obj1(o1), obj2(o2), pos(p) {}

		Object &obj1, &obj2;
		Vector pos;
	};

///Håller information om trianglar för kollisionsdetektering med sfär
///Trianglarnas verticer måste specificeras motsols
	class Triangle
	{
	public:
 
		Triangle(Vector p1, Vector p2, Vector p3);

		Vector verts[3];

		Vector norm;

		//Vectorer för edges. edges[0] är vektorn mellan p1 och p2, 
		//edge 2 vektorn mellan p2 och p3, edges[2] mellan p3 och p1
		Vector edges[3];


		//Normaler för planen som går genom normalen för trianglarna och
		//respektive edge. Normalerna går utåt.
		Vector norms[3];
	};

	class Pair {
	public:
		virtual void calc_lower_bound() = 0;
		virtual Collision* check_collision() = 0;
		virtual double get_lower_bound() = 0;
		virtual void simulate() = 0;

		bool operator>(const Pair& rh){ return get_lower_bound() > rh.get_lower_bound(); };
	};

	class ObjectObjectPair : public Pair {
	private:
		Object& first, second;
	};

	class ObjectTrianglePair : public Pair {
	private:
		Object& first;
		Triangle& second;
	};

	class LaserObjectPair : public Pair {
	private:
		Object& first, second;
	};

	class LaserTrianglePair : public Pair {
	private:
		Object& first;
		Triangle& second;
	};


	Triangle::Triangle(Vector p0, Vector p1, Vector p2)
	{
		verts[0] = p0;
		verts[1] = p1;
		verts[2] = p2;

		for(int i=0; i<3; i++)
			edges[i] = verts[(i+1) % 3] - verts[i];

		norm = normalize_3(cross_product(edges[0],edges[1]));

		for(int i=0; i<3; i++)
			norms[i] = normalize_3(cross_product(edges[i],norm));
	}






	bool collides(const Object& obj, const Triangle& tri);

	void Object::simulate(double dt)
	{
	
		//Dummy acceleration
		Vector a1(0.0, 0.0, 0.0);
        
		Vector p1 = pos;

		Vector v1 = vel;

		pos = p1 + dt*v1 + pow(dt,2)*a1*0.5;

		vel = v1 + dt*a1;

		if(length3(rot_vel) > 0){
			dir = Matrix4().rotate(dt*length3(rot_vel),normalize_3(rot_vel) )*dir;
		}


		sim_time+=dt;

	
	}

	class ObjectPair
	{
	private:
		Object &obj1, &obj2;
		static int count;
	public:
		ObjectPair(Object& o1, Object& o2) : obj1(o1), obj2(o2), 
			wake_up(0.0), simulated(0.0) { id = count++; }

		int id;

		void calc_wake_up();
	
		double wake_up;
		double simulated;

		double sim_time();   

		void simulate(double dt);

		void collide(const Collision& col);

		Collision get_collision();

		bool has_collided();

		bool operator<(const ObjectPair& rh) const { return wake_up < rh.wake_up; }
		bool operator>(const ObjectPair& rh) const { return wake_up > rh.wake_up; }

	};

	int ObjectPair::count = 0;

	class Derefless {
	public:
		bool operator()(const ObjectPair* op1, const ObjectPair* op2) { return *op1 > *op2; };
	};

	double ObjectPair::sim_time()
	{
		if(obj1.sim_time > obj2.sim_time)
			return obj1.sim_time;
		else
		{
//                cout << "ObjectPair: Eneven sim_times!" << obj1.sim_time-obj2.sim_time << endl;
			return obj2.sim_time;
		}
	} 

	void ObjectPair::collide(const Collision& col)
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
        
		Vector r_pos1 = col.pos - col.obj1.pos;
		Vector r_pos2 = col.pos - col.obj2.pos;

		Vector n1 = normalize_3(r_pos1);
		//vi behöver bara en normalvektor

		//Hastigheterna i normalriktninen
        
		double v1 = DotProduct3(n1 , col.obj1.vel);
		double v2 = DotProduct3(n1 , col.obj2.vel);

		double m1 = col.obj1.mass;
		double m2 = col.obj2.mass;

		//v4 är nya normalhastigheten för obj2;
		//v3 är nya normalhastigheten för obj1;
		double v4 = -(-m1*v1 -e*m1*v1 + e*m1*v2 -m2*v2)/(m1+m2);
		double v3 = -(-m1*v1 -m2*v2 + m2*v4)/m1;

		obj1.vel += (v3-v1)*n1;
		obj2.vel += (v4-v2)*n1;


		//Nu adderar vi lite rotationer också, för skojs skull
		//Detta görs genom en fulformel. 
		//Antag en kontakttid på 1 hundradels sekund

		//Kontaktpunkten kommer att påverkas av en kraft som ligger
		//i tangentplanet och med samma rikning som medelhastigheten 
		//av objekten

		Vector vp1 = col.obj1.vel +
			cross_product(col.obj1.rot_vel,r_pos1 );

		Vector vp2 = col.obj2.vel +
			cross_product(col.obj2.rot_vel,r_pos2 );
        
		Vector dv = vp2-vp1;

        
		double I1 = 2.5*col.obj1.mass*col.obj1.rad*col.obj1.rad;
		double I2 = 2.5*col.obj2.mass*col.obj2.rad*col.obj2.rad;

		double friction = 0.3;


        
		Vector F = dv*friction;  //Kraftvektor

		double dt = 2.1;
		Vector M1 = cross_product(F,r_pos1);  //Moment
		Vector M2 = cross_product(F,r_pos2);
	


		Vector ra1 = M1/I1;  //rotationsaccelerationer
		Vector ra2 = M2/I2;


		;  //kontakttid

		//Fixa till rotationshastigheten nu då

		obj1.rot_vel -= dt*ra1;
		obj2.rot_vel += dt*ra2;

	}

	Collision ObjectPair::get_collision()
	{
		Vector n1 = normalize_3(obj2.pos-obj1.pos);
		Vector n2 = normalize_3(obj1.pos-obj2.pos);

		n1*=obj1.rad;
		n1+=obj1.pos;
		n2*=obj2.rad;
		n2+=obj2.pos;
	
		return Collision(obj1, obj2, 0.5*(n1+n2) );  //To get the point inbetween
	}

	bool ObjectPair::has_collided()
	{
		return (length3(obj1.pos-obj2.pos)-(obj1.rad+obj2.rad) < 0);
	}

//Calculate the minimum time to collision. Method:
//let v1 be the velocity of object 1
//let a1 be the maximum possible acceleration, including all forces, 
//such as afterburner and gravity. It does not include collisions with other objects
//let r1 be the radius of the object. In noncircular cases, r1 the maximum distance
//from the center of the object and any part of the object.
//
//The other object is similar, but with index 2 instead
//
//Let d be the distance between the centerpoint of the two objects.
//let t is the time we want to solve for
//
//We have the following equation:
//v1*t + a1*t^2/2 + v2*t + a2*t^2/2 + r1 + r2 -d == 0
//If we solve that for t we get two solutions, and you chose one of them (the correct)

	void ObjectPair::calc_wake_up()
	{
		double d = length3(obj1.pos - obj2.pos);
		double t = 0.0;

		//check for zero
		if (!d)
			wake_up = sim_time() + min_dt;
		else{
			double r1 = obj1.rad;
			double r2 = obj2.rad;
                
			double a1 = obj1.max_acc;
			double a2 = obj2.max_acc;
                
			double v1 = length3(obj1.vel);
			double v2 = length3(obj2.vel);
                
			t  =   2*v1 +2*v2 - 
				sqrt( 4*(a1+a2)*( 2*d - 2*r1 - 2*r2) + pow( (-2)*v1 - 2*v2 , 2) );
			t  = t / (-2*(a1+a2));
			if(t>0)
				wake_up = sim_time() + t;
			else
				wake_up = sim_time() + min_dt;
		}
	}

	void ObjectPair::simulate(double dt)
	{
		double until = dt+simulated;
//	obj1.simulate(until-obj1.sim_time);
//	obj2.simulate(until-obj2.sim_time);
	
		obj1.simulate(dt);
		obj2.simulate(dt);

		simulated = until;
	}



	void ObjectTrianglePair::collide(Collision &col)
	{
		//Hastigheterna i normalriktninen

		Vector normal=tri.norm;

		double e = 0.3; //studscoeeficient

		double v = -DotProduct3(normal, obj.vel);
	
		obj.vel += (1+e)*v*normal; 
/*ignorera rotationer vid kollisioner vid mark tills vidare	
  //Antag att skeppet inte roterar	
  Vector dv = obj.vel-v*normal;

        
  double I1 = 2.5*obj.mass*obj.rad*obj.rad;

  double friction = 0.3;
        
  Vector F = dv*obj.mass*friction;  //Kraftvektor

  double dt = 0.2;
  Vector M1 = cross_product(F,-normal*obj.rad);  //Moment

  Vector ra1 = M1/I1;  //rotationsaccelerationer


//	cout << "Vektorer: " << length3(F) << length3(ra2);

        ;  //kontakttid

        //Fixa till rotationshastigheten nu då

	obj.rot_vel += dt*ra1;
*/

	}

	int ObjectTrianglePair::count = 0;

	void ObjectTrianglePair::calc_wake_up()
	{
		double d = fabs(DotProduct3(obj.pos - tri.verts[0], tri.norm)) - obj.rad;
            
		double t = 0.0;

		//check for zero
		if (!d)
			wake_up = sim_time() + min_dt;
		else{
			double r = obj.rad;
 
			double a = obj.max_acc;
                
			double v = length3(obj.vel);

			t = (-v+sqrt(2*a*d+v*v)) / a;
 
			if(t>0)
				wake_up = sim_time() + t;
			else
				wake_up = sim_time() + min_dt;
		}  
	}

	bool ObjectTrianglePair::has_collided()
	{	
		return collides(obj,tri);	
	}



	class DereflessTri {
	public:
		bool operator()(const ObjectTrianglePair* op1, const ObjectTrianglePair* op2) { return *op1 > *op2; };
	};


	std::vector<Object*> objects;

	std::vector<Triangle*> triangles;

//std::priority_queue<ObjectPair*> pairs;
	std::priority_queue<ObjectPair*, std::vector<ObjectPair*>, Derefless> pairs;


//GL stuff
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = {30.0, 30.0, 30.0, 0.0};
	GLfloat amb_light[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat red_mat[] = {1.0, 0.5, 0.5, 1.0};
	GLfloat blue_mat[] = {0.5, 0.5, 1.0, 1.0};


	void display(){

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBegin(GL_TRIANGLES);
		for(int i = 0; i < triangles.size();++i)
		{
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(triangles[i]->verts[0].x, triangles[i]->verts[0].y, triangles[i]->verts[0].z);
			glVertex3f(triangles[i]->verts[1].x, triangles[i]->verts[1].y, triangles[i]->verts[1].z);
			glVertex3f(triangles[i]->verts[2].x, triangles[i]->verts[2].y, triangles[i]->verts[2].z);
		}
		glEnd();

		for(int i = 0; i < objects.size(); i++){
			glPushMatrix();

			glTranslatef(objects[i]->pos.x, objects[i]->pos.y, objects[i]->pos.z);
			glMultMatrixf(objects[i]->dir);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
 
			glutWireSphere(objects[i]->rad, 10, 10);        
			glPopMatrix();
		}
        
		glutSwapBuffers();

		if (current_time > 1000.0)
			exit(0);

		float dt = timer() - current_time;        
		float temp_current=timer();

		double start = timer();	
		update_world(temp_current, dt);
//        cout << "Update: " << timer() - start<<endl;

		current_time = temp_current;
	}

	void gfx_init()
	{
		glEnable(GL_LIGHTING);

		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glLightfv(GL_LIGHT1, GL_AMBIENT, amb_light);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_NORMALIZE);
        
		//Set up camera
		glMatrixMode(GL_PROJECTION);
		gluPerspective( 40, 1.33, 0.01, 100);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(0.0, 0.0, 10,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
	}

	const float velocity = 2.0;




//a är punkt 1, b är punkt 2, edge är b-1, r är punkten vi vill ha
	Vector closestPoint(Vector a, Vector b, Vector edge, Vector r)
	{
		double t = DotProduct3((r - a),edge) / DotProduct3(edge,edge);
	
		//Check boundarys
		if(t > 1) return b;
		if(t < 0) return a;

		return a + t*edge;
	}


//tests if a object collides with a triangle
	bool collides(const Object& obj, const Triangle& tri)
	{
		bool collides = false;
		//Formel: dist = (R-P) dot norm
		//R är punkt i planet, norm är normal, P är punkten att testa

		double dist = DotProduct3( tri.norm, obj.pos - tri.verts[0]);

		if(dist < obj.rad) {
			//Check if inside all edges;
			if( DotProduct3( tri.norms[0], obj.pos - tri.verts[0]) < 0 ) //&&
				//DotProduct3( tri.norms[1], obj.pos - tri.verts[1]) < 0 &&
				//	DotProduct3( tri.norms[2], obj.pos - tri.verts[2]) < 0 )
				collides = true;
		


			Vector Rab = closestPoint(tri.verts[0],tri.verts[1],tri.edges[0],obj.pos);
			Vector Rbc = closestPoint(tri.verts[1],tri.verts[2],tri.edges[1],obj.pos);
			Vector Rca = closestPoint(tri.verts[2],tri.verts[0],tri.edges[2],obj.pos);

			Vector closest = length3(Rab) < length3(Rbc) ? Rab : Rbc;
			closest = length3(closest) < length3(Rca) ? closest : Rca;
		

			if(length3(obj.pos-closest) < obj.rad)
				collides = true;
		}



		return collides;

	}

	void update_world(double frame_start,double tot_time){



		double frame_stop = current_time+tot_time;
		double dt;


		ObjectPair* pair;
		ObjectTrianglePair* objtri;
		std::priority_queue<ObjectTrianglePair*, std::vector<ObjectTrianglePair*>, 
			DereflessTri> tri_pairs; 
        
		for(int i = 0; i < objects.size(); i++){
			for(int j = 0; j < triangles.size();++j){
				objtri = new ObjectTrianglePair(*objects[i],*triangles[j]);
				objtri->calc_wake_up();
				if(objtri->wake_up < frame_stop)
					tri_pairs.push(objtri);
                   
			}
		}

        

		while( (!pairs.empty() && pairs.top()->wake_up < frame_stop )  ||
		       (!tri_pairs.empty() && tri_pairs.top()->wake_up < frame_stop)){
		
			//Now check if we want to simulate a collision between objects 
			if( tri_pairs.empty() || (!pairs.empty() && (pairs.top()->wake_up < tri_pairs.top()->wake_up ))){
			
				pair = pairs.top();
				// cout << "Popped pair: " << pair->id << ".\n";
			
				dt = pair->wake_up - pair->sim_time();
				if( dt < min_dt)
					dt=min_dt;
			
				//the object will be simulated this far when the loo has ended
			
				pairs.pop();
			
				//Pair.simulate simuleras till en viss tid
			
				pair->simulate(dt);
			
				if(pair->has_collided()){
					Collision col = pair->get_collision();
					pair->collide(col);
					pair->simulate(dt);
				
					while(pair->has_collided()){
						pair->simulate(min_dt);
					}
				
				}
				//		cout << "Before -- Pair: " << pair->id << " Wake_up: " << pair->wake_up << ".\n";
				pair->calc_wake_up();
				//		cout << "After -- Pair: " << pair->id << " Wake_up: " << pair->wake_up << ".\n";
			
				pairs.push(pair);// bara om de inte har dött
			}
			else{
				objtri = tri_pairs.top();
			
				dt = objtri->wake_up - objtri->sim_time();
				if( dt < min_dt)
					dt=min_dt;
			
				tri_pairs.pop();
				objtri->simulate(dt);
			

				if(objtri->has_collided()){
				
				//Nu testas om objekten har kolliderat med fler trianglar
					Collision col = objtri->get_collision();
					objtri->collide(col);						

					objtri->simulate(min_dt);
				
					while(objtri->has_collided()){
						objtri->simulate(min_dt);
					}
				
				}


				objtri->calc_wake_up();
			
				tri_pairs.push(objtri);// bara om de inte har dött
			}
		}                
	
        
        
		for(int i = 0; i < objects.size(); i++){
			dt = frame_stop - objects[i]->sim_time;
			if(dt > 0)
				objects[i]->simulate(dt);
		}


	}

}//Namespace

using namespace phys;

int main(int argc, char **argv)	
{
	phys::Triangle* tri;

        tri = new phys::Triangle(Vector( 0.0, -2.0, 2.0),
                Vector( 4.0, -2.0, 2.0),
                Vector( 0.0, -2.0, -6.0) );


        triangles.push_back(tri);

	tri = new phys::Triangle(Vector(0.0, -2.0, 2.0),
			Vector(0.0, -2.0, -6.0),
			Vector(-3.0, -4.0, 2.0));
	triangles.push_back(tri);


	Object* object;

  	object = new Object(Vector(-4.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0), 0.5);

	objects.push_back(object);

	object = new Object(Vector(0.0, -1.0, 0.0), Vector(0.0, 0.4, 0.0), 0.5);

	objects.push_back(object);
        
 //       object = new Object(Vector(4.0, 0.6, 0.0), Vector(-1.0, 0.0, 0.0), 0.5);
      
//	objects.push_back(object); 

  
//	object = new Object(Vector(1.0, 3.0, 0.0), Vector(0.0, -1.1, 0.0), 0.5, 5.0, 1.0);

//	objects.push_back(object);


        object = new Object(Vector(-1.7, 4.0, 0.0), 
		Vector(-0.0, -1.4, 0.0), 
		0.5, 3.0, 1.0);
	
	objects.push_back(object); 
	
	object = new Object(Vector(-0.26, 4.0, 0.0), 
		Vector(-0.0, -1.3, 0.0), 
		0.5, 3.0, 1.0);
	
	objects.push_back(object); 
	object = new Object(Vector(1.26, 5.0, 0.0), 
		Vector(-0.0, -1.6, 0.0), 
		0.5, 3.0, 1.0);
	
	objects.push_back(object); 
	
 //       object = new Object(Vector(-3.0, 0.8, 0.0), Vector(2.0, 0.0, 0.0), 0.5, 5, 10.0);
      
//	objects.push_back(object); 


	for(int i = 0; i < objects.size(); i++)
		for(int j = i + 1; j < objects.size(); j++){
			ObjectPair* pair = new ObjectPair(*objects[i], *objects[j]);
			pair->calc_wake_up();
			pairs.push(pair);
		}

        glutInitWindowSize( 600, 450);        
        glutInit(&argc,argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH );

        glutCreateWindow("Physics Testlab");

	glutDisplayFunc(display);
	glutIdleFunc(display);

	gfx_init();

	current_time= 0.0;
        glutMainLoop();

        return 0;
}
