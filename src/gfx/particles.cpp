/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:34 $
 * $Log: particles.cpp,v $
 * Revision 1.36  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.35  2002/09/21 17:41:50  pstrand
 * uninit.vars.
 *
 * Revision 1.34  2002/09/13 07:45:55  pstrand
 * gcc-varningar
 *
 * Revision 1.33  2002/05/29 00:35:12  fizzgig
 * cleanup, small fixes, improvement, etc
 *
 * Revision 1.32  2002/04/29 20:33:26  fizzgig
 * Added quadtree for particle-systems, plus shot->silly fix
 *
 * Revision 1.31  2002/04/14 09:22:08  pstrand
 * quadtree redesign and Sphere::radius -> Sphere::r
 *
 * Revision 1.30  2002/04/07 00:05:35  pstrand
 * fix. for old gcc:s
 *
 * Revision 1.29  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.28  2002/02/13 21:57:18  macke
 * misc effects modifications
 *
 * Revision 1.27  2002/01/26 23:27:12  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.26  2001/12/04 23:01:23  picon
 * menu system now working ... barely. :)
 *
 * Revision 1.25  2001/08/06 12:16:15  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.24.2.1  2001/08/05 14:01:25  peter
 * objektmeck...
 *
 * Revision 1.24  2001/07/30 23:43:18  macke
 * Häpp, då var det kört.
 *
 * Revision 1.23  2001/07/24 23:52:47  macke
 * Jo, explicit ska det vara (fel på annat ställe)..  rättade till lite array-fel också..
 *
 * Revision 1.22  2001/07/23 23:48:10  macke
 * Slimmad grafikhantering samt lite namnbyten
 *
 * Revision 1.21  2001/07/21 23:09:18  peter
 * intel fix..
 *
 * Revision 1.20  2001/07/09 13:33:06  peter
 * gcc-3.0 fixar
 *
 * Revision 1.19  2001/07/06 01:47:13  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.18  2001/06/09 01:58:51  macke
 * Grafikmotor reorg
 *
 * Revision 1.17  2001/06/07 05:14:19  macke
 * Reaper v0.9
 *
 * Revision 1.16  2001/05/15 03:44:54  macke
 * *** empty log message ***
 *
 * Revision 1.15  2001/05/15 01:42:26  peter
 * minskade ner på debugutskrifterna
 *
 * Revision 1.14  2001/05/14 20:00:53  macke
 * bugfix och rök på missiler..
 *
 * Revision 1.13  2001/05/10 11:40:15  macke
 * häpp
 *
 */

#include "os/compat.h"

#include <vector>
#include <iterator>
#include <deque>

#include "main/types.h"
#include "main/types_ops.h"
#include "main/types_io.h"

#include "gfx/gfx.h"
#include "gfx/managers.h"
#include "gfx/abstracts.h"
#include "gfx/misc.h"
#include "gfx/exceptions.h"
#include "gfx/instances.h"

#include "os/gl.h"
#include "os/debug.h"
#include "misc/stlhelper.h"
#include "misc/utility.h"
#include "world/query_obj.h"

#include <stdlib.h>

namespace reaper {
namespace gfx {
namespace lowlevel {
using namespace std;
using namespace reaper::gfx::misc;
using reaper::misc::frand;
using reaper::misc::frand2;
using std::min;
using std::max;

debug::DebugOutput dout("gfx::particles",0);
vector<Point> points;	
vector<Color> colors;	
back_insert_iterator<vector<Point> > pi(back_inserter(points));
back_insert_iterator<vector<Color> > ci(back_inserter(colors));

class ParticleBase : public SimEffect
{
protected:
	world::Sphere sphere;
public:
	ParticleBase(const RenderID &id, const RenderID &tex) : SimEffect(id,tex) {}
	const world::Sphere& get_sphere() { return sphere; }
	const Point& get_pos() const { return sphere.p; }
	float get_radius() const { return sphere.r; }
};

/// Small collision sparks
class Sparks : public ParticleBase
{
	class Spark
	{
	public:
		Point pos;
		Vector vel;
		Spark(const Point &p, const Vector &v) : pos(p), vel(v) {}
	};

	mutable TextureRef tr;
	float alpha;
	std::vector<Spark> sparks;
public:
	Sparks(const Point &p, float i);
	bool simulate_death(float dt);
	void render();
	void setup() const;
	void restore() const;
};

//////////////////////////////////////////////////////

/// Instant smoke (not continously generated as from SmokeBase)
class SmokePuff : public ParticleBase
{
	class Puff
	{
	public:
		Point pos;
		Vector vel;
		float size;
		Puff(const Point &p, const Vector &v, float s) : pos(p), vel(v), size(s) {}
	};

