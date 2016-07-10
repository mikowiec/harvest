#include "os/compat.h"

#include "gfx/shadow/main.h"
#include "gfx/shadow/utility.h"

namespace reaper {
namespace gfx {
namespace shadow {

const int SilhouetteShadow::texture_size = 128;

SilhouetteShadow::SilhouetteShadow() :
	texture(texture_size, texture_size)
{}

void SilhouetteShadow::set_renderstate()
{
	texture::setup_viewport(texture);
	glColor4f(0, 0, 0, 0.5f);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
}

void SilhouetteShadow::restore_renderstate()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	texture::restore_viewport();
}

int SilhouetteShadow::render(const Frustum &frustum)
{
	object::DynamicPtr player = wr->get_local_player();
	int n_shadows = 0;

	if (player.valid() && add_shadow(*player, frustum)) {
		set_renderstate();
		render_shadow(*player);
		texture.copy_to_texture();
		restore_renderstate();		
		texture.use();
		render_triangles();		

		n_shadows++;
	}

	n_shadows += SimpleShadowDynamic::render(frustum, false);
	return n_shadows;
}

}
}
}

