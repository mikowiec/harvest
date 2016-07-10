#include "os/compat.h"

#include <vector>
#include <deque>

#include "main/types.h"
#include "main/types_ops.h"

#include "gfx/gfx.h"
#include "gfx/managers.h"
#include "gfx/misc.h"
#include "gfx/light.h"
#include "gfx/abstracts.h"
#include "gfx/misc.h"
#include "gfx/instances.h"
#include "gfx/displaylist.h"
#include "gfx/gfx_types.h"
#include "gfx/static_geom.h"

#include "os/gl.h"
#include "os/debug.h"

#include "world/query_obj.h"

#include "misc/utility.h"
#include "misc/stlhelper.h"


using reaper::misc::frand2;

namespace reaper {
namespace {
	debug::DebugOutput dout("gfx::Effects",0);
}

namespace gfx {
namespace lowlevel {

using misc::BillBoard;

class Shot : public Effect
{
	class ShotInitializer : public Initializer
	{
		misc::StaticGeometry *geom;
	public:
		ShotInitializer() : geom(0) {}

		void init() {
			using reaper::misc::push_back;
			using std::vector;

			vector<Point> points;
			push_back(points)
				<< Point(0,0,-1)
				<< Point(0,0,4)
				<< Point(-.2,0,-.7)
				<< Point(0,-.2,-.7)
				<< Point(.2,0,-.7)
				<< Point(0,.2,-.7);

			vector<Color> colors;
			push_back(colors)
				<< Color(0,1,0)
				<< Color(0,1,0)
				<< Color(1,1,1)
				<< Color(1,1,1)
				<< Color(1,1,1)
				<< Color(1,1,1);

			vector<int> indices;
			push_back(indices)
				<< 0 << 2 << 3
				<< 0 << 3 << 4
				<< 0 << 4 << 5
				<< 0 << 5 << 2
				<< 1 << 2 << 3
				<< 1 << 3 << 4
				<< 1 << 4 << 5
				<< 1 << 5 << 2;

			geom = new misc::StaticGeometry(&points[0], &colors[0], 0, 0,
				    true, points.size(), &indices[0], indices.size());
		}

		void exit() {
			delete geom;
			geom = 0;
		}

		void render() {
			geom->render();
		}
	};
	
	mutable TextureRef tr;
	world::Sphere sphere;
	const Matrix &mtx;
	Color color;

	static Color s_white;
	static Color s_black;
public:
	static ShotInitializer initializer;

	Shot(const Matrix &m, const Color &c);
	void render();
	const world::Sphere& get_sphere();

	void setup() const;
	void restore() const {}
};

Shot::ShotInitializer Shot::initializer;
}

namespace initializers {
Initializer *shot = &lowlevel::Shot::initializer;
}

namespace lowlevel {

Color Shot::s_white(1,1,1);
Color Shot::s_black(0,0,0);

Shot::Shot(const Matrix &m, const Color &c) :
	Effect("shot",""), tr(TextureMgr::get_ref()), mtx(m), color(c)
{
	sphere.r = 1;
}

const world::Sphere& Shot::get_sphere()
{
	sphere.p = mtx.pos();
	return sphere;
}

void Shot::render()
{
	glPushMatrix();
	glMultMatrix(mtx);
	initializer.render();
	glPopMatrix();
}

void Shot::setup() const
{
	glBlendFunc(GL_ONE,GL_ZERO);
	tr->use("");
}

/////////////////////////////////////////////////////////////////////////////

class Sphere : public Effect
{
	class SphereInitializer : public Initializer
	{
	public:
		misc::DisplayList list;

		SphereInitializer() : list(false) {}

		void init() {
			list.init();

			list.begin();
			misc::sphere(1,20,20);
			glPopMatrix();
			list.end();	
		}

		void exit() {
			list.kill();
		}
	};

	mutable TextureRef tr;
	world::Sphere sphere;
	const Matrix &mtx;
	const Color &color;
public:
	static SphereInitializer initializer;

