
#ifndef REAPER_GFX_CLOUD_Impostor_H
#define REAPER_GFX_CLOUD_Impostor_H

#include "gfx/abstracts.h"
#include "gfx/texture.h"
#include "gfx/camera.h"
#include "gfx/cloud/misc.h"

#include <deque>
#include <map>

namespace reaper {
namespace gfx {
namespace cloud {

struct Layer {
	Vector rel;
	std::pair<float,float> z_range;
	std::map<int, texture::DynamicTexture*> texmap;

	~Layer();
};

class Impostor
{
	Matrix ply_mtx;
	bool enable_flag, regen_flag, use_flag;
	std::deque<Layer> layers;
	int screen_sz;

	Camera last_cam;
	Point cld_pos;
	float radius;
	Color color;
	Project proj;
	Color clear;
	float scale;

	float z_min, z_max;

	texture::DynamicTexture* get_tex(int k, float f);
public:
	Impostor(float r);
	~Impostor();
	bool regen(const Camera&, const Point&, float);
	bool use_impostor();
	void setup(float r);
	int num_layers();
	void grab_texture(int);
	void before_render(const Camera& cam, const Matrix& ply);
	void after_render();
	void render(const Point&, float);
	std::pair<float,float> z_range(int) const;
};

template<class T>
class ImpostorWrap : public T
{
	Impostor imp;
public:
	ImpostorWrap() : imp(get_radius()) { }
	template<class A>
	ImpostorWrap(A a) : T(a), imp(get_radius()) { }
	template<class A, class B>
	ImpostorWrap(A a, B b) : T(a,b), imp(get_radius()) { }

	virtual void setup() const {
		T::setup();
	}
	virtual void render() {
		if (imp.use_impostor())
			imp.render(get_pos(), get_radius());
		else
			T::render(imp.z_range(0));
	}
	virtual void restore() const {
		T::restore();
	}
	virtual void update(const Camera& cam, const Matrix& ply) {
		T::update(cam, ply);
		if (imp.regen(cam, get_pos(), get_radius())) {
			imp.before_render(cam, ply);
			for (int i = 0; i < imp.num_layers(); ++i) {
//				derr << "render to imposter " << imp.z_range(i).first << "  " << imp.z_range(i).second << '\n';
				T::render(imp.z_range(i));
				imp.grab_texture(i);
			}
			imp.after_render();
		}
	}
};


}
}
}


#endif

