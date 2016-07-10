#include "os/compat.h"

#include "gfx/texture.h"
#include "gfx/exceptions.h"
#include "gfx/settings.h"

namespace reaper {
namespace gfx {
namespace texture {

DynamicTexture::DynamicTexture(int w, int h, bool black_white)
: width(w), height(h)
{
	use();
	glTexParameter2D(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameter2D(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if(Settings::current.use_arb_texture_border_clamp) {
    	    glTexParameter2D(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_ARB);
	    glTexParameter2D(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_ARB);

	    if(Settings::current.use_sgis_generate_mipmap) {
		    glTexParameter2D(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		    glTexParameter2D(GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
	    } else {
		    glTexParameter2D(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    }
        } else {
    	    glTexParameter2D(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	    glTexParameter2D(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

	int max_s = max(w,h);
	int max_hw = glGetInteger(GL_MAX_TEXTURE_SIZE);
	if(max_s > max_hw ) {
		int s = max_s / max_hw;
		w /= s;
		h /= s;
	}

	//FIXME: Optimize formats for different frame buffer layouts (16-bit, 24-bit)
	glTexImage2D(GL_TEXTURE_2D, 0, black_white ? GL_ALPHA8 : GL_RGBA8,
	             width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	set_size(width*height * black_white ? 1 : 4);

        throw_on_gl_error("DynamicTexture::init(): ");
}

// copies framebuffer to texture
void DynamicTexture::copy_to_texture()
{
	use();
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
}

void setup_viewport(int w, int h)
{
	glPushAttrib(GL_SCISSOR_BIT | GL_VIEWPORT_BIT);
	glEnable(GL_SCISSOR_TEST);
	glViewport(0, 0, w, h);
	glScissor(0, 0, w, h);
}

void setup_viewport(const DynamicCubeMap &cm)
{
	setup_viewport(cm.w, cm.w);
}

void setup_viewport(const DynamicTexture &t)
{
	setup_viewport(t.width, t.height);
}

void restore_viewport()
{
	glPopAttrib();
}

}
}
}
