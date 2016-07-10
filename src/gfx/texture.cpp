#include "os/compat.h"

#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <stack>

#include "main/types.h"
#include "gfx/misc.h"
#include "gfx/texture.h"
#include "gfx/settings.h"
#include "gfx/exceptions.h"
#include "os/gl.h"
#include "os/debug.h"
#include "res/res.h"
#include "misc/utility.h"

namespace reaper {
namespace gfx {
namespace texture {
namespace {

reaper::debug::DebugOutput dout("gfx::texture");

GLenum to_compressed(GLenum format)
{
	if(!Settings::current.use_arb_texture_compression) {
		return format;
	} else if(Settings::current.use_ext_s3tc_texture_compression) {
		switch(format) {
		case GL_ALPHA:           return GL_COMPRESSED_ALPHA_ARB;
		case GL_LUMINANCE:       return GL_COMPRESSED_LUMINANCE_ARB;
		case GL_LUMINANCE_ALPHA: return GL_COMPRESSED_LUMINANCE_ALPHA_ARB;
		case GL_INTENSITY:       return GL_COMPRESSED_INTENSITY_ARB;
		case GL_RGB:             return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		case GL_RGBA:            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		} 
	} else {
		switch(format) {
		case GL_ALPHA:           return GL_COMPRESSED_ALPHA_ARB;
		case GL_LUMINANCE:       return GL_COMPRESSED_LUMINANCE_ARB;
		case GL_LUMINANCE_ALPHA: return GL_COMPRESSED_LUMINANCE_ALPHA_ARB;
		case GL_INTENSITY:       return GL_COMPRESSED_INTENSITY_ARB;
		case GL_RGB:             return GL_COMPRESSED_RGB_ARB;
		case GL_RGBA:            return GL_COMPRESSED_RGBA_ARB;
		}
	}
	throw reaper::gfx::gfx_fatal_error("texture::to_compressed() - unknown format");
}

template<typename T>
int calc_size(int components) 
{
	int size = 0;
	int lod = 0;
	int width = 0;
	int height = 0;

	if(Settings::current.use_arb_texture_compression && 
	   glGetTexLevelParameter2D(0,GL_TEXTURE_COMPRESSED_ARB)) {				
		while (true) { 
			width  = glGetTexLevelParameter2D(lod,GL_TEXTURE_WIDTH);
			height = glGetTexLevelParameter2D(lod,GL_TEXTURE_HEIGHT);					

			if (width == 0 || height == 0)

				break;
			size  += glGetTexLevelParameter2D(lod,GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB);
			lod++;
		}
	} else {
		// FIXME: get actual size in some better way
		do {
			width  = glGetTexLevelParameter2D(lod,GL_TEXTURE_WIDTH);
			height = glGetTexLevelParameter2D(lod,GL_TEXTURE_HEIGHT);					
			size += width*height;
			lod++;
		} while (width>1 && height>1);
		
		size *= components;
	}
	return size;
}
using std::max;

int calc_lod(int w, int h, int scale_factor) 
{
	int begin_lod = 0;

	//FIXME: replace by proper log2 algorithm
	// and read some modifications to this table
	// from file, so that system can be more finely tuned
	switch(scale_factor) {
	case 1:		begin_lod = 0; break;
	case 2:		begin_lod = 1; break;
	case 4:		begin_lod = 2; break;
	case 8:		begin_lod = 3; break;
	case 16:	begin_lod = 4; break;
	default:	break;
	}	

	int min_lod = 0;
	int min_size = std::min(w, h);
	if(min_size > 64)
		min_lod = 1;
	if(min_size > 128)
		min_lod = 2;
	if(min_size > 256)
		min_lod = 3;
	if(min_size > 512)
		min_lod = 4;

	return std::min(min_lod, begin_lod);
};

// Rescale texture if size is too big
template<typename T>
void scale_texture(int c, int &w, int &h, GLenum format, GLenum type, T *& data)
{
	int s = Settings::current.texture_scaling;
	int s_tex = 1 << calc_lod(w, h, s);
	int s_hw = max(w,h) / glGetInteger(GL_MAX_TEXTURE_SIZE);
	int scale_factor = max(max(s, s_tex), s_hw);	
	
	if(scale_factor > 1) {
		int new_width  = max(1,w / scale_factor);
		int new_height = max(1,h / scale_factor);

		// data is malloc:ed by pnglib, so we use the same for new_data
		T *new_data = static_cast<T*>(malloc(new_height * new_width * c * sizeof(T))); 

		gluScaleImage(format, w, h, type, data, new_height, new_width, type, new_data);

		free(data);
		data = new_data;			
		w = new_width;
		h = new_height;
	}

	throw_on_gl_error("texture::scale_texture()");
}

template<typename T>
int init(int c, int &w, int &h, GLenum mag_filter, GLenum min_filter, 
	 GLenum format, GLenum type, T *& data, bool texcmpr)
{
	GLenum internal_format = texcmpr ? to_compressed(format) : format;

	scale_texture(c, w, h, format, type, data);

	if(min_filter == GL_NEAREST || min_filter == GL_LINEAR) {
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, type, data);
	} else {
		if (Settings::current.use_sgis_generate_mipmap) {
			glTexParameter2D(GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, type, data);
		} else {
			gluBuild2DMipmaps(GL_TEXTURE_2D, internal_format, w, h, format, type, data);			
		}
	}

