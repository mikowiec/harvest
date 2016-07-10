
#ifndef REAPER_GFX_CLOUD_CLOUD_IMPL_H
#define REAPER_GFX_CLOUD_CLOUD_IMPL_H

#include "main/types.h"
#include "gfx/gfx_types.h"
#include "gfx/cloud/particle.h"
#include "gfx/cloud/simulate.h"
#include "gfx/abstracts.h"
#include "gfx/instances.h"
#include "gfx/texture.h"
#include "gfx/camera.h"
#include "world/query_obj.h"

#include <string>
#include <deque>

namespace reaper {
namespace gfx {
namespace cloud {

struct Light
{
	Point pos;
	Color color;

	Light(Point, Color);
};

typedef std::deque<Light> Lights;


class Cloud
{
	SimCloud sc;
	Point pos;
	float radius;
	float extinction;
	float albedo;
	std::deque<Particle*> particles;
public:
	Cloud(const Point& pos, float radius);
	void init();
	void prelight(Light);
	int render(const Lights&, const Camera& cam, float z_min, float z_max);
	const Point& get_pos() const { return pos; }
	float get_radius() const { return radius; }


	void simul();
};

class CloudMgr
{
	Lights lights;
	Cloud cloud;
	texture::CloudParticle tex;

	void init_cloud();
public:
	CloudMgr(const Point& pos);
	~CloudMgr();
	void init();
	int prepare_render(const Camera& cam, float z_min, float z_max);
	void render(const Point& eye);
	const Point& get_pos() const { return cloud.get_pos(); }
	float get_radius() const { return cloud.get_radius(); }

	void simul();
};

class CloudEffect : public SimEffect, protected impl_accessor
{
	Camera cam;
	
	CloudMgr* cloud;
	world::Sphere sp;
public:
	CloudEffect(CloudMgr* c);

	virtual void init();

	const world::Sphere& get_sphere();
	void setup() const;
	void render(std::pair<float,float> z_range);
	void restore() const;

	virtual void update(const Camera& cam, const Matrix& mtx);

	const Point& get_pos() const;
	float get_radius() const;
	bool simulate_death(float dt);
	void simulate(float dt);

	void simul();
};

}
}
}

#endif

