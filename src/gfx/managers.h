#ifndef REAPER_GFX_MANAGERS_H
#define REAPER_GFX_MANAGERS_H

#include "misc/uniqueptr.h"
#include "gfx/gfx_types.h"
#include "misc/unique.h"
#include "main/types.h"

#include <memory>

namespace reaper {
class Point;
class Vector;

namespace world {
class Frustum;
}
namespace gfx {
class Material;
class RenderInfo;
using reaper::misc::Unique;

namespace lowlevel {
class MeshMgrImpl;
class LightMgrImpl;
class VertexMgrImpl;
class TextureMgrImpl;
}
namespace light {
class Light;
}
namespace mesh {
class ShadowVolume;
}

class MeshMgr;
class LightMgr;
class VertexMgr;
class TextureMgr;

typedef reaper::misc::UniquePtr<MeshMgr>    MeshRef;
typedef reaper::misc::UniquePtr<LightMgr>   LightRef;
typedef reaper::misc::UniquePtr<VertexMgr>  VertexRef;
typedef reaper::misc::UniquePtr<TextureMgr> TextureRef;

class LightMgr
{
public:	
//	static LightRef create();
//	static LightRef create(const std::string &file);
	static LightRef get_ref();
	
	void setup_global();                               ///< Call each frame	
	void setup_locals(const Point& pos, float radius); ///< Call for each object	
	void render(const world::Frustum &f);              ///< Render lightmaps on terrain

	void add_global(light::Light *l);
	void add_static(light::Light *l);
	void add_dynamic(light::Light *l);

	void remove_global(light::Light *l);
	void remove_static(light::Light *l);
	void remove_dynamic(light::Light *l);

	Color calc_diffuse_light(const Point &p, const Vector &n, const Material &m);

	int num_static_lights();

	~LightMgr(); 
private:
	LightMgr();
	std::auto_ptr<lowlevel::LightMgrImpl> i;

	friend class reaper::misc::UniquePtr<LightMgr>;
};

class TextureMgr
{
public:
//	static TextureRef create();	
	static TextureRef get_ref();

	void use(Unique id, int level = 2);  /// Binds the texture to use for rendering	
	void load(Unique id, int level = 2); /// Loads the texture from disk, does not bind

	int count() const;         /// Returns the number of loaded objects	
	void purge();              /// Frees all loaded objects

	~TextureMgr();
private:
	TextureMgr();
	std::auto_ptr<lowlevel::TextureMgrImpl> i;

	friend class reaper::misc::UniquePtr<TextureMgr>;
};

class MeshMgr
{
public:
//	static MeshRef create();
	static MeshRef get_ref();	

	void   load(Unique id);
	void   render(const RenderInfo &id, int lod);
	Matrix get_matrix(const RenderInfo &id, const std::string &sub_obj);
	Unique get_texture(const RenderInfo &id);
	float  get_radius(Unique id);
	void   generate_shadow_volume(Unique id,
	                    const Vector &light, 
	                    const Matrix &mtx, 
			    mesh::ShadowVolume &sv);

	int count() const; ///< Returns the number of loaded objects	
	void purge();      ///< Frees all loaded objects

	int get_num_vertices();  ///< returns vertices rendered since last reset
	int get_num_triangles(); ///< returns triangles rendered since last reset
	void reset_stats();      ///< resets triangle & vertex count to zero

	~MeshMgr();
private:
	MeshMgr();
	std::auto_ptr<lowlevel::MeshMgrImpl> i;

	friend class reaper::misc::UniquePtr<MeshMgr>;
};

class VertexMgr
{
public:
	//static VertexRef create();
	static VertexRef get_ref();

	void* malloc(size_t size);
	void* realloc(void *ptr, size_t size, bool keep_data);
	void  free(void*);
	size_t size(void*);

	bool writable(void*);
	void wait(void*);
	void clear();

	size_t total_size(); 

	void glDrawArrays(const void *ptr, int mode, int first, int count);
	void glDrawElements(const void *ptr, int mode, int count, int type, const void *indices, int size);
	void glDrawRangeElements(const void *ptr, int mode, int start, int end, int count, int type, const void *indices, int size);

	~VertexMgr();
private:
	VertexMgr();
	std::auto_ptr<lowlevel::VertexMgrImpl> i;

	friend class reaper::misc::UniquePtr<VertexMgr>;
};

}
}

#endif
/*
 * $Author: fizzgig $
 * $Date: 2003/05/20 10:48:09 $
 * $Log: managers.h,v $
 * Revision 1.22  2003/05/20 10:48:09  fizzgig
 * added includes
 *
 * Revision 1.21  2003/01/11 23:58:43  fizzgig
 * improved opengl implementation reporting
 *
 * Revision 1.20  2002/09/20 23:46:59  pstrand
 * use_texturing->texture_level, minor event fix.
 *
 * Revision 1.19  2002/05/21 03:07:51  fizzgig
 * envmap update options (gfx_renderer::environment_mapping: 0->off 1->incremental 2->instant)
 * preliminary support for shadow volumes (now requests stencil buffer as well)
 *
 * Revision 1.18  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.17  2002/02/28 12:40:09  fizzgig
 * shadow optimization + better texture statistics
 *
 * Revision 1.16  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.15  2002/02/08 11:27:52  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.14  2002/01/27 19:46:03  peter
 * *** empty log message ***
 *
 * Revision 1.13  2002/01/23 04:42:47  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.12  2002/01/22 23:44:01  peter
 * reversed last two revs
 *
 * Revision 1.10  2002/01/16 00:25:26  picon
 * terrängbelysning
 *
 * Revision 1.9  2002/01/11 14:28:07  macke
 * no message
 *
 * Revision 1.8  2002/01/10 23:09:07  macke
 * massa bök
 *
 * Revision 1.7  2001/08/06 12:16:14  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 *
 */

