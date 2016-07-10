
#include "os/compat.h"

#include <iterator>
#include <functional>
#include <algorithm>

#include "main/types_ops.h"
#include "main/types4_ops.h"
#include "main/types_io.h"
#include "main/types4_io.h"

#include "misc/utility.h"

#include "gfx/cloud/cloud.h"
#include "gfx/cloud/cloud_impl.h"
#include "gfx/cloud/impostor.h"
#include "gfx/cloud/misc.h"
#include "gfx/cloud/simulate.h"
#include "gfx/texture.h"
#include "gfx/camera.h"
#include "gfx/renderer.h"
#include "gfx/misc.h"
#include "misc/sequence.h"
#include "misc/stlhelper.h"

#include "misc/profile.h"
#include "os/gl.h"
#include "os/gl/gl_helper.h"
#include "os/debug.h"

namespace reaper {
namespace gfx {
namespace cloud {

namespace { debug::DebugOutput derr("cloud"); }

using namespace reaper::misc;


Light::Light(Point p, Color c) : pos(p), color(c) { }


class SetupGL
{
	StateKeeper depth, fog, light, texture, blend;
public:
	SetupGL()
	 : depth(GL_DEPTH_TEST, false),
	   fog(GL_FOG, false),
	   light(GL_LIGHTING, false),
	   texture(GL_TEXTURE_2D, true),
	   blend(GL_BLEND, true)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	~SetupGL()
	{
	}