	std::vector<Puff> smoke;
	mutable TextureRef tr;
	Color color;
public:
	SmokePuff(const Point &p, float i);
	bool simulate_death(float dt);
	void render();
	void setup() const;
	void restore() const;
};

//////////////////////////////////////////////////////

class ExplosionBase : public ParticleBase
{
	class Fragment
	{
	public:
		Point pos;
		Vector vel;
		Fragment(const Point &p, const Vector &v);
	};

	std::vector<Fragment> frags;
	float intensity;
	float color;		///< color of fragments
	float sph_color;	///< color of sphere
	float rot_angle;
	Vector v;
	const Vector (*a)();
	mutable TextureRef tr;
protected:
	ExplosionBase(const Point &p, float i, const Vector &vel, const Vector (*v)(), const Vector (*a)());
public:
	~ExplosionBase();

	bool simulate_death(float dt);	
	void render();
	void setup() const;
	void restore() const;
};

//////////////////////////////////////////////////////

class GroundExp : public ExplosionBase
{
	static const Vector acc();
	static const Vector vel();
public:
	GroundExp(const Point &p, float i);
};

//////////////////////////////////////////////////////

class AirExp : public ExplosionBase
{
	static const Vector acc();
	static const Vector vel();
public:
	AirExp(const Point &p, const Vector &v, float i);
};

//////////////////////////////////////////////////////

class SmokeBase	: public ParticleBase
{
	class Cloud
	{
	public:
		Point pos;
		Vector vel;
		float size;
		Cloud(const Point &p, float size);

		static const float respawn_time;	// seconds 
		static const float init_size;		// meters
	};

	Point pos;
	float intensity;
	float time;		// time since last cloud generated
	Color color;
	std::deque<Cloud> clouds;
	mutable TextureRef tr;	
protected:
	SmokeBase(const Point &p, float i, Color c);
public:
	~SmokeBase();

	bool simulate_death(float dt);	
	void render();
	void setup() const;
	void restore() const;
};

//////////////////////////////////////////////////////

class GreySmoke : public SmokeBase
{
public:
	GreySmoke(const Point &p, float i);
};

class WhiteSmoke : public SmokeBase
{
public:
	WhiteSmoke(const Point &p, float i);
};

//////////////////////////////////////////////////////

class BloodNGuts : public ParticleBase
{
	class Fragment
	{
	public:
		Point pos;
		Vector vel;
		Fragment(const Point &p, const Vector &v) : pos(p), vel(v) {}
	};

	std::vector<Fragment> splats;
	float intensity;
	mutable TextureRef tr;
public:
	BloodNGuts(const Point &pos, float i);
	