	Sphere(const Matrix &m, float r, const Color &c);
	void render();
	void set_pos(const Point &p);
	const world::Sphere& get_sphere();

	void setup() const;
	void restore() const {}
};

Sphere::SphereInitializer Sphere::initializer;
}

namespace initializers {	
Initializer *sphere = &lowlevel::Sphere::initializer;
}

namespace lowlevel {


Sphere::Sphere(const Matrix &m, float r, const Color &c) :
	Effect("sphere",""), tr(TextureMgr::get_ref()), mtx(m), color(c)
{
	sphere.r = r;
}

void Sphere::render()
{	
	glColor4(color);
	glPushMatrix();
	glTranslate(mtx.pos());
	glScalef(sphere.r, sphere.r, sphere.r);
	initializer.list.call();
	// glPopMatrix() in list :)
}
 
void Sphere::setup() const
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	tr->use("");
}

const world::Sphere& Sphere::get_sphere()
{
	sphere.p = mtx.pos();
	return sphere;
}

/////////////////////////////////////////////////////////////////////////////

// lit sphere with a lifetime of one second
class ShieldSphere : public SimEffect
{
	light::Light light;
	const Matrix &mtx;
	const float radius;
	float lifetime;
	mutable world::Sphere sphere;

public:
	ShieldSphere(const Matrix &m, float r, const Color &c, const Vector &light);
	const world::Sphere& get_sphere();
	const Point& get_pos() const;
	float get_radius() const; 

	void render();
	void setup() const;
	void restore() const;

	bool simulate_death(float dt);
	void simulate(float dt);
};

ShieldSphere::ShieldSphere(const Matrix &m, float r, const Color &c, const Vector &light)
: SimEffect("shieldsphere",""), light(light, c), mtx(m), radius(r), lifetime(0)
{
	sphere.r = r;
}

const world::Sphere& ShieldSphere::get_sphere()
{
	sphere.p = mtx.pos();
	return sphere;
}

const Point& ShieldSphere::get_pos() const 
{ 
	return mtx.pos(); 
}
float ShieldSphere::get_radius() const 
{ 
	return sphere.r; 
}

void ShieldSphere::setup() const {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glBlendFunc(GL_ONE, GL_ONE);
}

void ShieldSphere::render() {
	light.activate(0);
	glPushMatrix();
	glMultMatrix(mtx);
	glColor3f(1,1,1);
	Sphere::initializer.list.call();
	glPopMatrix();
}

void ShieldSphere::restore() const {
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
}

bool ShieldSphere::simulate_death(float dt) {
	return false;
}

void ShieldSphere::simulate(float dt) {
	lifetime += dt;

	light.diffuse.a = 1 - lifetime;
	light.specular.a = 1 - lifetime;
}
/////////////////////////////////////////////////////////////////////////////

class DistanceEmitter
{
private:
	Point prev;
	float move_dist;
	const float emit_dist;
public:
	DistanceEmitter(const Point& p, float ed) : prev(p), move_dist(0), emit_dist(ed) {}

	// returns true if an emit is due, then p is updated with correct pos
	bool emit(const Point &new_p, Point &p) {
		float len = move_dist + length(new_p - prev);
		if(len > emit_dist) {
			float x = emit_dist / len;
			prev = new_p*x + prev*(1-x);
			p = prev;
			move_dist = 0;
			return true;
		} else {
			prev = new_p;
			move_dist += len;
			return false;
		}
	}		
};

class EngineTrail : public SimEffect
{
	class TrailPiece {
	public:
		Point p1;
		Point p2;		
		TrailPiece(const Point &P1, const Point &P2) : p1(P1), p2(P2) {}
	};

	std::deque<TrailPiece> trail;
	DistanceEmitter emitter;

	const Matrix &mtx;
	const float size;
	const Point offset;
	Color color;
	world::Sphere sphere;

	mutable TextureRef tr;

