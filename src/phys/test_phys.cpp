#include "main/types.h"
#include "main/3d_types.h"
#include <queue>
//#include <locale>
#include <GL/glut.h>
#include <time.h>
#include <iostream.h>

using namespace reaper;


void update_world(double frame_start,double dt);

inline float timer() {return (float)clock()/(float)CLOCKS_PER_SEC;}



double frameTime = 1/20.0, min_dt = 0.00005, current_time = 0.0;

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
	double max_acc;
	double mass;
	Vector rot_vel;
	Matrix4 dir;
	void simulate(double dt);
	double sim_time;
        
};

void Object::simulate(double dt)
{
	
	//Dummy acceleration
	Vector a1(0.0, 0.0, 0.0);
        
        Vector p1 = pos;

        Vector v1 = vel;

        pos = p1 + dt*v1 + pow(dt,2)*a1*0.5;

        vel = v1 + dt*a1;

	if(length3(rot_vel) > 0){
		dir = Matrix4().rotate(length3(rot_vel),normalize_3(rot_vel) )*dir;
	}
//	cout << "Lenght: " << Length3(rot_vel);

	sim_time+=dt;

	
}

class Collision
{
public:
	Collision(Object& o1,Object& o2, Vector p) : obj1(o1), obj2(o2), pos(p) {}

	Object &obj1, &obj2;
	Vector pos;
};

class ObjectPair
{
private:
	Object &obj1, &obj2;
	static int count;
public:
	ObjectPair(Object& o1, Object& o2) : obj1(o1), obj2(o2), wake_up(0.0) { id = count++; }

 	int id;

	void calc_wake_up();
	
	double wake_up;

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
        if(obj1.sim_time==obj2.sim_time)
                return obj2.sim_time;
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
        
        Vector dv = vp1-vp2;

        
        double I1 = 2.5*col.obj1.mass*col.obj1.rad*col.obj1.rad;
        double I2 = 2.5*col.obj2.mass*col.obj2.rad*col.obj2.rad;

        double friction = 0.3;


        
        Vector F = dv*(col.obj1.mass+col.obj2.mass)*friction;  //Kraftvektor

	double dt = 0.5;
        Vector M1 = cross_product(F,r_pos1);  //Moment
        Vector M2 = cross_product(F,r_pos2);
	


        Vector ra1 = M1/I1;  //rotationsaccelerationer
        Vector ra2 = M2/I2;

//	cout << "Vektorer: " << Length3(F) << Length3(ra2);

        ;  //kontakttid

        //Fixa till rotationshastigheten nu då

	obj1.rot_vel += dt*ra1;
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

	obj1.simulate(dt);
	obj2.simulate(dt);	
}

std::vector<Object*> objects;

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

	for(int i = 0; i < objects.size(); i++){
		glPushMatrix();

                glTranslatef(objects[i]->pos.x, objects[i]->pos.y, objects[i]->pos.z);
                glMultMatrixf(objects[i]->dir);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
 
		glutWireSphere(objects[i]->rad, 10, 10);        
		glPopMatrix();
	}
        
        glutSwapBuffers();
        double start = timer();
	if (current_time > 20.0)
                exit(0);

	float dt = timer() - current_time;        
	float temp_current=timer();

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
        
        //Set up camera
        glMatrixMode(GL_PROJECTION);
        gluPerspective( 40, 1.33, 0.01, 100);
        glMatrixMode(GL_MODELVIEW);
        gluLookAt(0.0, 0.0, 10,
                0.0, 0.0, 0.0,
                0.0, 1.0, 0.0);
}

const float velocity = 2.0;


int main(int argc, char **argv)	
{
  
  Object* object;

  /*	object = new Object(Vector(-3.0, 0.0, 0.0), Vector(1.0, 0.0, 0.0), 0.5);

	objects.push_back(object);

	object = new Object(Vector(0.0, -1.0, 0.0), Vector(0.0, 0.4, 0.0), 0.5);

	objects.push_back(object);
        
        object = new Object(Vector(3.0, 0.0, 0.0), Vector(-2.2, 0.0, 0.0), 0.5);
      
	objects.push_back(object); 

  */
	object = new Object(Vector(-3.0, 0.0, 0.0), Vector(velocity, 0.0, 0.0), 0.5, 2.0, 1.0);

	objects.push_back(object);


        object = new Object(Vector(4.0, 0.7, 0.0), 
                                Vector(-velocity, 0.0, 0.0), 
                                0.5, 2.0, 1.0, 
                                Vector(0.0, 0.0, 0.0));
      
	objects.push_back(object); 
        
//        object = new Object(Vector(3.0, 0.0, 0.0), Vector(-velocity, 0.0, 0.0), 0.5, 5, 10.0);
      
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

	current_time=0.0;
        glutMainLoop();

        return 0;
}


void update_world(double frame_start,double tot_time){

        double frame_stop = current_time+tot_time;
        double dt;
        
        ObjectPair* pair;
        
        if(pairs.size() > 0 ){
                while( pairs.top()->wake_up < frame_stop){

                        pair = pairs.top();
			// cout << "Popped pair: " << pair->id << ".\n";
                        
                        dt = pair->wake_up - pair->sim_time();
                        if( dt < min_dt)
                                dt=min_dt;
                        
                        //the object will be simulated this far when the loo has ended
                        
                        pairs.pop();
                        pair->simulate(dt);
                                                
                        if(pair->has_collided()){
                                Collision col = pair->get_collision();
                                pair->collide(col);
                                pair->simulate(min_dt);
                                        
                              while(pair->has_collided()){
                                      pair->simulate(min_dt);
                              }
                               
                        }

                

			//		cout << "Before -- Pair: " << pair->id << " Wake_up: " << pair->wake_up << ".\n";
		pair->calc_wake_up();
		//		cout << "After -- Pair: " << pair->id << " Wake_up: " << pair->wake_up << ".\n";

                pairs.push(pair);// bara om de inte har dött
                }                
        }
        
        
        for(int i = 0; i < objects.size(); i++){
                dt = frame_stop - objects[i]->sim_time;
                if(dt > 0)
                    objects[i]->simulate(dt);
        }


}