        glTexParameter2D(GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameter2D(GL_TEXTURE_MIN_FILTER, min_filter);       

        throw_on_gl_error("texture::init()");	

	return calc_size<T>(c);
}

int get_format(int c)
{
	int format;
	switch(c) {
	case 1: format = GL_LUMINANCE; break;
	case 3: format = GL_RGB;  break;
	case 4: format = GL_RGBA; break;
	default: throw reaper::gfx::gfx_fatal_error("Texture(): Unsupported texture format.");
	}
	return format;
}

int load_s3tc(const std::string &file)
{
	using reaper::misc::read_binary;

	reaper::gfx::throw_on_gl_error("texture::load_s3tc() - entering");

	res::res_stream is(res::Cache, file + ".s3tc", res::throw_on_error);
	int w, h, c, format, internal_format, size;

	read_binary(is, w);
	read_binary(is, h);
	read_binary(is, c);
	read_binary(is, format);
	read_binary(is, internal_format);
	read_binary(is, size);

	char *data   = (char*)malloc(w*h*c);
	int lod      = 0;
	int tot_size = size;

	int begin_lod = calc_lod(w, h, Settings::current.texture_scaling);

	while(size != 0) {
		is.read(data, size);
		if(lod >= begin_lod) {
			int actual_lod = lod - begin_lod;
			glCompressedTexImage2DARB(GL_TEXTURE_2D, actual_lod, internal_format, w, h, 0, size, data );
		}
		read_binary(is,size);
		if(lod >= begin_lod) {
			tot_size += size;
		}
		w = max(1,w>>1);
		h = max(1,h>>1);
		lod++;
	}
	
	free(data);
	reaper::gfx::throw_on_gl_error("texture::load_s3tc()");

	return tot_size;
}

int generate_save_s3tc(const std::string &file)
{
	using reaper::misc::write_binary;

	char *data = 0;
	int w, h;
	int c = reaper::gfx::misc::load_png(file,data,w,h);
	int format = get_format(c);
	scale_texture(c,w,h,format,GL_UNSIGNED_BYTE,data);
	int internal_format = to_compressed(format);

	res::res_out_stream os(res::Cache, file + ".s3tc");
	write_binary(os,w);
	write_binary(os,h);
	write_binary(os,c);
	write_binary(os,format);
	write_binary(os,internal_format);

	int new_w    = w;
	int new_h    = h;
	int lod      = 0;
	int cur_size = w*h*c;
	int tot_size = 0;
	char *tmp    = (char*)malloc(cur_size);

	do {
		w = new_w;
		h = new_h;

		new_w = max(1,w>>1);
		new_h = max(1,h>>1);

		glTexImage2D(GL_TEXTURE_2D, lod, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, data);

		gluScaleImage(format, w, h, GL_UNSIGNED_BYTE, data,
			      new_w, new_h, GL_UNSIGNED_BYTE, tmp);

		int compressed = glGetTexLevelParameter2D(lod, GL_TEXTURE_COMPRESSED_ARB);
		if(!compressed) {
			throw reaper::gfx::gfx_fatal_error("texture compression error...");
		}

		int size = glGetTexLevelParameter2D(lod, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB);
		tot_size += size;
		if(size > cur_size) {
			data = (char*)realloc(data, size);
			cur_size = size;
		}
		glGetCompressedTexImageARB(GL_TEXTURE_2D, lod, data);

		write_binary(os, size);
		os.write(data,size);

		std::swap(data,tmp);
		lod++;
	} while( !(w==1 && h==1));
	free(data);
	free(tmp);

	write_binary(os,(int) 0);

	dout << "cached texture " << file << ".png to /data/cache/" << file << ".s3tc - " << tot_size << " byte texel data\n";
	reaper::gfx::throw_on_gl_error("texture::generate_save_s3tc()");

	return tot_size;
}	
} // end anonymous namespace

TextureBase::TexCont TextureBase::textures;

TextureBase::TextureBase() : 
	i(0,0)
{ 
	glGenTextures(1,&i.name); 
	textures.insert(&i); 
}

TextureBase::~TextureBase() 
{
	textures.erase(&i);
	glDeleteTextures(1,&i.name); 
}

	
void TextureBase::set_size(unsigned int s) 
{ 
	i.size = s; 
}

unsigned int TextureBase::all_textures_size()
{
	int size = 0;
	for(TexCont::const_iterator i = textures.begin(); i != textures.end(); ++i) {
		size += (*i)->size;
	}
	return size;
}

unsigned int TextureBase::resident_textures_size()
{
	std::vector<GLboolean> residences(textures.size());
	std::vector<unsigned int> texnames;
	texnames.reserve(textures.size());
	int size = 0;
	int cnt = 0;

	for(TexCont::const_iterator i = textures.begin(); i != textures.end(); ++i) {
		texnames.push_back((*i)->name);
	}
		
	glAreTexturesResident(textures.size(),&texnames[0],&residences[0]);

	for(TexCont::const_iterator i = textures.begin(); i != textures.end(); ++i) {
		size += residences[cnt++] ? (*i)->size : 0;
	}

	return size;	
}


Texture::Texture(Unique id)
{
	use();

	//FIXME: Replace this hack by a proper texture config file
	bool texcmpr = true;
	const std::string &file = id.get_text();
	if(file == "main" ||
	   file.substr(0,4) == "menu" ||
	   file == "engine_wake" ||
	   file.substr(0,3) == "sky") {				
		texcmpr = false;
	}

	int size = -1;
	if(texcmpr &&
	   Settings::current.use_ext_s3tc_texture_compression &&
	   Settings::current.use_arb_texture_compression &&
	   Settings::current.texture_caching) {
		try {
			size = load_s3tc(file);
		} 
		catch ( res::resource_not_found ) {
			size = generate_save_s3tc(file);
		}
		glTexParameter2D(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameter2D(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
		char *data = 0;
		try {          
			int c = reaper::gfx::misc::load_png(file,data,w,h);
			int format = get_format(c);
			size = init(c, w, h, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, format, GL_UNSIGNED_BYTE, data, texcmpr);
			free(data);
		}
		catch(gfx_fatal_error &) {		
			free(data);
			data = 0;
			throw;
		}
	}

	set_size(size);

	//FIXME: Replace this hack by a proper texture config file
	if(file == "fire_particle" || file == "hud_target_area") {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
}

Smoke::Smoke()
{
	int s = 32;		
	const float radius = s/2;

	float* data = static_cast<float*>(malloc(s*s*2*sizeof(float)));
	float* ptr = data;

	for(int y=1;y<=s;y++) {
		const float y_dist = radius-y;
		for(int x=1;x<=s;x++) {
			const float x_dist = radius-x;
			*ptr++ = std::max<float>(0, 1 - sqrt(x_dist*x_dist + y_dist*y_dist) / (radius - 1));
		}
	}
	
	use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	int size = init(2, s, s, GL_LINEAR, GL_LINEAR, GL_ALPHA, GL_FLOAT, data, true);
	set_size(size);
	free(data);

        throw_on_gl_error("Smoke::init(): ");
}

CloudParticle::CloudParticle()
{
	int s = 16;
	const float radius = s/2;

	float* data = static_cast<float*>(malloc(s*s*2*sizeof(float)));
	float* ptr = data;

	for (int y = 1; y <= s; y++) {
		const float y_dist = radius-y;
		for (int x = 1; x <= s; x++) {
			const float x_dist = radius-x;
			float v = std::max<float>(0, 0.8 - sqrt(x_dist*x_dist + y_dist*y_dist) / (radius - 1));
			v /= 2.0;
			*ptr++ = v;
			*ptr++ = v;
		}
	}
	
	use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	int size = init(2, s, s, GL_LINEAR, GL_LINEAR, GL_LUMINANCE_ALPHA, GL_FLOAT, data, true);
	set_size(size);
	free(data);

        throw_on_gl_error("Smoke::init(): ");
}


}
}
}

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:34 $
 * $Log: texture.cpp,v $
 * Revision 1.69  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.68  2002/05/29 00:35:12  fizzgig
 * cleanup, small fixes, improvement, etc
 *
 * Revision 1.67  2002/05/24 13:04:55  pstrand
 * *** empty log message ***
 *
 * Revision 1.66  2002/05/22 22:15:29  fizzgig
 * use SGIS_generate_mipmap instead of gluBuildMipmaps2D if available
 *
 * Revision 1.65  2002/05/22 17:33:57  pstrand
 * glGetTexLevelParameter2D(lod,GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB) seems to have problems when width&height is zero.. ??
 *
 * Revision 1.64  2002/05/21 09:55:25  pstrand
 * clouds..
 *
 * Revision 1.63  2002/05/19 17:36:49  fizzgig
 * glh-lib added (for extensions detect & init)
 * nvparse added
 * extgen added
 * texture blending example added
 *
 * Revision 1.62  2002/05/16 23:59:00  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.61  2002/05/10 14:33:28  fizzgig
 * fixed incorrect handling of max texture size
 *
 * Revision 1.60  2002/05/06 19:49:55  fizzgig
 * changed to glPop/PushAttrib() for scissor/viewport handling in DynamicTexture
 *
 * Revision 1.59  2002/04/11 01:03:34  pstrand
 * explicit res_stream exception...
 *
 * Revision 1.58  2002/02/28 12:40:09  fizzgig
 * shadow optimization + better texture statistics
 *
 * Revision 1.57  2002/02/26 22:41:17  pstrand
 * mackefix
 */
