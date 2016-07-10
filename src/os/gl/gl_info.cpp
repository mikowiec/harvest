
#include "os/compat.h"

#define GLH_EXT_SINGLE_FILE
#include "os/gl.h"


#include "os/gl/gl_info.h"

#include "os/debug.h"
#include "os/gl/gl_state.h"
#include "misc/parse.h"
#include "misc/sequence.h"

#include <string>

#include <set>

namespace reaper {
namespace os {
namespace gfx {


reaper::debug::DebugOutput dout("os::gl",2);

using std::string;

string win_extstr();

#ifdef WIN32
string win_extstr()
{
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = 
		(PFNWGLGETEXTENSIONSSTRINGARBPROC)
		wglGetProcAddress("wglGetExtensionsStringARB");
	if (wglGetExtensionsStringARB)
	{
		const char* winsys_extensions =
			wglGetExtensionsStringARB(wglGetCurrentDC());
		if (winsys_extensions)
			return string(winsys_extensions);
	}
	return "";
}
#else

#ifdef NO_GLXGETCURRENTDISPLAY
Display* glXGetCurrentDisplay() {
	/* We don't give XOpenDisplay any arguments
	 * in gfx_x11, so this should work */
	return XOpenDisplay(0);
}
#endif

string win_extstr()
{
	Display* d = glXGetCurrentDisplay();
	if (!d) 
		return "";
	int scr = DefaultScreen(d);
	const char* winsys_extensions = glXQueryExtensionsString(d, scr);
	if (!winsys_extensions)
		return "";
	return winsys_extensions;
}
#endif


const string gl_getstring(GLenum name)
{
	const char* p = reinterpret_cast<const char*>(glGetString(name));
	return (p != 0) ? p : "";
}


typedef std::set<std::string> Exts;
typedef Exts::const_iterator Exts_iter;

struct OpenGLinfo_data
{
	
	std::string vend;
	std::string rend;
	std::string ver;
	Exts supp_exts;
	Exts fail_exts;

};

OpenGLinfo::OpenGLinfo()
 : data(0)
{
}

void OpenGLinfo::init()
{
	delete data;
	data = new OpenGLinfo_data;
	data->vend = gl_getstring(GL_VENDOR);
	data->rend = gl_getstring(GL_RENDERER);
	data->ver  = gl_getstring(GL_VERSION);

	string extstr(gl_getstring(GL_EXTENSIONS));
	
	data->supp_exts.clear();
	data->fail_exts.clear();
	Exts exts;
	misc::split(extstr, std::inserter(exts, exts.begin()));
	misc::split(win_extstr(), std::inserter(exts, exts.begin()));

	for (Exts_iter i = exts.begin(); i != exts.end(); ++i) {
		if (glh_init_extension(i->c_str()))
			data->supp_exts.insert(*i);
		else
			data->fail_exts.insert(*i);
	}

        glGet(GL_RED_BITS, data2.red_bits);
        glGet(GL_GREEN_BITS, data2.green_bits);
        glGet(GL_BLUE_BITS, data2.blue_bits);
        glGet(GL_ALPHA_BITS, data2.alpha_bits);
        glGet(GL_DEPTH_BITS, data2.depth_bits);
        glGet(GL_STENCIL_BITS, data2.stencil_bits);
        
        glGet(GL_MAX_CLIP_PLANES, data2.max_clip_planes);
        glGet(GL_MAX_LIGHTS, data2.max_lights);
        glGet(GL_MAX_TEXTURE_SIZE, data2.max_texture_size);
       
        float point_size[2];
        glGetFloatv(GL_POINT_SIZE_RANGE, point_size);
        data2.min_point_size = point_size[0];
        data2.max_point_size = point_size[1];
    
        if(is_supported("GL_VERSION_1_2")) {
                glGet(GL_MAX_ELEMENTS_VERTICES, data2.max_elements_vertices);
                glGet(GL_MAX_ELEMENTS_INDICES, data2.max_elements_indices);
        } else if(is_supported("GL_EXT_draw_range_elements")) {
                glGet(GL_MAX_ELEMENTS_VERTICES_EXT, data2.max_elements_vertices);
                glGet(GL_MAX_ELEMENTS_INDICES_EXT, data2.max_elements_indices);
        } else {
                data2.max_elements_vertices = 4096;
                data2.max_elements_indices = 4096;
        }
    
        if(is_supported("GL_VERSION_1_3")) {
                glGet(GL_MAX_TEXTURE_UNITS, data2.max_texture_units);
        } else if(is_supported("GL_ARB_multitexture")) {
                glGet(GL_MAX_TEXTURE_UNITS_ARB, data2.max_texture_units);
        } else {
                data2.max_texture_units = 1;
        }
}

OpenGLinfo::~OpenGLinfo()
{
	delete data;
}

std::ostream& operator<<(std::ostream &s, const OpenGLinfo &info)
{
	s << "Vendor:   " << info.data->vend << '\n';
	s << "Renderer: " << info.data->rend << '\n';
	s << "Version:  " << info.data->ver << '\n';
	s << "Supported extensions:" << '\n';

	s << "    ";
	misc::copy(misc::cseq(info.data->supp_exts),
		std::ostream_iterator<string>(s, "\n    "));
	s << '\n';

	return s;
}

bool OpenGLinfo::is_supported(const std::string& s) const
{
	if (s == "GL_VERSION_1_2")
		return (data->ver.find("1.0") == string::npos &&
			data->ver.find("1.1") == string::npos);
	return data->supp_exts.find(s) != data->supp_exts.end();
}

const std::string& OpenGLinfo::vendor() const { return data->vend; }
const std::string& OpenGLinfo::renderer() const { return data->rend; }
const std::string& OpenGLinfo::version() const { return data->ver; }
const OpenGLinfo_data2& OpenGLinfo::info() const { return data2; }

OpenGLinfo& opengl_info_inst()
{
	static OpenGLinfo inst;
	return inst;
}

void reinit_opengl()
{
	OpenGLinfo& info = opengl_info_inst();
	info.init();
	dout << "OpenGL info:\n" << info;
	reinit_glstates();
}

const OpenGLinfo& opengl_info()
{
	return opengl_info_inst();
}

}
}
}