	static const int trail_length;
public:
	EngineTrail(const Matrix &mtx, float size, const Point &offset);

	bool simulate_death(float dt);
	void simulate(float dt);
	void render();

	const world::Sphere& get_sphere() { return sphere; }
	const Point& get_pos() const { return sphere.p; }
	float get_radius() const { return sphere.r; }

	void setup() const;
	void restore() const;
};

const int EngineTrail::trail_length = 50;

EngineTrail::EngineTrail(const Matrix &m, float s, const Point &o) : 
	SimEffect("engine_trail","engine_wake"),
	emitter(m.pos(), 1),
	mtx(m), size(s), offset(o),
	color(1,1,1,.7),
	sphere(m.pos(), 1),
	tr(TextureMgr::get_ref())
{
}

void EngineTrail::simulate(float dt)
{
	const Point  p = mtx * offset;
	const Vector w = mtx.col(0) * size;

	Point new_p;
	while(emitter.emit(p, new_p)) { 
		trail.push_front(TrailPiece(new_p-w, new_p+w));
	}
	
	if(!trail.empty()) {
		trail.front().p1 = p - w;
		trail.front().p2 = p + w;

		while(trail.size() > trail_length) {
			trail.pop_back();
		}

		sphere.p = (trail.front().p1 + trail.back().p1) / 2;
		sphere.r = length(trail.front().p1 - sphere.p);
	}
}

bool EngineTrail::simulate_death(float dt)
{
	return false;
}

void EngineTrail::render()
{
	using namespace std;

	static vector<Point> points;
	static back_insert_iterator<vector<Point> > pi(back_inserter(points));

	points.reserve(trail_length*2);
	for(std::deque<TrailPiece>::iterator i = trail.begin(); i != trail.end(); ++i) {
		pi = i->p1;
		pi = i->p2;
	}

	glVertexPointer(3, GL_FLOAT, sizeof(Point), &points[0]);
	glDrawArrays(GL_TRIANGLE_STRIP,0,points.size());
	points.clear();
}

void EngineTrail::setup() const
{
	static std::vector<Color>    colors(trail_length*2);
	static std::vector<TexCoord> texcoords(trail_length*2);
	static bool init = false;
	if(!init) {
		Color c(color);
		const float dec = 1.2 / trail_length;
		for(int i=0; i<trail_length*2; i+=2) {
			colors[i]      = c;
			colors[i+1]    = c;
			texcoords[i]   = TexCoord(.5,0);
			texcoords[i+1] = TexCoord(.5,1);
			c.a -= dec;
		}
		init = true;
	}
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glColorPointer(4,    GL_FLOAT, sizeof(Color),    &colors[0]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(TexCoord), &texcoords[0]);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	tr->use("engine_wake");
}

void EngineTrail::restore() const
{
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

/////////////////////////////////////////////////////////////////////////////

class SmokeTrail : public SimEffect
{
	class TrailPiece {
	public:
		Point pos;
		Vector vel;
		float size;
		float max_size;
		Color c;
		float life_time;
		
		TrailPiece(const Point &p, float s, const Vector &v) 
		: pos(p), vel(v.x + frand2()*2, v.y + frand2()*2, v.z + frand2()*2), 
		  max_size(s), c(1,1,0), life_time(0)
		{ }

		bool simulate(float dt) {
			pos += vel*dt;
			life_time += dt;
			size = std::min<float>(max_size, life_time);

			// fade from yellow to white to transparent
			if(life_time > .2 && life_time < .7) {
				c = Color(1,1,(life_time*2)-.4);
			}
			
			const float max_life = 1.5;
			if(life_time < max_life) {
				c.a = (max_life-life_time)/max_life;				
				return true;
			} else {
				return false;
			}
		}

		// VC ARGH! (it's supposed to be void...)
		bool render() {
			BillBoard::add(size, pos, c);
			return true;
		}
	};

	std::deque<TrailPiece> trail;
	DistanceEmitter emitter;

	const Matrix &mtx;
	const float size;
	const Point offset;
	world::Sphere sphere;

	mutable TextureRef tr;

public:
	SmokeTrail(const Matrix &mtx, float size, const Point &offset);

	bool simulate_death(float dt);
	void simulate(float dt);
	void render();

	const world::Sphere& get_sphere() { return sphere; }
	const Point& get_pos() const { return sphere.p; }
	float get_radius() const { return sphere.r; }

	void setup() const;
	void restore() const;
};

SmokeTrail::SmokeTrail(const Matrix &m, float s, const Point &o) : 
	SimEffect("smoke_trail","smoke"), emitter(m.pos(), s*4),
	mtx(m), size(s), offset(o),
	sphere(m.pos(), 1),
	tr(TextureMgr::get_ref())
{
}

void SmokeTrail::simulate(float dt)
{
	Point new_p;
	while(emitter.emit(mtx * offset, new_p)) {
		trail.push_front(TrailPiece(new_p, size, mtx.col(2) * 60));
	}
	
	simulate_death(dt);
}

bool SmokeTrail::simulate_death(float dt)
{
	// better not touch that matrix here. :)
	for(std::deque<TrailPiece>::iterator i = trail.begin(); i != trail.end();) {
		if(!i->simulate(dt)) {
			trail.erase(i,trail.end());
			break;
		} else {
			++i;
		}
	}
	//color.a = .7 * trail.size()/trail_length;

	switch(trail.size()) {
	case 0: return false;

	case 1: sphere.p = trail.front().pos;
		sphere.r = trail.front().size;
		break;

	default:sphere.p = (trail.front().pos + trail.back().pos) / 2;
		sphere.r = length(trail.front().pos - trail.back().pos) / 2;
		break;
	}
	
	return true;
}

void SmokeTrail::render()
{
	std::for_each(trail.begin(), trail.end(), std::mem_fun_ref(&TrailPiece::render));
}

void SmokeTrail::setup() const
{
}

void SmokeTrail::restore() const
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	tr->use("smoke");
	BillBoard::render_colors();
}

/////////////////////////////////////////////////////////////////////////////

class LightBase : public SimEffect
{
protected:
	LightRef lr;
	mutable light::Light* light;

