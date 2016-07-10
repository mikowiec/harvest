
#ifndef REAPER_OBJECT_PROJECTILE_H
#define REAPER_OBJECT_PROJECTILE_H

#include "object/hull.h"
#include "object/phys_data.h"
#include "object/forward.h"
#include "object/phys.h"
#include "object/renderer.h"
#include "object/factory.h"
#include "object/sound.h"
#include "object/collide.h"
#include "object/reg.h"
#include "object/object_impl.h"

namespace reaper {
namespace object {


class ProjectileBase : public ShotBase
{
protected:
	SillyData data;
	ShotInfo phys_data;
	Vector velocity;
	std::auto_ptr<StatAcc> sacc;
	void move();

	reaper::phys::ShotAccessor* acc;
	sound::Projectile* sound;
	hull::Shielded hull;  //Dummy hull

public:
	ProjectileBase(const std::string& name, const Matrix &mtx, CompanyID c, ID p);
	~ProjectileBase();

	void initialize();

	event::Events& events() { return *data.events; }

	const GetStat& stat() const { return *sacc; };
};

class Laser
 : public ProjectileBase
{
	std::auto_ptr<renderer::Laser> rd;

	void on_kill();
public:
	Laser(const Matrix &mtx, CompanyID c, ID p);
	const gfx::RenderInfo* render(bool effects) const;
};

class Missile : public ProjectileBase
{
	std::auto_ptr<renderer::Missile> rd;

	void on_kill();
public:
	Missile(const std::string& id, const Matrix &mtx, CompanyID c, ID p);
	const gfx::RenderInfo* render(bool effects) const;
};

}
}
#endif

