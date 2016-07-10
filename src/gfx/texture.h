#ifndef REAPER_GFX_TEXTURE_H
#define REAPER_GFX_TEXTURE_H

#include "os/gl.h"
#include "misc/unique.h"
#include <set>

namespace reaper {
namespace gfx {
namespace texture {

using std::string;
using reaper::misc::Unique;

struct TexInfo {
	unsigned int size, name;
	TexInfo(unsigned int s, unsigned int n) : size(s), name(n) {}

	struct ptr_less {
		bool operator()(const TexInfo *x, const TexInfo *y) const {
			return x->name < y->name;
		}
	};
};

//FIXME: Texture is not appropriate to store in standard containers
// due to deletion of texture upon destruction. Store pointers to it
// until this has been solved

class TextureBase
{
public:
	typedef std::set<TexInfo*, TexInfo::ptr_less> TexCont; 

	~TextureBase();

	unsigned int get_size() { return i.size; }
	unsigned int get_name() { return i.name; }
	
	TexInfo i;

	static unsigned int all_textures_size();
	static unsigned int resident_textures_size();	
	static TexCont textures;
protected:	
	void set_size(unsigned int s);
	TextureBase();
};

/////////////////////////////////////////////////////////////////////////

/// Two-dimensional texture	
class Texture2D : public TextureBase
{
public:
        void use() const { glBindTexture2D(i.name); } ///< Tells OpenGL to use this texture for rendering        
protected:
        int w;	///< Texture width (pixels)        
        int h;  ///< Texture height (pixels)
};

class CubeMap : public TextureBase
{
public:
	enum Side { 
		PosX = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
		NegX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
		PosY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 
		NegY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 
		PosZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, 
		NegZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
	};

	CubeMap(int size);

	void use() const { glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, i.name); } 

	void enable_reflection_map();
	void enable_normal_map();
	void restore_mapping();
protected:
	int w; // cube maps have (6) square textures
};

//////////////////////////////////////////////////////////////////////////

/// PNG file texture
class Texture : public Texture2D
{        
public:
	/// Loads the texture from disk and loads it to the OpenGL driver
	Texture(Unique id);
};

/// Procedurally generated texture (32x32)
class Smoke : public Texture2D
{
public:
	Smoke();
};

// Dynamic texture used for rendering to
class DynamicTexture : public Texture2D
{
public:
	DynamicTexture(int w, int h, bool black_white = false);
	void copy_to_texture();

	friend void setup_viewport(const DynamicTexture &cm);
protected:
        int width;
        int height;
	int components;
	int format;
	int type;
};	
	
//////////////////////////////////////////////////////////////////////////

// TODO: Implement PNGCubeMap (or similiar)

class DynamicCubeMap : public CubeMap
{
public:
	DynamicCubeMap(int size);
	void copy_to_texture(Side side);

	friend void setup_viewport(const DynamicCubeMap &cm);
};



class CloudParticle : public Texture2D
{
public:
	CloudParticle();
};


// sets/restores viewport which matches the dimensions of the texture
void setup_viewport(int w, int h);
void setup_viewport(const DynamicTexture &t);
void setup_viewport(const DynamicCubeMap &cm);
void restore_viewport();

}
}
}
#endif

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:35 $
 * $Log: texture.h,v $
 * Revision 1.29  2002/12/29 17:04:35  pstrand
 * hw/ moved to os/
 *
 * Revision 1.28  2002/05/21 09:55:25  pstrand
 * clouds..
 *
 * Revision 1.27  2002/05/16 23:59:00  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.26  2002/02/28 12:40:09  fizzgig
 * shadow optimization + better texture statistics
 *
 * Revision 1.25  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.24  2002/02/10 13:58:08  macke
 * cleanup
 *
 * Revision 1.23  2002/02/08 11:27:53  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.22  2002/01/10 23:09:09  macke
 * massa bök
 *
 * Revision 1.21  2001/12/04 23:01:24  picon
 * menu system now working ... barely. :)
 *
 * Revision 1.20  2001/08/06 12:16:16  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.19.4.1  2001/08/03 13:43:54  peter
 * pragma once obsolete...
 *
 * Revision 1.19  2001/07/06 01:47:14  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.18  2001/06/09 01:58:54  macke
 * Grafikmotor reorg
 *
 * Revision 1.17  2001/05/10 11:40:18  macke
 * häpp
 *
 */

