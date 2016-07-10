#ifndef REAPER_MESH_TEXTURE_H
#define REAPER_MESH_TEXTURE_H

#include "os/compat.h"
#include "gfx/misc.h"
#include <iostream>
#include "glhelpers.h"
#include <string.h>
#include "os/gl.h"
#include "gl/glext.h"
#include <vector>

namespace reaper 
{
namespace gfx
{
namespace mesh
{

class Texture 
{
public:
	char filename[64];
	unsigned int id;
	int w, h;
	char* data;

	void set_id(int i){id = i;};

	void use(int n, bool blend);

	void stop();

	void setup();

	void load();

	void load(char* filename);

	void report();
};

class TextureMgr
{
public:
	std::vector<Texture> tex;

	const int cnt() const{return tex.size();}

	void new_tex(char* filename);

	void prepare();

	void stop();

	void clear(){tex.clear();}

//	void use();

	void use(int id, bool blend);

};

}
}
}

#endif
