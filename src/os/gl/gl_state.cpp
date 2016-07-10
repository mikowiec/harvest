
#include "os/compat.h"

#include <vector>

#include "os/gl.h"
#include "os/gl/gl_info.h"
#include "os/debug.h"
#include "misc/map.h"

#include "misc/sequence.h"

namespace reaper {
namespace {
debug::DebugOutput dout("gl::state",0);

template<class Map>
void clear_map(Map& map)
{
	using namespace reaper::misc;
	std::vector<typename Map::mapped_type> tmp(map.size());
	typename Map::iterator c, e = map.end();
	for (c = map.begin(); c != e; ++c) {
		dout << "del: " << c->second << '\n';
		tmp.push_back(c->second);
	}
	map.clear();
	for_each(seq(tmp), delete_it);
}

// INT_MAX is not good, but better than -1 (unsigned...)
const GLuint Not_Init = INT_MAX;

typedef void (APIENTRY * PFNGLSTATEFUNC)(GLenum state);

template<PFNGLSTATEFUNC enable_func, PFNGLSTATEFUNC disable_func>
class State
{
	GLenum    state;
	GLboolean enabled;
public:
	State(GLenum st, GLboolean en) : state(st), enabled(en) {} 
	State(GLenum st) : state(st), enabled(::glIsEnabled(state)) { }

	inline void enable()  {
		if(enabled == GL_FALSE) { 
			enable_func(state); 
			enabled = GL_TRUE; 
		}
	}
	inline void disable() {
		if(enabled == GL_TRUE) {
			disable_func(state); 
			enabled = GL_FALSE; 
		} 
	}

