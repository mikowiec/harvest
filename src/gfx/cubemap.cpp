#include "os/compat.h"

#include "gfx/texture.h"
#include "gfx/settings.h"

namespace reaper {
namespace gfx {
namespace texture {

CubeMap::CubeMap(int s)
: w(min(s, glGetInteger(GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB)))
{
}

void CubeMap::enable_reflection_map()
{
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
}
void CubeMap::enable_normal_map()
{
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
}

void CubeMap::restore_mapping()
{
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
}

DynamicCubeMap::DynamicCubeMap(int s) : CubeMap(s)
{	
	use();

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);	
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(Settings::current.use_sgis_generate_mipmap) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR	);
	} else {
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}
	
	for(int i = PosX; i < PosX + 6; i++ ) {
		glTexImage2D(i, 0, GL_RGBA8, w, w, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}
	float size = w*w*4*6* Settings::current.use_sgis_generate_mipmap ? 1.33f : 1;
	set_size(static_cast<unsigned int>(size));
}

void DynamicCubeMap::copy_to_texture(Side side)
{
	use();
	glCopyTexSubImage2D(side, 0, 0, 0, 0, 0, w, w);

}

}
}
}

