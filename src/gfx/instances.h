#ifndef REAPER_GFX_INSTANCES_H
#define REAPER_GFX_INSTANCES_H

#include "gfx/gfx.h"
#include "gfx/gfx_types.h"
#include "gfx/abstracts.h"
#include "object/id.h"

namespace reaper {
namespace gfx {

class impl_accessor {
protected:
	Renderer &r;
	impl_accessor() : r(Renderer::get_unsafe_ref()) {}

	lowlevel::Renderer& ref() const { return *r.i.get(); }
};

void hud_reg(object::ID, Hud* h);

/// Helper class for pointers to graphics entities
template<class T>
class smart_ptr : protected impl_accessor {
protected:
	mutable std::auto_ptr<T> ptr;
	smart_ptr(T* p) : impl_accessor(), ptr(p) {}
	smart_ptr(const smart_ptr<T> &p) : impl_accessor(), ptr(p.ptr) {}
public:
	T* get() const { return ptr.get(); }
	T* release() { return ptr.release(); }
};

class EffectPtr : public smart_ptr<Effect> {
public:
	explicit EffectPtr(Effect *p) : smart_ptr<Effect>(p) {}
	EffectPtr(const EffectPtr &p) : smart_ptr<Effect>(p) {}
	void draw() const;
};

class SimEffectPtr : public smart_ptr<SimEffect> {
	mutable bool owned;
public:
	explicit SimEffectPtr(SimEffect *p) : smart_ptr<SimEffect>(p), owned(true) {}
	SimEffectPtr(const SimEffectPtr &p) : smart_ptr<SimEffect>(p), owned(true) { p.owned = false;}
	~SimEffectPtr();
	void insert() const;
	void insert_release();
	void transfer();
	void remove() const;
};

EffectPtr shot(const Matrix &m, const Color &c); 
EffectPtr sphere(const Matrix &m, float r, const Color &c);

SimEffectPtr shieldsphere(const Matrix &m, float r, const Color &c, const Vector &light);

SimEffectPtr enginetrail(const Matrix &mtx, float size, const Point &offset);
SimEffectPtr smoketrail(const Matrix &mtx, float size, const Point &offset);
SimEffectPtr static_light(const Point &pos, const Color &color, float radius, float fade_speed);
SimEffectPtr dynamic_light(const Matrix &m, const Point &o, const Color &color, 
			       float radius, float fade_speed, bool gravitation = true);

SimEffectPtr sparks(const Point &p, float i);
SimEffectPtr smokepuff(const Point &p, float i);
SimEffectPtr ground_explosion(const Point &p, float i);
SimEffectPtr air_explosion(const Point &p, const Vector &v, float i);
SimEffectPtr grey_smoke(const Point &p, float i);
SimEffectPtr white_smoke(const Point &p, float i);
SimEffectPtr blood_n_guts(const Point &p, float i);

}
}
#endif
