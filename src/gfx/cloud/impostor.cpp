
#include "os/compat.h"

#include "main/types_ops.h"
#include "main/types_io.h"

#include "os/debug.h"
#include "gfx/cloud/impostor.h"
#include "gfx/cloud/misc.h"
#include "gfx/misc.h"

#include "misc/sequence.h"
#include "misc/free.h"

namespace reaper {
namespace gfx {
namespace cloud {

using namespace reaper::misc;

namespace { debug::DebugOutput derr("cloud"); }

Layer::~Layer()
{
	for_each(seq(texmap), delete_it);
}

Impostor::Impostor(float r)
 : enable_flag(false), regen_flag(false), use_flag(false), scale(1.0),
   z_min(0), z_max(10000)
{
	last_cam.pos = Point(0,0,0);
	setup(r);
}

Impostor::~Impostor()
{
}

void Impostor::setup(float r)
{
	Layer def;
	def.rel = Vector(0,0,0);
	def.z_range.first = -r;
	def.z_range.second = r;
	layers.push_back(def);
}

std::pair<float,float> Impostor::z_range(int k) const 
{
	return layers[k].z_range;
}

bool Impostor::regen(const Camera& cam, const Point& pos, float r)
{
	cld_pos = pos;
	radius = r;
	if (!enable_flag)
		return false;

	Vector v1 = norm(cld_pos - cam.pos);
	Vector v2 = norm(cld_pos - last_cam.pos);
	
	Point2D p1 = proj(cld_pos + v1*radius);
	Point2D p2 = proj(cld_pos + v2*radius);

	Point2D p3 = proj(cld_pos + cam.up*radius);
	Point2D p4 = proj(cld_pos + last_cam.up*radius);

	if (length(p1 - p2) > 1 || length(p3-p4) > 1) {
		if (!regen_flag)
			derr << "regenerates texture\n";
		regen_flag = true;
		last_cam = cam;
	}
	if (length(cam.pos - cld_pos) < radius) {
		if (use_flag)
			derr << "disable\n";
		use_flag = false;
		regen_flag = false;
	} else {
		use_flag = true;
	}
	return regen_flag;
}

bool Impostor::use_impostor()
{
	enable_flag = true;
	proj.save();
	return use_flag;
}

int sel_sz(float sz)
{
	if (sz > 63)
		return 256;
	if (sz > 7)
		return 64;
	return 8;
}

texture::DynamicTexture* Impostor::get_tex(int k, float f)
{
	int sz = sel_sz(f);
	Layer& l = layers[k];
	if (l.texmap.find(sz) == l.texmap.end())
		l.texmap[sz] = new texture::DynamicTexture(sz,sz);
	return l.texmap[sz];
}

void Impostor::before_render(const Camera& cam, const Matrix& ply)
{
	Vector at_cloud = cld_pos - cam.pos;
	float dist = length(at_cloud);
	
	Vector r = radius*proj.right();
	Vector u = radius*proj.up();

	float x1 = proj(cld_pos-r).x,
	      x2 = proj(cld_pos+r).x,
	      y1 = proj(cld_pos-u).y,
	      y2 = proj(cld_pos+u).y;
	
	screen_sz = static_cast<int>(ceil(max(x2-x1, y2-y1)));
	float ts = sel_sz(screen_sz);
	scale = max(1.0f, ts / screen_sz);

	Vector e_r = proj.right();
	Vector e_u = proj.up();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(cam.vert_fov, 1, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector cld = norm(cld_pos - cam.pos);
	float d = radius / tan(37.5 * 3.14/180);
	Point eye = cld_pos - cld*d;

	gluLookAt(eye.x, eye.y, eye.z, 
		  cld_pos.x, cld_pos.y, cld_pos.z,
		  e_u.x, e_u.y, e_u.z);

	texture::setup_viewport(*get_tex(0, screen_sz));
	glGetFloatv(GL_COLOR_CLEAR_VALUE, clear.get());
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

/* imposter splitting not implemented yet
	if (length(at_cloud) < radius) {
		float cam_ply_dist = length(ply.pos() - cam.pos);
		Vector e_a = norm(at_cloud);

		layers.clear();
		float split_at = dot(ply.pos() - cld_pos, e_a);

		Layer l1, l2;
		l1.z_range.first = -radius;
		l1.z_range.second = split_at;
		l1.rel = e_a * (split_at - cam_ply_dist/4.0);
		l2.z_range.first = split_at;
		l2.z_range.second = radius;
		l2.rel = e_a * radius;

		layers.push_back(l2);
		layers.push_back(l1);

	} else {
*/
		if (layers.size() != 1) {
			layers.clear();
			setup(radius);
		}
//	}
}

int Impostor::num_layers()
{
	return layers.size();
}

void Impostor::grab_texture(int k)
{
	get_tex(k, screen_sz)->copy_to_texture();
}

void Impostor::after_render()
{
	glClearColor(clear);
	texture::restore_viewport();
	regen_flag = false;
	use_flag = true;
	enable_flag = false;
}

void Impostor::render(const Point& pos, float radius)
{
	StateKeeper st1(GL_TEXTURE_2D, true);
	StateKeeper st2(GL_BLEND, true);
	StateKeeper st3(GL_DEPTH_TEST, true);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	misc::BillBoard::set_vectors();
	for (size_t k = 0; k < layers.size(); ++k) {
		get_tex(k, screen_sz)->use();
		Vector rel = layers[k].rel;
		misc::BillBoard::add(radius, pos + rel, Color(1,1,1));
//		derr << "render impostor at " << pos << ' ' << rel << ' ' << length(pos+rel-last_cam.pos) << '\n';
	}
	misc::BillBoard::render_colors();
}


}
}
}


