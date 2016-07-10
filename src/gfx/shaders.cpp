/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:34 $
 * $Log: shaders.cpp,v $
 * Revision 1.4  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.3  2002/05/29 00:35:12  fizzgig
 * cleanup, small fixes, improvement, etc
 *
 * Revision 1.2  2002/03/06 21:08:15  fizzgig
 * Texture-preload now functioning in TexShaders as well..
 *
 * Revision 1.1  2001/06/09 01:58:52  macke
 * Grafikmotor reorg
 *
 */

#include "os/compat.h"

#include <sstream>
#include <list>
#include <map>

#include "gfx/misc.h"
#include "gfx/managers.h"
#include "gfx/exceptions.h"
#include "gfx/shaders.h"
#include "os/debug.h"
#include "os/gl.h"
#include "res/res.h"
#include "res/config.h"

namespace reaper {
namespace gfx  {
namespace shaders {
namespace {
debug::DebugOutput dout("gfx::shaders");

typedef std::map<std::string, int> GLValueMap;

GLValueMap init()
{
	GLValueMap table;

	table["COMBINE"]       = GL_COMBINE_ARB;
	table["BLEND"]         = GL_BLEND;
	table["DECAL"]         = GL_DECAL;
	table["REPLACE"]       = GL_REPLACE;
	table["MODULATE"]      = GL_MODULATE;
	table["ADD"]           = GL_ADD;
	table["ADD_SIGNED"]    = GL_ADD_SIGNED_ARB;
	table["INTERPOLATE"]   = GL_INTERPOLATE_ARB;
	table["SUBTRACT"]      = GL_SUBTRACT_ARB;
	table["TEXTURE"]       = GL_TEXTURE;
	table["CONSTANT"]      = GL_CONSTANT_ARB;
	table["PRIMARY_COLOR"] = GL_PRIMARY_COLOR_ARB;
	table["PREVIOUS"]      = GL_PREVIOUS_ARB;

        table["SRC_COLOR"]           = GL_SRC_COLOR;
        table["ONE_MINUS_SRC_COLOR"] = GL_ONE_MINUS_SRC_COLOR;
        table["SRC_ALPHA"]           = GL_SRC_ALPHA;
        table["ONE_MINUS_SRC_ALPHA"] = GL_ONE_MINUS_SRC_ALPHA;

	return table;
}

int str_to_gl(const std::string& s)
{
	static GLValueMap table(init());
	if(table.find(s) != table.end()) {
		return table[s];
	} else {
		throw gfx_fatal_error(s + " is an illegal parameter for glTexEnv()");
	}
}

void tex_env(GLenum pname, const std::string &param)
{
	glTexEnvi(GL_TEXTURE_ENV, pname, str_to_gl(param));
}
void tex_env(GLenum pname, float params)
{
	glTexEnvf(GL_TEXTURE_ENV, pname, params);
}
} // end anonymous namespace

TexShader::TexShader(const std::string &file) : tr(TextureMgr::get_ref())
{
	using namespace std;
	using res::withdefault;

	res::res_stream rs(res::Shader, file);

	string s;
	getline(rs,s);

	while(rs.good()) {
		stringstream ss;

		do {
			ss << s << "\n";
			getline(rs,s);
		} while(s.find("**TEXENV_BLOCK**") != 0 && rs.good());
		
		res::ConfigEnv env(ss);		

		TexEnv te;
		te.texture = withdefault(env, "TEXTURE", "");
		te.list.begin();
		tex_env(GL_TEXTURE_ENV_MODE,   withdefault(env, "TEXTURE_ENV_MODE", "MODULATE"));
		tex_env(GL_COMBINE_RGB_ARB,    withdefault(env, "COMBINE_RGB",    "MODULATE"));
		tex_env(GL_COMBINE_ALPHA_ARB,  withdefault(env, "COMBINE_ALPHA",  "MODULATE"));
		tex_env(GL_SOURCE0_RGB_ARB,    withdefault(env, "SOURCE0_RGB",    "TEXTURE"));
		tex_env(GL_SOURCE1_RGB_ARB,    withdefault(env, "SOURCE1_RGB",    "PREVIOUS"));
		tex_env(GL_SOURCE2_RGB_ARB,    withdefault(env, "SOURCE2_RGB",    "CONSTANT"));
		tex_env(GL_SOURCE0_ALPHA_ARB,  withdefault(env, "SOURCE0_ALPHA",  "TEXTURE"));
		tex_env(GL_SOURCE1_ALPHA_ARB,  withdefault(env, "SOURCE1_ALPHA",  "PREVIOUS"));
		tex_env(GL_SOURCE2_ALPHA_ARB,  withdefault(env, "SOURCE2_ALPHA",  "CONSTANT"));
		tex_env(GL_OPERAND0_RGB_ARB,   withdefault(env, "OPERAND0_RGB",   "SRC_COLOR"));
		tex_env(GL_OPERAND1_RGB_ARB,   withdefault(env, "OPERAND1_RGB",   "SRC_COLOR"));
		tex_env(GL_OPERAND2_RGB_ARB,   withdefault(env, "OPERAND2_RGB",   "SRC_ALPHA"));
		tex_env(GL_OPERAND0_ALPHA_ARB, withdefault(env, "OPERAND0_ALPHA", "SRC_ALPHA"));
		tex_env(GL_OPERAND1_ALPHA_ARB, withdefault(env, "OPERAND1_ALPHA", "SRC_ALPHA"));
		tex_env(GL_OPERAND2_ALPHA_ARB, withdefault(env, "OPERAND2_ALPHA", "SRC_ALPHA"));
		tex_env(GL_RGB_SCALE_ARB,      withdefault(env, "RGB_SCALE",   1.0));
		tex_env(GL_ALPHA_SCALE,        withdefault(env, "ALPHA_SCALE", 1.0));
		te.list.end();

		texenvs.push_back(te);
	}

	throw_on_gl_error("gfx::TexShader::TexShader()");  
}

void TexShader::use()
{
	int t = 0;
	for(Cont::iterator i = texenvs.begin(); i != texenvs.end(); ++i) {
		glActiveTextureARB(GL_TEXTURE0_ARB + t++);
		tr->use(i->texture);
		i->list.call();
	}
}

void TexShader::restore()
{
	int t = 0;
	for(Cont::const_iterator i = texenvs.begin(); i != texenvs.end(); ++i) {
		glActiveTextureARB(GL_TEXTURE0_ARB + t++);
		tr->use("");
	}
	glActiveTextureARB(GL_TEXTURE0_ARB);			
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void TexShader::pre_load()
{
	for(Cont::iterator i = texenvs.begin(); i != texenvs.end(); ++i) {
		tr->load(i->texture);
	}

}

}
}
}
