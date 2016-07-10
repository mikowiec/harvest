
#ifndef REAPER_HW_GL_EXT_H
#define REAPER_HW_GL_EXT_H

#include <string>

namespace reaper {
namespace os {
namespace gfx {

struct OpenGLinfo_data;

struct OpenGLinfo_data2 
{
        int blue_bits;
        int red_bits;
        int green_bits;
        int alpha_bits;
        int depth_bits;
        int stencil_bits;

        int max_clip_planes;
        int max_lights;
        int max_texture_size;

        float min_point_size;
        float max_point_size;

        int max_elements_vertices;
        int max_elements_indices;
        int max_texture_units;
};

class OpenGLinfo
{
	OpenGLinfo_data* data;
        OpenGLinfo_data2 data2;

	friend OpenGLinfo& opengl_info_inst();
	friend void reinit_opengl();

	OpenGLinfo();
	OpenGLinfo(const OpenGLinfo&);
	void init();
public:
	~OpenGLinfo();

	const std::string& vendor() const;
	const std::string& renderer() const;
	const std::string& version() const;
        
        const OpenGLinfo_data2& info() const;
	bool is_supported(const std::string& s) const;

	friend std::ostream& operator<<(std::ostream &s, const OpenGLinfo &i);
};

std::ostream& operator<<(std::ostream&, const OpenGLinfo&);

// This needs to be called when a new opengl context has been created.
// It calls reinit_glstates as well.
void reinit_opengl();


const OpenGLinfo& opengl_info();


}
}
}

#endif