	float fade_speed;    ///< amount to increase constant attenuation every second

	LightBase(light::Light*, float fade_speed);
	world::Sphere sphere;
public:
	~LightBase() { }
	bool simulate_death(float dt); // fades light until intensity = 0, then expires
	void set_color(const Color &c);

	const world::Sphere& get_sphere() { return sphere; }	
	const Point& get_pos() const { return sphere.p; }
	float get_radius() const { return sphere.r; }

	void render() {}
	void setup() const {};
	void restore() const {}
};

LightBase::LightBase(light::Light *l, float fs) :
	SimEffect("light",""), lr(LightMgr::get_ref()), light(l), fade_speed(fs)
{
}

void LightBase::set_color(const Color &c)
{
	light->diffuse = c;
	light->specular = c;
}

bool LightBase::simulate_death(float dt)
{
	min_clamp(light->diffuse -= fade_speed * dt);
      	min_clamp(light->specular -= fade_speed * dt);
	return light->diffuse.r + light->diffuse.g + light->diffuse.b > .1;
}

/////////////////////////////////////////////////////////////////////////////

class StaticLight : public LightBase
{
public:
	StaticLight(const Point &pos, const Color &color, float radius, float fade_speed);
	~StaticLight();
};

StaticLight::StaticLight(const Point &p, const Color &c, float r, float f) : 
	LightBase(new light::Light(p, c, r), f)
{
	sphere.p = p;
	sphere.r = r;
	lr->add_static(light);
}

StaticLight::~StaticLight()
{
	lr->remove_static(light);
	delete light;   
}

/////////////////////////////////////////////////////////////////////////////

class DynamicLight : public LightBase
{
protected:
	const Matrix &mtx;
	Point  offset;    
	Vector vel;
	bool   grav;
public:
	DynamicLight(const Matrix &m, const Point &o, const Color &color, 
	             float radius, float fade_speed, bool gravitation = true);
	~DynamicLight();