	inline GLboolean is_enabled() const { return enabled; }
};

/* we need to keep one state for each texture unit */
template<class T>
std::vector<T> init(GLenum state)
{
	return std::vector<T>(32, T(state, GL_FALSE));
}

typedef State<&::glEnable,            &::glDisable>            ServerState;
typedef State<&::glEnableClientState, &::glDisableClientState> ClientState;

typedef reaper::misc::Map<GLenum, ServerState> ServerStateMap;
typedef reaper::misc::Map<GLenum, ClientState> ClientStateMap;

// Global state cache variables

ServerStateMap gl_states("GL State");
ClientStateMap gl_client_states("GL Client State");

std::map<int, std::vector<ServerState> > tex_states;
std::map<int, std::vector<GLuint> > active_tex;
std::vector<ClientState> texture_array(init<ClientState>(GL_TEXTURE_COORD_ARRAY));

GLenum active_texture_unit        = GL_TEXTURE0_ARB;
GLenum active_client_texture_unit = GL_TEXTURE0_ARB;

GLenum src_blend_factor = Not_Init;
GLenum dst_blend_factor = Not_Init;

} // end anonymous namespace

void reinit_glstates()
{

	gl_states.purge();
	gl_client_states.purge();

	tex_states[GL_TEXTURE_2D]    = init<ServerState>(GL_TEXTURE_2D);
	tex_states[GL_TEXTURE_GEN_S] = init<ServerState>(GL_TEXTURE_GEN_S);
	tex_states[GL_TEXTURE_GEN_T] = init<ServerState>(GL_TEXTURE_GEN_T);
	tex_states[GL_TEXTURE_GEN_R] = init<ServerState>(GL_TEXTURE_GEN_R);
	tex_states[GL_TEXTURE_GEN_Q] = init<ServerState>(GL_TEXTURE_GEN_Q);

	active_tex[GL_TEXTURE_1D] = std::vector<GLuint>(32, Not_Init);
	active_tex[GL_TEXTURE_2D] = std::vector<GLuint>(32, Not_Init);

	texture_array = init<ClientState>(GL_TEXTURE_COORD_ARRAY);

	if (os::gfx::opengl_info().is_supported("GL_ARB_texture_cube_map")) {	
		tex_states[GL_TEXTURE_CUBE_MAP_ARB] = init<ServerState>(GL_TEXTURE_CUBE_MAP_ARB);
		active_tex[GL_TEXTURE_CUBE_MAP_ARB] = std::vector<GLuint>(32, Not_Init);
	}

	active_texture_unit        = GL_TEXTURE0_ARB;
	active_client_texture_unit = GL_TEXTURE0_ARB;

	src_blend_factor = Not_Init;
	dst_blend_factor = Not_Init;

	glEnableClientState(GL_VERTEX_ARRAY);
}

GLvoid glActiveTextureARB(GLenum unit)
{
	if(unit != active_texture_unit) {
		active_texture_unit = unit;
		::glActiveTextureARB(unit);
	}
}
GLvoid glClientActiveTextureARB(GLenum unit)
{
	if(unit != active_client_texture_unit) {
		active_client_texture_unit = unit;
		::glClientActiveTextureARB(unit);
	}
}

GLvoid glBindTexture(GLenum target, GLuint texture)
{
	if(texture != active_tex[target][active_texture_unit - GL_TEXTURE0_ARB]) {
		active_tex[target][active_texture_unit - GL_TEXTURE0_ARB] = texture;
		::glBindTexture(target,texture);
	}
}

GLboolean glIsEnabled(GLenum state)
{
	switch(state) {
	case GL_TEXTURE_2D:
	case GL_TEXTURE_CUBE_MAP_ARB:
	case GL_TEXTURE_GEN_S: 
	case GL_TEXTURE_GEN_T: 
	case GL_TEXTURE_GEN_R: 
	case GL_TEXTURE_GEN_Q: 
		return tex_states[state][active_texture_unit - GL_TEXTURE0_ARB].is_enabled();

	case GL_VERTEX_ARRAY: 
	case GL_NORMAL_ARRAY: 
	case GL_COLOR_ARRAY: 
	case GL_INDEX_ARRAY: 
	case GL_EDGE_FLAG_ARRAY: 
	case GL_VERTEX_ARRAY_RANGE_NV:
	case GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV:
		return gl_client_states.get(state).is_enabled();
	case GL_TEXTURE_COORD_ARRAY:
		return texture_array[active_client_texture_unit - GL_TEXTURE0_ARB].is_enabled();
	default:
		return gl_states.get(state).is_enabled();
	}
}

GLvoid glEnable(GLenum state)
{
	switch(state) {
	case GL_TEXTURE_2D:
	case GL_TEXTURE_CUBE_MAP_ARB:
	case GL_TEXTURE_GEN_S: 
	case GL_TEXTURE_GEN_T: 
	case GL_TEXTURE_GEN_R: 
	case GL_TEXTURE_GEN_Q: 
		tex_states[state][active_texture_unit - GL_TEXTURE0_ARB].enable();
		break;
	default:
		gl_states.get(state).enable();
	}
}

GLvoid glDisable(GLenum state)
{
	switch(state) {
	case GL_TEXTURE_2D:
	case GL_TEXTURE_CUBE_MAP_ARB:
	case GL_TEXTURE_GEN_S: 
	case GL_TEXTURE_GEN_T: 
	case GL_TEXTURE_GEN_R: 
	case GL_TEXTURE_GEN_Q: 
		tex_states[state][active_texture_unit - GL_TEXTURE0_ARB].disable();
		break;
	default:
		gl_states.get(state).disable();
	}
}

GLvoid glEnableClientState(GLenum state)
{
	if(state == GL_TEXTURE_COORD_ARRAY) {
		texture_array[active_client_texture_unit - GL_TEXTURE0_ARB].enable();
	} else {
		gl_client_states.get(state).enable();
	}
}

GLvoid glDisableClientState(GLenum state)
{
	if(state == GL_TEXTURE_COORD_ARRAY) {
		texture_array[active_client_texture_unit - GL_TEXTURE0_ARB].disable();
	} else {
		gl_client_states.get(state).disable();
	}
}

GLvoid glBlendFunc(GLenum src, GLenum dst)
{
	if(src != src_blend_factor || dst != dst_blend_factor) {
		src_blend_factor = src;
		dst_blend_factor = dst;
		::glBlendFunc(src_blend_factor, dst_blend_factor);
	}
}

///////////////////////////////////////////////////////////////////////////////////////

StateKeeperBase::StateKeeperBase(GLenum st, bool val)
 : state(st), value(val), prev_val(glIsEnabled(state) == GL_TRUE)
{ }

StateKeeperBase::~StateKeeperBase() { }

StateKeeper::StateKeeper(GLenum st, bool val)
 : StateKeeperBase(st, val)
{	
	if (prev_val != value) {
		value ? glEnable(state) : glDisable(state);
	}
}

StateKeeper::~StateKeeper()
{
	if (prev_val != value) {
		prev_val ? glEnable(state) : glDisable(state);
	}
}

ClientStateKeeper::ClientStateKeeper(GLenum st, bool val)
 : StateKeeperBase(st, val)
{	
	if(prev_val != value) {
		value ? glEnableClientState(state) : glDisableClientState(state);
	}
}

ClientStateKeeper::~ClientStateKeeper()
{
	if(prev_val != value) {
		prev_val ? glEnableClientState(state) : glDisableClientState(state);
	}
}

} // end reaper namespace
