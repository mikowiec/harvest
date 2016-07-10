
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:05 $
 * $Log: pong.cpp,v $
 * Revision 1.20  2003/01/06 12:42:05  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.19  2002/12/29 16:59:27  pstrand
 * hw/ moved to os/
 *
 * Revision 1.18  2001/01/14 05:33:16  peter
 * *** empty log message ***
 *
 * Revision 1.17  2001/01/13 17:23:57  peter
 * adapted to new object model
 *
 * Revision 1.16  2001/01/13 01:49:59  peter
 * *** empty log message ***
 *
 * Revision 1.15  2001/01/11 22:58:01  peter
 * *** empty log message ***
 *
 * Revision 1.14  2001/01/10 23:17:13  peter
 * *** empty log message ***
 *
 * Revision 1.13  2001/01/10 14:43:43  peter
 * eventtable (for current events)
 *
 * Revision 1.12  2001/01/10 13:32:59  peter
 * *** empty log message ***
 *
 * Revision 1.11  2001/01/10 03:24:45  peter
 * *** empty log message ***
 *
 * Revision 1.10  2001/01/10 02:24:18  peter
 * hw/gfx.cpp
 *
 * Revision 1.9  2001/01/09 22:16:51  peter
 * *** empty log message ***
 *
 * Revision 1.8  2001/01/09 18:36:05  peter
 * världsiteratorer... (funkar inte än)
 *
 * Revision 1.7  2001/01/09 00:22:01  peter
 * *** empty log message ***
 *
 * Revision 1.6  2001/01/08 18:43:07  peter
 * *** empty log message ***
 *
 * Revision 1.5  2001/01/06 14:23:25  peter
 * gcc...
 *
 * Revision 1.4  2001/01/06 05:45:49  peter
 * no message
 *
 * Revision 1.3  2001/01/01 15:43:00  peter
 * mer testkod
 *
 * Revision 1.2  2000/11/24 17:32:35  peter
 * win32 fixar...
 *
 * Revision 1.1  2000/11/23 23:37:56  peter
 * pong!
 *
 *
 */

#include "os/compat.h"

#include <string>
#include <iostream>
#include <vector>
#include <valarray>
#include <cstdio>
#include <typeinfo>

#include "main/types.h"
#include "os/gl.h"
#include "misc/sequence.h"
#include "misc/stlhelper.h"
#include "os/test/pong.h"
#include "os/snd.h"
#include "os/debug.h"
#include "world/world.h"

using namespace reaper;
using namespace reaper::misc;

namespace { reaper::os::debug::DebugOutput derr("pong"); }


class Mover
{
	Point& pos_ref;
	Vector& vel;
	BBox& bbox;
public:
	Mover(Point& p, Vector& v, BBox& bx)
	: pos_ref(p), vel(v), bbox(bx) { }
	void add_vel(const Vector& v)
	{
		vel += v;
	}
	void set_vel(const Vector& v)
	{
		vel = v;
	}
	const Vector& get_vel()
	{
		return vel;
	}
	int move()
	{
		pos_ref.x += vel[0];
		pos_ref.y += vel[1];
		pos_ref.z += vel[2];
//		bbox.abs_move(pos_ref.x, pos_ref.y);
		return 0;
	}
};

class Wall
{
	Geometry geom;
	float width, height;
public:
	Wall(const Point& p, float w, float h)
	 , geom(BBox(Point(p.x-w, p.y-h, 0), Point(p.x+w, p.y+h, 0)))
	 , width(w), height(h)
	{ }
	virtual ~Wall() { }


	virtual const Geometry& geometry() const { return geom; }

	virtual int render(RendererInterface& ri)
	{
		ri.draw_box(Point(pos.x - width, pos.y - height, 0),
			    Point(pos.x + width, pos.y + height, 0));
		return 0;
	}

	virtual int collide(Geometric& other) {
		return 0;
	}

};

class Player
 , public Controllable
{
	Geometry geom;
	Mover mover;
public:
	Player(const Point& p)
	 : geom(BBox(Point(-0.01f,-0.1f,0), Point(0.01f,0.1f,0)))
	 , mover(pos, speed, geom.bbox)
	{
		geom.bbox.rel_move(pos.x, pos.y);
	}
	void move(float v)
	{
		mover.add_vel(Vector(0, v, 0));
	}

	virtual const Geometry& geometry() const { return geom; }

	virtual int render(RendererInterface& ri)
	{
		ri.draw_box(Point(pos.x - 0.02, pos.y - 0.1, 0), 
			    Point(pos.x + 0.02, pos.y + 0.1, 0));
		return 0;
	}
	
	virtual int move()
	{
		mover.move();
		return 0;
	}

	virtual int collide(Geometric& other)
	{
		mover.add_vel(Vector(0, -2*mover.get_vel().y, 0));
		return 0;
	}

	virtual int do_stuff()
	{
		return 0;
	}

	virtual int control(ControlInput& ci)
	{
		return 0;
	}
};