	static void camera(const Point& p, const Point& at, float radius) {
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(90, 1.33, 1, 10000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Vector a = norm(p - at);
		Vector up(0,1,0);
		Vector left(cross(up, a));
		if (length(left) < 0.1) {
			up = Vector(0,0,1);
			left = cross(up, a);
		}
		up = cross(a, left);
		gluLookAt(p.x, p.y, p.z,
			  at.x, at.y, at.z, 
			  up.x, up.y, up.z);
	}			
};

template<class C>
void cube(C& particles)
{
	particles.push_back(new Particle(Point(0, 0, 0), 2));
	particles.push_back(new Particle(Point(5, 0, 0), 2));
	particles.push_back(new Particle(Point(0, 5, 0), 2));
	particles.push_back(new Particle(Point(5, 5, 0), 2));
	particles.push_back(new Particle(Point(0, 0, 5), 2));
	particles.push_back(new Particle(Point(5, 0, 5), 2));
	particles.push_back(new Particle(Point(0, 5, 5), 2));
	particles.push_back(new Particle(Point(5, 5, 5), 2));
}

template<class C>
void medium_cube(C& ps)
{
	for (int i = 0; i < 10; i+=3) {
		for (int j = 0; j < 10; j+=5) {
			for (int k = 0; k < 10; k+=2) {
				ps.push_back(new Particle(Point(i, j, k), 3.0));
			}
		}
	}
}

template<class C>
void big_cube(C& ps)
{
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			for (int k = 0; k < 10; ++k) {
				ps.push_back(new Particle(Point(i, j, k), 1.0));
			}
		}
	}
}

template<class C>
void random1(C& ps)
{
	reaper::misc::CenterRand rnd(-1.0, 1.0);
	for (int i = 0; i < 1000; ++i) {
		ps.push_back(new Particle(Point(5*rnd(), 4*rnd(), 8*rnd()), 1.1 + rnd(), 8.0, 0.9));

	}
}

Particle* rnd_particle(reaper::misc::FakeRand& rnd)
{
	Point p(rnd(), rnd(), rnd());
	float size = 3.5 + 7*rnd()*(2-length(p));
	return new Particle(Point(p*Point(40,12,55)), size);
}

template<class C>
void random2(C& ps)
{
	reaper::misc::FakeRand rnd(-1.0, 1.0);
	for (int i = 0; i < 2000; ++i) {
		ps.push_back(rnd_particle(rnd));
	}
}

template<class C, class P>
void simul1(C& sc, P& ps)
{
	reaper::misc::FakeRand rnd(-1,1);
	for (int i = 0; i < 150; ++i)
		sc.simulate();
	for (int x = 0; x < 60; x++) {
		for (int y = 0; y < 20; ++y) {
			for (int z = 0; z < 60; ++z) {
				float f = sc.get(x,y,z);
				if (sc.get(x,y,z) > 0.5) {
					Point p((30.0-x)*3.5, (10.0-y)*2.5, (30.0-z)*4.0);
					p += Point(rnd(), rnd(), rnd());
					ps.push_back(new Particle(p, f*25.0));
				}
			}
		}
	}
	
}

Cloud::Cloud(const Point& p, float r)
 : sc(60, 20, 60), pos(p), radius(r),
   extinction(8.0), albedo(0.9)
{
	derr << "Generating cloud\n";
//	cube(particles);
//	medium_cube(particles);
//	big_cube(particles);
//	random2(particles);
//	sc.simulate();
	simul1(sc, particles);
}

void uncolor(Particle* p)
{
	p->colors.clear();
}

void Cloud::init()
{
	for_each(seq(particles), uncolor);
}

struct ZDistMin : public std::binary_function<const Particle&, const Particle&, bool>
{
	Vector dir;
	ZDistMin(const Vector& d) : dir(d) { }
	bool operator()(const Particle* p1, const Particle* p2) const {
		return dot(p1->pos,dir) < dot(p2->pos,dir);
	}
};


float phase(const Vector& v1, const Vector& v2)
{
	float cos_theta = dot(v1, v2) / (length(v1)*length(v2));
	return 3.0 / 4.0 * (1 + cos_theta*cos_theta);
}

void Cloud::prelight(Light light)
{
	derr << "Prelighting " << particles.size() << " particles\n";
	Vector l = pos - light.pos;

	SetupGL::camera(light.pos, pos, radius);
	misc::BillBoard::set_vectors();

	Project proj;

	sort(seq(particles), ZDistMin(light.pos - pos));
	for (int k = particles.size()-1; k >= 0; --k) {
		Particle* p_k = particles[k];

		Point p = p_k->pos;
		Point2D p_scr(proj(p+pos));

		int x = static_cast<int>(floor(p_scr.x));
		int y = static_cast<int>(floor(p_scr.y));
		Color c_x(1,1,1,1);
		glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, c_x.get());

		Color i_k = c_x * light.color;

		Color p_c(albedo * extinction * i_k / (4*3.14));

		Color c = p_c * 3.0 / 2.0;
		c.a = 1.0;
//		derr << "color for particle " << Point(light.pos - (pos+p_k.pos)) << " " << k << "  " << c.r << ' ' << c.g << ' ' << c.b << ' ' << c.a << '\n';
		misc::BillBoard::add(p_k->radius, pos + p, c);
		misc::BillBoard::render_colors();

		p_c.a = 1 - exp(-extinction);

		p_k->colors.push_back(p_c);
	}
}

int Cloud::render(const Lights& lights, const Camera& cam, float z_min, float z_max)
{
	misc::BillBoard::set_vectors();

	sort(seq(particles), ZDistMin(cam.pos - pos));

	Vector e_e = norm(pos-cam.pos);
//	derr << "dist: " << e_e << ' ' << z_min << ' ' << z_max << '\n';
	int n = 0;
	for (size_t k = 0; k < particles.size(); ++k) {
		const Particle* p_k = particles[k];
		Vector omega = pos+p_k->pos - cam.pos;
		float len = dot(p_k->pos, e_e);
		if (len < z_min || len > z_max) {
//			derr << "skipping: " << len << " not in " << z_min << ' ' << z_max << '\n';
			continue;
		}
		++n;
		Color c(0.1,0.1,0.1,1);
		for (size_t l = 0; l < lights.size(); ++l) {
			c += p_k->colors[l] * phase(omega, pos+p_k->pos - lights[l].pos);
			max_clamp(c);
		}
//		Color c = p_k.colors[light.id] * phase(omega, pos - light.pos);
		c.a = 1.0;
//		derr << obs << " " << Point(pos+p_k.pos) << " " << length(Point(obs - (pos+p_k.pos))) << ' ' << c.r << ' ' << c.b << ' ' << c.r << ' ' << c.a << '\n';
		misc::BillBoard::add(p_k->radius, pos + p_k->pos, c);
	}
	return n;
}

void Cloud::simul()
{
	for (int i = 0; i < 1; ++i)
		sc.simulate();
	particles.clear();
	simul1(sc, particles);
}

CloudMgr::CloudMgr(const Point& p)
 : cloud(p, 200)
{
	lights.push_back(Light(Point(2800,  800, 2800), Color(1.0, 1.0, 1.0, 1.0)));
	lights.push_back(Light(Point(2500,  650, 2800), Color(1.0, 1.0, 1.0, 1.0)));
	lights.push_back(Light(Point(2800,  850, 2800), Color(0.6, 0.6, 0.8, 1.0)));
	lights.push_back(Light(Point(3000,  800, 2600), Color(0.5, 0.1, 0.1, 1.0)));
}

CloudMgr::~CloudMgr()
{
	
}

void CloudMgr::init_cloud()
{
	cloud.init();
	for_each(seq(lights), apply_to(&cloud, &Cloud::prelight));
}

void CloudMgr::init()
{
	reaper::misc::time::Profiler pf;
	pf.start();
	SetupGL gl;
	StateKeeper st1(GL_DEPTH_TEST, false);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glColor4f(1,1,1,1);
//	texture::Texture tex("cloud_particle");
	tex.use();
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	init_cloud();
	pf.stop();
	derr << "init " << pf.get_last() << '\n';
}

void CloudMgr::simul()
{
	cloud.simul();
	for_each(seq(lights), apply_to(&cloud, &Cloud::prelight));
}

int CloudMgr::prepare_render(const Camera& cam, float z_min, float z_max)
{
	return cloud.render(lights, cam, z_min, z_max);
}

void CloudMgr::render(const Point&)
{
	SetupGL gl;
	StateKeeper st1(GL_DEPTH_TEST, true);
	glDepthFunc(GL_LEQUAL);
	tex.use();
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	misc::BillBoard::render_colors();
//	derr << "render real at " << get_pos() << '\n';
}

CloudEffect::CloudEffect(CloudMgr* c)
 : SimEffect("cloud", "cloud"), cloud(c),
   sp(cloud->get_pos(), cloud->get_radius())
{
}

void CloudEffect::init()
{
	cloud->init();
	ref().insert(this);
}

const world::Sphere& CloudEffect::get_sphere()
{
	return sp;
}

void CloudEffect::setup() const
{
}

void CloudEffect::render(std::pair<float,float> z_range)
{
	reaper::misc::time::Profiler pf;
	pf.start();
	int n = cloud->prepare_render(cam, z_range.first, z_range.second);
	cloud->render(cam.pos);
	pf.stop();
	derr << "render " << n << " particles, " << pf.get_last() << " us\n";
}

void CloudEffect::restore() const
{
}

const Point& CloudEffect::get_pos() const
{
	return sp.p;
}

float CloudEffect::get_radius() const
{
	return sp.r;
}

bool CloudEffect::simulate_death(float dt)
{
//	sp.p = mtx.pos();
	return true;
}

void CloudEffect::simulate(float dt)
{
	simulate_death(dt);
}

void CloudEffect::simul()
{
	cloud->simul();
}

void CloudEffect::update(const Camera& c, const Matrix& mtx)
{
	cam = c;
}

void CloudSystem::init()
{
	CloudEffect* eff1 = new ImpostorWrap<CloudEffect>(new CloudMgr(Point(2800, 600, 2800)));
	eff1->init();
	clouds.push_back(eff1);

}

void CloudSystem::simul()
{
//	for_each(seq(clouds), std::mem_fun(&CloudEffect::simul));
	for (size_t i = 0; i < clouds.size(); ++i) {
		clouds[i]->simul();
	}

}

void CloudSystem::update(const Camera& cam, const Matrix& mtx)
{
//	for_each(seq(clouds), std::bind2nd(std::mem_fun(&CloudEffect::update), mtx));
	for (size_t i = 0; i < clouds.size(); ++i) {
		clouds[i]->update(cam, mtx);
	}
}


}
}
}