	bool simulate_death(float dt);	
	void render();
	void setup() const;
	void restore() const;
};

//////////////////////////////////////////////////////

Sparks::Sparks(const Point &p, float i) : 
	ParticleBase("sparks",""), tr(TextureMgr::get_ref()), alpha(1)
{
	sphere.p = p;
	sphere.r = 0;
	sparks.reserve(static_cast<int>(i*40));

	for(float f=0;f<i; f += 1/30.0f) {
		sparks.push_back(Spark(p, norm(Vector(frand2(), frand2(), frand2())) *2*i* (frand()+.5)));
	}
}

void Sparks::render()
{
	using namespace std;

	points.reserve(sparks.size()*2);
	colors.reserve(sparks.size()*2);

	for(std::vector<Spark>::const_iterator i = sparks.begin(); i != sparks.end();++i) {
		pi = i->pos;
		pi = i->pos - (norm(i->vel)*.5 + i->vel*.1);
		ci = Color(1, 1, 0, alpha);
		ci = Color(1, 0, 0, 0);
	}
}

void Sparks::setup() const
{
}

void Sparks::restore() const
{
	tr->use("");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, sizeof(Color), &colors[0]);
	glVertexPointer(3, GL_FLOAT, sizeof(Point), &points[0]);
	glDrawArrays(GL_LINES, 0, points.size());
	points.clear();
	colors.clear();
	glDisableClientState(GL_COLOR_ARRAY);
}

bool Sparks::simulate_death(float dt)
{
	alpha -= .5*dt;
	float sq_radius = 0;
	for(std::vector<Spark>::iterator i = sparks.begin(); i != sparks.end(); ++i) {
		i->vel.y -= 3*dt;
		i->pos += i->vel*dt;		
		sq_radius = max(sq_radius, length_sq(sphere.p - i->pos));
	}
	sphere.r = sqrt(sq_radius) + 1; // fragments are of size 1

	// keep alive as long as fragment is not black
	return alpha > 0;	
}

//////////////////////////////////////////////////////

SmokePuff::SmokePuff(const Point &p, float i) : 
	ParticleBase("smoke_puff","smoke"), tr(TextureMgr::get_ref()), color(0,0,0,1)
{
	sphere.p = p;
	sphere.r = 0;
	smoke.reserve(static_cast<int>(i*10));

	for(float f=0;f<i; f += 1/10.0f) {
		smoke.push_back(Puff(p ,norm(Vector(frand2(), frand2()+.2, frand2()))*(3+i)*(frand()+.2), .3));
	}
}

void SmokePuff::render()
{
	for(std::vector<Puff>::const_iterator i = smoke.begin(); i != smoke.end();++i) {
		BillBoard::add(i->size, i->pos, color);
	}
}

void SmokePuff::setup() const
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	tr->use("smoke");
}

void SmokePuff::restore() const
{
	BillBoard::render_colors();
}


bool SmokePuff::simulate_death(float dt)
{
	color.a -= .4*dt;
	float sq_radius = 0;
	for(std::vector<Puff>::iterator i = smoke.begin(); i != smoke.end(); ++i) {
		i->vel *= max(.9f, 1-dt);
		i->vel.y += 5*dt;
		i->pos += i->vel*dt;
		i->size += dt;
		sq_radius = max(sq_radius, length_sq(sphere.p - i->pos));
	}
	sphere.r = sqrt(sq_radius) + 1; // fragments are of size 1

	// keep alive as long as fragment is not invisible
	return color.a > 0.01;	
}


//////////////////////////////////////////////////////

ExplosionBase::Fragment::Fragment(const Point &p, const Vector &v) :
	pos(p), vel(v)
{
}

//////////////////////////////////////////////////////

ExplosionBase::ExplosionBase(const Point &p, float i, const Vector &vel, 
                             const Vector (*vel_gen)(), const Vector (*acc_gen)()) :
	ParticleBase("explosion","fire_particle"), intensity(i), color(1), sph_color(1), v(vel), a(acc_gen),
	tr(TextureMgr::get_ref())
{
	sphere.p = p;
	sphere.r = 0;
	frags.reserve(static_cast<int>(i*10));

	for(float f=0;f<i; f += 1/10.0f) {
		frags.push_back(Fragment(p,v+vel_gen()));
	}
}


ExplosionBase::~ExplosionBase()
{
}

void ExplosionBase::render()
{
	// Fade from white->yellow->red
	Color c;

	if(color > .5) {
		c = Color(1,color,color,1);
	} else if ( color > .25) {
		c = Color(color*2, (color-.25)*2, (color-.25)*2, 1);
	} else {
		c = Color(color*2,0,0,color*4);
	}

	float rot = rot_angle;
	for(std::vector<Fragment>::const_iterator i = frags.begin(); i != frags.end(); ++i) {
		BillBoard::add(2, i->pos, c);
	}
	
	if(sph_color>0) {
		glColor4f(1,sph_color*2,sph_color,sph_color*2);
		glPushMatrix();
		glTranslate(sphere.p);
		misc::sphere((1.2-sph_color)*17,20,20);
		glPopMatrix();
	}
}

void ExplosionBase::setup() const
{
	// for spheres
	tr->use(""); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void ExplosionBase::restore() const
{
	tr->use("fire_particle");
	BillBoard::render_colors();
}

bool ExplosionBase::simulate_death(float dt)
{
	color -= .33*dt;
	rot_angle += 720*dt;
	float sq_radius = 0;
	for(std::vector<Fragment>::iterator i = frags.begin(); i != frags.end(); ++i) {
		i->vel += a()*dt;
		i->pos += i->vel*dt;
		sq_radius = max(sq_radius, length_sq(sphere.p - i->pos));
	}
	sphere.p += v*dt;
	v *= 1-.1*dt;
	sphere.r = sqrt(sq_radius) + 1; // fragments are of size 1
	sph_color -= dt;

	// keep alive as long as fragment is not black
	return color > 0;	
}


//////////////////////////////////////////////////////

GroundExp::GroundExp(const Point &p, float i) : 
	ExplosionBase(p,i,Vector(0,0,0),vel,acc)
{
}

const Vector GroundExp::vel()
{
	return Vector(8*frand2(),15-10*frand(),8*frand2());
}

const Vector GroundExp::acc()
{
	return Vector(0,-5,0);
}


//////////////////////////////////////////////////////

AirExp::AirExp(const Point &p, const Vector &v, float i) : 
	ExplosionBase(p,i,v,vel,acc)
{
}

const Vector AirExp::vel()
{
	return Vector(frand2(),frand2(),frand2())*15;
}

const Vector AirExp::acc()
{
	return Vector(0,-5,0);
}


//////////////////////////////////////////////////////

GreySmoke::GreySmoke(const Point &p, float i) : SmokeBase(p,i, Color(0,0,0)) {}
WhiteSmoke::WhiteSmoke(const Point &p, float i) : SmokeBase(p,i, Color(1,1,1)) {}

//////////////////////////////////////////////////////

SmokeBase::SmokeBase(const Point &p, float i, Color c) : 
	ParticleBase("smoke_generator","smoke"), pos(p), intensity(i), time(0), color(c),
	tr(TextureMgr::get_ref())
{
	sphere.p = p;
}

SmokeBase::~SmokeBase()
{
}

void SmokeBase::render()
{
	Color c(color);
	for(std::deque<Cloud>::iterator i = clouds.begin(); i != clouds.end();++i) {
		c.a = (1 - i->size/5) * 2;
		BillBoard::add(i->size, i->pos, c);
	}
}

void SmokeBase::setup() const
{
}

void SmokeBase::restore() const
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	tr->use("smoke");
	BillBoard::render_colors();
}

bool SmokeBase::simulate_death(float dt)
{
	time -= dt;
	if(time<0 && intensity > 0) {
		clouds.push_back(Cloud(pos,intensity));
		time += Cloud::respawn_time;
	}	

	float sq_radius = 0;
	for(std::deque<Cloud>::iterator i = clouds.begin(); i != clouds.end();) {
		i->size += 2*dt;
		if(i->size<5) {
			i->vel += Vector(2*frand2(),0,2*frand2())*dt;
			i->pos += i->vel*dt;
			sq_radius = max(sq_radius, length_sq(sphere.p - i->pos));
			++i;
		} else {
			i = clouds.erase(i, clouds.end());
		}
	}

	sphere.r = sqrt(sq_radius);
	intensity -= .2*dt;

	return !clouds.empty();
}


const float SmokeBase::Cloud::respawn_time = 0.125f;	// seconds
const float SmokeBase::Cloud::init_size = 1.0f;	// meters

SmokeBase::Cloud::Cloud(const Point &p, float s)
 : pos(p),
   vel(frand2()*6, 10, frand2()*6),
   size(init_size+.7*frand2()*s)
{
}
			
BloodNGuts::BloodNGuts(const Point &p, float i) : 
	ParticleBase("blood_n_guts",""), intensity(i), tr(TextureMgr::get_ref())
{
	sphere = world::Sphere(p, 0);
	splats.reserve(itrunc(i*200));
	for(float f=0;f<i; f += 1/200.0f) {
		splats.push_back(Fragment(p,Vector(8*frand2()*i,10+8*frand2()*i,8*frand2()*i)));
	}
}

void BloodNGuts::render()
{
	glColor4f(1,0,0,intensity);	
	glVertexPointer(3,GL_FLOAT,sizeof(Fragment),&splats[0]);
	glDrawArrays(GL_POINTS,0,splats.size());
}

void BloodNGuts::setup() const
{
	tr->use("");
}

void BloodNGuts::restore() const
{
}

bool BloodNGuts::simulate_death(float dt)
{
	float sq_radius = 0;
	int cnt = 0;
	for(std::vector<Fragment>::iterator i = splats.begin(); i != splats.end(); ++i) {
		i->vel += Vector(2*frand2(),-10,2*frand2())*dt;
		i->pos += i->vel*dt;
		sq_radius = max(sq_radius, length_sq(sphere.p - i->pos));
	}
	sphere.r = sqrt(sq_radius);
	intensity -= dt/5;
	// keep alive as long as fragment is not black
	return intensity > 0;	
}

}

using namespace lowlevel;

SimEffectPtr sparks(const Point &p, float i) { 
	return SimEffectPtr(new Sparks(p,i)); 
}
SimEffectPtr smokepuff(const Point &p, float i) {
	return SimEffectPtr(new SmokePuff(p,i)); 
}
SimEffectPtr ground_explosion(const Point &p, float i) {
	return SimEffectPtr(new GroundExp(p,i)); 
}
SimEffectPtr air_explosion(const Point &p, const Vector &v, float i) {
	return SimEffectPtr(new AirExp(p,v,i)); 
}
SimEffectPtr grey_smoke(const Point &p, float i) {
	return SimEffectPtr(new GreySmoke(p,i)); 
}
SimEffectPtr white_smoke(const Point &p, float i) {
	return SimEffectPtr(new WhiteSmoke(p,i)); 
}
SimEffectPtr blood_n_guts(const Point &p, float i) {
	return SimEffectPtr(new BloodNGuts(p,i)); 
}

}
}