	bool simulate_death(float dt); // fades and moves light 
	void simulate(float dt);       // updates light position w/r to matrix and offset

	void set_offset(const Point &p);
	void set_velocity(const Vector &v) { vel = v; }
};

DynamicLight::DynamicLight(const Matrix &m, const Point &o, const Color &c, 
	                   float r, float fs, bool g) :
	LightBase(new light::Light(Point(m.pos() + o), c, r), fs), 
	mtx(m), offset(o), vel(0,0,0), grav(g) 
{
	sphere.p = m.pos() + o;
	sphere.r = r;
	lr->add_dynamic(light);
}


DynamicLight::~DynamicLight()
{
	lr->remove_dynamic(light);
	delete light;
}

void DynamicLight::set_offset(const Point &p)
{
	offset = p; 
	Point light_pos = mtx.pos() + offset;
	light->pos = Vector4(light_pos);
	sphere.p = light_pos;
}

void DynamicLight::simulate(float dt)
{
	Point light_pos = mtx.pos() + offset;
	light->pos = Vector4(light_pos);
	sphere.p = light_pos;
}

bool DynamicLight::simulate_death(float dt)
{
	// better not touch that matrix here. :)
	if(grav) {
		vel.y -= 5*dt;
	}

	Point light_pos(light->pos.as_point());
	light_pos += vel*dt;
	light->pos = Vector4(light_pos);
	sphere.p = light_pos;

	return LightBase::simulate_death(dt);
}

} // end namespace lowlevel

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

using namespace lowlevel;

EffectPtr shot(const Matrix &m, const Color &c) 
{
	return EffectPtr(new Shot(m,c)); 
}
EffectPtr sphere(const Matrix &m, float r, const Color &c) 
{
	return EffectPtr(new Sphere(m,r,c)); 
}

SimEffectPtr shieldsphere(const Matrix &m, float r, const Color &c, const Vector &light) 
{
	return SimEffectPtr(new ShieldSphere(m,r,c,light));
}

SimEffectPtr enginetrail(const Matrix &mtx, float size, const Point &offset) 
{
	return SimEffectPtr(new EngineTrail(mtx,size,offset));
}
SimEffectPtr smoketrail(const Matrix &mtx, float size, const Point &offset) 
{
	return SimEffectPtr(new SmokeTrail(mtx,size,offset));
}

SimEffectPtr static_light(const Point &pos, const Color &color, float radius,float fade_speed) 
{
	return SimEffectPtr(new StaticLight(pos,color,radius,fade_speed));
}
SimEffectPtr dynamic_light(const Matrix &m, const Point &o, const Color &color, 
			float radius, float fade_speed, bool gravitation)
{
	return SimEffectPtr(new DynamicLight(m,o,color,radius,fade_speed,gravitation));
}

}
}

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:32 $
 * $Log: effects.cpp,v $
 * Revision 1.75  2002/12/29 17:04:32  pstrand
 * hw/ moved to os/
 *
 * Revision 1.74  2002/09/23 12:08:43  fizzgig
 * Moved all anonymous namespaces into the reaper-namespace
 *
 * Revision 1.73  2002/09/21 17:41:50  pstrand
 * uninit.vars.
 *
 * Revision 1.72  2002/04/30 10:17:22  pstrand
 * init. eff.sphere and no vector<T>::at (old gcc:s doesn't have this.. will workaround it someday)
 *
 * Revision 1.71  2002/04/29 20:33:26  fizzgig
 * Added quadtree for particle-systems, plus shot->silly fix
 *
 * Revision 1.70  2002/04/14 09:22:03  pstrand
 * quadtree redesign and Sphere::radius -> Sphere::r
 *
 * Revision 1.69  2002/04/07 00:05:34  pstrand
 * fix. for old gcc:s
 *
 * Revision 1.68  2002/03/11 10:50:48  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.67  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.66  2002/02/13 21:57:17  macke
 * misc effects modifications
 *
 * Revision 1.65  2002/02/08 11:27:49  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.64  2002/01/31 05:35:03  peter
 * *** empty log message ***
 *
 * Revision 1.63  2002/01/30 23:47:49  macke
 * better missile smoke..
 *
 * Revision 1.62  2002/01/23 04:42:45  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.61  2002/01/22 23:44:00  peter
 * reversed last two revs
 *
 * Revision 1.59  2002/01/10 23:09:06  macke
 * massa bök
 *
 * Revision 1.58  2002/01/08 19:40:21  macke
 * Texturdetaljnivå, samt lite småfix..
 *
 * Revision 1.57  2001/12/18 14:34:57  macke
 * minifix
 *
 * Revision 1.56  2001/12/14 16:31:24  macke
 * meck å pul
 *
 * Revision 1.55  2001/12/13 17:03:28  peter
 * småfixar...
 *
 * Revision 1.54  2001/12/11 23:16:45  macke
 * engine tails now disappear properly opon death...
 *
 * Revision 1.53  2001/12/05 14:04:11  peter
 * matrix now in (semi)row-major
 *
 * Revision 1.52  2001/12/04 23:01:22  picon
 * menu system now working ... barely. :)
 *
 * Revision 1.51  2001/12/02 19:47:43  peter
 * get_row -> get_col
 *
 * Revision 1.50  2001/11/26 02:20:06  peter
 * ljudfixar (smartptr), andra mindre fixar..
 *
 * Revision 1.49  2001/11/25 19:56:20  peter
 * kommer inte åt privat shot::initialize...
 *
 * Revision 1.48  2001/11/21 00:27:51  picon
 * missile smoke / ship tail / misc
 *
 * Revision 1.47  2001/11/20 21:44:09  picon
 * firing shots after restart now works..
 *
 * Revision 1.46  2001/11/10 11:58:29  peter
 * diverse minnesfixar och strul...
 *
 * Revision 1.45  2001/10/08 15:59:17  macke
 * Separerad simulering och rendering för grafikmotorn
 *
 * Revision 1.44  2001/09/24 02:33:23  macke
 * Meckat lite med fulbuggen i grafikmotorn.. verkar funka att deallokera nu.. ?
 *
 * Revision 1.43  2001/09/10 00:24:53  macke
 * Grafikfix... (skum cpu-läcka i ljussättningen kvarstår)
 *
 * Revision 1.42  2001/08/18 16:46:20  macke
 * Grafikfixar
 *
 * Revision 1.41  2001/08/06 12:16:11  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.40.2.1  2001/07/31 17:34:03  peter
 * testgren...
 *
 * Revision 1.40  2001/07/30 23:43:15  macke
 * Häpp, då var det kört.
 *
 * Revision 1.39  2001/07/24 23:52:46  macke
 * Jo, explicit ska det vara (fel på annat ställe)..  rättade till lite array-fel också..
 *
 * Revision 1.38  2001/07/23 23:48:10  macke
 * Slimmad grafikhantering samt lite namnbyten
 *
 * Revision 1.37  2001/07/09 13:33:06  peter
 * gcc-3.0 fixar
 *
 * Revision 1.36  2001/07/06 01:47:09  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.35  2001/06/09 01:58:46  macke
 * Grafikmotor reorg
 *
 * Revision 1.34  2001/06/07 05:14:16  macke
 * Reaper v0.9
 *
 * Revision 1.33  2001/05/15 04:24:32  macke
 * *** empty log message ***
 *
 * Revision 1.32  2001/05/15 03:44:54  macke
 * *** empty log message ***
 *
 * Revision 1.31  2001/05/14 20:00:51  macke
 * bugfix och rök på missiler..
 *
 * Revision 1.30  2001/05/10 11:40:10  macke
 * häpp
 *
 */