class Ball
 : public DynamicObject
{
	Geometry geom;
	Mover mover;
public:
	Ball(const Point& p, const Quaternion& d)
	 : DynamicObject(0, Slick, Slick_BattleShip_1, p,
	    d, 0.0f, false, false, Assignment(), true)
	 , geom(BBox(Point(-0.018f,-0.018f,0), Point(0.018f,0.018f,0)))
	 , mover(pos, speed, geom.bbox)
	{
		mover.set_vel(Vector(d.i, d.j, d.k));
	}
	/*
	int collide(Base& other)
	{
		return 0;
	}
	*/

	virtual const Geometry& geometry() const { return geom; }

	virtual int render(RendererInterface& ri)
	{
		ri.draw_sphere(pos, 0.018);
		return 0;
	}
	
	virtual int move()
	{
		mover.move();
		return 0;
	}

/*	int collide(Player& other)
	{
		
		return 0;
	}
*/
	
	virtual int collide(Geometric& other)
	{
		Vector cur_spd = mover.get_vel();
		
		if (other.geometry().bbox.center().x != 1.0)
			mover.add_vel(Vector(-2*cur_spd.x, 0, 0));
		else
			mover.add_vel(Vector(0, -2*cur_spd.y, 0));
	
		if (dynamic_cast<Player*>(&other)) {
			mover.add_vel(Vector(0, rand() % 100 / 10000.0, 0));
		}
		return 0;
	}

	virtual int do_stuff()
	{
		return 0;
	}
};


class Phys {
	world::WorldRef world;
public:
	Phys(world::WorldRef w)
	: world(w)
	{ }

	template<class Iter, class Obj>
	void check_coll_int(Iter b, Iter e, Obj& d)
	{
		for (Iter c = b; c != e; ++c) {
			if (d.geometry().bbox.crosses(c->geometry().bbox)) {
				d.collide(*c);
			}
		}
	}

	int check_coll(DynamicObject& o)
	{
		check_coll_int(world->begin_st(), world->end_st(), o);
		check_coll_int(world->begin_dyn(), world->end_dyn(), o);
		return 0;
	}
};


class Renderer
 : public RendererInterface
{
	GLUquadricObj* ballobj;
	world::WorldRef world;
public:
	Renderer()
	 : ballobj(gluNewQuadric())
	 , world(world::World::get_ref())
	{ }


	void render()
	{
		world::World_st_iterator c, e = world->end_st();
		for (c = world->begin_st(); c != e; ++c) {
			c->render(*this);
		}

		world::World_dyn_iterator cd, ed = world->end_dyn();
		for (cd = world->begin_dyn(); cd != ed; ++cd) {
			cd->render(*this);
		}
	}
	
	void draw_box(const Point& p1, const Point& p2)
	{
		glRectf(p1.x, p1.y, p2.x, p2.y);
	}
	void draw_sphere(const Point& c, float radius)
	{
		glPushMatrix();
		glTranslatef(c.x, c.y, c.z);
		gluSphere(ballobj, radius, 15, 15);
		glPopMatrix();
	}
};

class Pong_data {
public:
	bool stop;

	reaper::LocalPlayer lp;
	world::WorldRef world;

	Player* player1;
	Player* player2;
	int npl;

	Renderer renderer;
	Phys phys;

	Pong_data()
	 : lp(0,Horny,Ammo_Box,Point(0,0,0),Quaternion(0,0,1,0),10,true,10,Assignment(),1)
	 , world(world::World::create("test_level", lp))
	 , player1(new Player(Point(0.3f, 1.0f, 0.0f)))
	 , player2(new Player(Point(1.7f, 1.0f, 0.0f)))
	 , npl(1)
	 , phys(world)
	{
		world->add_object(new Wall(Point( 0.2, 1.0, 1.0), 0.02, 0.8));
		world->add_object(new Wall(Point( 1.8, 1.0, 1.0), 0.02, 0.8));
		world->add_object(new Wall(Point( 1.0, 0.2, 1.0), 0.8, 0.02));
		world->add_object(new Wall(Point( 1.0, 1.8, 1.0), 0.8, 0.02));
		world->add_object(player1);
		world->add_object(player2);
		world->add_object(new Ball(Point(1.0, 1.0, 0.0), Quaternion(-0.001, 0, 0, 0)));
		world->add_object(new Ball(Point(1.0, 1.1, 0.0), Quaternion( 0.001, 0, 0, 0)));
		world->add_object(new Ball(Point(1.1, 1.0, 0.0), Quaternion( 0.001,-0.001, 0, 0)));
		world->add_object(new Ball(Point(1.1, 1.1, 0.0), Quaternion( 0.001, 0.001, 0, 0)));
	}
};

Pong::Pong()
: stop(false)
{ 
	data = new Pong_data();
	init_state();
}

Pong::~Pong()
{
	delete data;
}

void Pong::init_state()
{
	stop = false;
}


string Pong::get_state()
{
	return string("");
}


void Pong::redraw()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.2f, 0.2f, 0.2f);
	glRectf(0.2f, 0.2f, 1.8f, 1.8f);
	glColor3f(1.0f, 1.0f, 1.0f);

	data->renderer.render();
}

void Pong::resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 2.0, 0.0, 2.0, 0.0, 2.0);
	glClearColor(0, 0, 0, 1.0);
}


bool Pong::tick()
{
	std::for_each(data->world->begin_dyn(), data->world->end_dyn(),
		std::mem_fun_ref(&DynamicObjectBase::move));
	std::for_each(data->world->begin_dyn(), data->world->end_dyn(),
		apply_to(&data->phys, &Phys::check_coll));
	return true;
}

void Pong::cmd(int ply, Player_Cmd c)
{
	if (ply == 0)
		data->player1->move(0.0001 * (c == Left ? 1 : -1));
	if (ply == 1)
		data->player2->move(0.0001 * (c == Left ? 1 : -1));
	
}

bool Pong::finished()
{
	return stop;
}

