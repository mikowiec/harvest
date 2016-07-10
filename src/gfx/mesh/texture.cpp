#include "os/compat.h"

#include "texture.h"
#include "gfx/settings.h"
#include "os/gl/gl_info.h"

namespace reaper 
{
namespace gfx
{
namespace mesh
{

	
	using std::cout;

	void Texture::use(int n, bool blend)
	{
		n = 0;

		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		//glClientActiveTextureARB(GL_TEXTURE0_ARB + n);

		//glActiveTextureARB(GL_TEXTURE0_ARB + n);

		if(blend){
			float col[4] = {0.7, 0.7, 0.7, 1.0};
			glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &col[0]);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		}

		glBindTexture(GL_TEXTURE_2D, id);
		glEnable(GL_TEXTURE_2D);
	}

	void Texture::stop()
	{
		glDisable(GL_TEXTURE_2D);
	}

	void Texture::setup()
	{
		glGenTextures(1,&id);
		glBindTexture(GL_TEXTURE_2D, id);
	    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 

		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
	    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//float col[4] = {0.5, 0.5, 0.5, 1.0};
		//glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &col[0]);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);
		
	}


	void Texture::load()
	{
		load(filename);
	}

	void Texture::load(char* filename){
		int j = reaper::gfx::misc::load_png(filename, data, w, h);
		
		cout << "Texture::load: texture " << filename << " dimensions " << w << " x " << h << " loaded\n";
		/*
		data = (char*) malloc(sizeof(float) * 16 * 3);
		for(int i = 0; i < 16; i++){
			data[i*3 + 0] = 1.0;
			data[i*3 + 1] = 1.0;
			data[i*3 + 2] = 1.0;
		}
		w = 4;
		h = 4;
		*/
	}

	void Texture::report(){
		cout << "\npng w h : " << w << " / " << h << "\n";
	}


	void TextureMgr::new_tex(char* filename){
		//int v0;
		//glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &v0);
		//cout << "nr of texunits: " << v0 << "\n";
		//if( cnt() + 1 > v0)throw MeshException("TextureMgr: No more Texture Units, GODDMAN!");
		Texture t;
		strcpy(t.filename, filename);
		tex.push_back(t);
	}

	void TextureMgr::prepare()
	{
		for(int i = 0; i < tex.size(); ++i){
			tex[i].load();
			tex[i].setup();
                        reaper::os::gfx::reinit_opengl();
			Settings::current = Settings::read(true);

		}
	}

	void TextureMgr::stop()
	{
		for(int i = 0; i < cnt(); ++i)
			glDisable(GL_TEXTURE0_ARB + i);

		glDisable(GL_TEXTURE_2D);
	}

	void TextureMgr::use(int id, bool blend)
	{
		tex[id].use(id, blend);
	}
/*
	void TextureMgr::use()
	{
		//if(id >= tex.size()) throw MeshException("TextureMgr::use: id out of bound");
		for(int i = 0; i < tex.size(); i++)
			tex[i].use(i, false);
		//tex[id].use();
	}
*/
}
}
}
