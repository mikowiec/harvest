/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:34 $
 * $Log: mesh_mgr.cpp,v $
 * Revision 1.9  2002/12/29 17:04:34  pstrand
 * hw/ moved to os/
 *
 * Revision 1.8  2002/05/21 03:07:51  fizzgig
 * envmap update options (gfx_renderer::environment_mapping: 0->off 1->incremental 2->instant)
 * preliminary support for shadow volumes (now requests stencil buffer as well)
 *
 * Revision 1.7  2002/04/14 19:27:06  pstrand
 * try again... (uniqueptr)
 *
 * Revision 1.6  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.5  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.4  2002/02/08 11:27:52  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.3  2001/07/31 23:19:15  macke
 * Lite mer benchmarking i grafikmotorn...
 *
 * Revision 1.2  2001/07/30 23:43:17  macke
 * Häpp, då var det kört.
 *
 * Revision 1.1  2001/07/23 23:49:36  macke
 * Namnbyte...
 *
 * Revision 1.2  2001/07/06 01:47:12  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.1  2001/06/09 01:58:50  macke
 * Grafikmotor reorg
 *
 * Revision 1.15  2001/05/14 20:00:52  macke
 * bugfix och rök på missiler..
 *
 * Revision 1.14  2001/05/10 11:40:14  macke
 * häpp
 *
 */

#include "os/compat.h"
#include "main/types.h"

#include <vector>

#include "gfx/managers.h"
#include "gfx/mesh.h"
#include "gfx/exceptions.h"
#include "gfx/abstracts.h"
#include "os/debug.h"
#include "res/res.h"
#include "misc/map.h"
#include "misc/lua.h"
#include "misc/creator.h"

namespace reaper {
namespace misc { UniquePtr<gfx::MeshMgr>::I UniquePtr<gfx::MeshMgr>::inst; }
namespace gfx {
namespace {
debug::DebugOutput dout("gfx::object::Manager",0);

inline std::string get_base(const std::string &s) 
{
	return s.substr(0,s.find_first_of('_'));
}
} // end anonymous namespace

namespace lowlevel {

using reaper::misc::Creator;
using namespace reaper::gfx::mesh;

class MeshMgrImpl
{
	enum ObjectType
	{
		Unknown,
		Standard,
		Parametric,
		Coded
	};

	typedef reaper::misc::Map<Unique,MeshBase>       MeshManager;
	typedef reaper::misc::Map<Unique,ParametricMesh> ParMeshManager;
	typedef std::map<Unique, reaper::misc::CreateBase<MeshBase>*> MeshCreators;

	MeshManager    mesh_manager;
	ParMeshManager parmesh_manager;
	MeshCreators   procedural_meshes;

	TextureRef tr;			

	int vertices;
	int triangles;

	const MeshBase& get_obj(Unique id); 
	const MeshBase& get_any_obj(Unique id); // gets both plain & param

	bool is_code_object(Unique id) const;
	ObjectType get_obj_type(Unique id);

public:
	MeshMgrImpl();
	~MeshMgrImpl();
	
	void   load(Unique id);
	void   render(const RenderInfo &ri, int lod);
	Matrix get_matrix(const RenderInfo &ri, const std::string &sub_obj);
	Unique get_texture(const RenderInfo &ri);
	float  get_radius(Unique id);
	void   generate_shadow_volume(Unique id,
	                    const Vector &light, 
	                    const Matrix &mtx, 
	                    mesh::ShadowVolume &sv);

	int count() const;
	void purge();

	int get_num_vertices() const { return vertices; }
	int get_num_triangles() const { return triangles; }
	void reset_stats() { vertices = 0; triangles = 0; }
};

MeshMgrImpl::MeshMgrImpl() : 
	mesh_manager("gfx::Mesh"), 
	parmesh_manager("gfx::ParametricMesh"), 
	vertices(0),
	triangles(0)
{
	procedural_meshes["sheep"] = new Creator<MeshBase,Sheep>();
}

MeshMgrImpl::~MeshMgrImpl()
{
	for(MeshCreators::iterator i = procedural_meshes.begin(); i != procedural_meshes.end(); ++i)
		delete i->second;

	throw_on_gl_error("object::MeshMgrImpl::~MeshMgrImpl()");
}

inline const MeshBase& MeshMgrImpl::get_obj(Unique id)
{
	// If object is loaded, return it
	// If not, check if it's a special creator object
	if(mesh_manager.is_loaded(id)) {
		return *mesh_manager.get_if(id);
	} else if(is_code_object(id)) {
		return mesh_manager.insert(procedural_meshes[id.get_text()]->create(),id);
	} else {
		return mesh_manager.insert(new PlainMesh(id.get_text()),id);
	}		
}

inline const MeshBase& MeshMgrImpl::get_any_obj(Unique id)
{
	if(mesh_manager.is_loaded(id)) {
		return *mesh_manager.get_if(id);
	} else if(parmesh_manager.is_loaded(id)) {
		return parmesh_manager.get(id);
	} else if(is_code_object(id) || get_obj_type(id) == Standard) {
		return get_obj(id);
	} else {
		return parmesh_manager.get(id);
	}
}


inline bool MeshMgrImpl::is_code_object(Unique id) const
{
	return procedural_meshes.find(id) != procedural_meshes.end();
}

using namespace reaper::misc::lua;

inline MeshMgrImpl::ObjectType MeshMgrImpl::get_obj_type(Unique id)
{
	// FIXME: also ugly, but in a different way ;)
	/* fix when the generic resource tree is implemented, 
	 * i.e.: if (res["objects"][id.get_text()]["parametric"])
	 * 		...
	 * 	 else
	 * 	        ...
	 */
	try {
		Lua lua;
		Table obj = load_table(lua, "objects/"+id.get_text());

		if (obj.lookup("parametric").type() != type::Nil)
			return Parametric;
		if (obj.lookup("lod").type() != type::Nil)
			return Standard;
	} catch (res::resource_not_found) {
		dout << id.get_text() << " not found\n";
	}
	return Unknown;
}


inline void MeshMgrImpl::render(const RenderInfo &ri, int lod)
{
	const MeshBase *mesh = 0;
	if(ri.param) {
		mesh = &parmesh_manager.get(ri.mesh);
		tr->use(mesh->get_texture());
		dynamic_cast<const ParametricMesh*>(mesh)->render(lod, *ri.param);
	} else {
		mesh = &get_obj(ri.mesh); 
		tr->use(mesh->get_texture());
		mesh->render(lod);
	}

	triangles += mesh->get_num_triangles();
	vertices  += mesh->get_num_vertices();
}

inline void MeshMgrImpl::load(Unique id)
{
	get_any_obj(id);
}

inline float MeshMgrImpl::get_radius(Unique id)
{
	return get_any_obj(id).get_radius();
}

inline Unique MeshMgrImpl::get_texture(const RenderInfo &ri) 
{
	return get_any_obj(ri.mesh).get_texture();
}

inline Matrix MeshMgrImpl::get_matrix(const RenderInfo &ri, const std::string &sub_obj)
{
	if(ri.param) {
		return parmesh_manager.get(ri.mesh).get_matrix(sub_obj, *ri.param);
	} else {
		string error = "MeshMgr: get_matrix not supported for non-parametric mesh: ";
		error += ri.mesh.get_text();
		throw gfx_fatal_error(error);
	}
}
void MeshMgrImpl::generate_shadow_volume(Unique id, const Vector &light, 
					 const Matrix &mtx, mesh::ShadowVolume &sv)
{
	get_any_obj(id).generate_shadow_volume(light, mtx, sv);
}

inline int MeshMgrImpl::count() const
{
	return mesh_manager.size() + parmesh_manager.size();
}

inline void MeshMgrImpl::purge()
{
	mesh_manager.purge();
	parmesh_manager.purge();
}
} // end lowlevel namespace

MeshMgr::MeshMgr() : i(new lowlevel::MeshMgrImpl) {}
MeshMgr::~MeshMgr() { }

/*
MeshRef MeshMgr::create()  {
	MeshRef r;
	if (!r.valid())
		MeshRef::create(new MeshMgr());
	return r;
}
*/

MeshRef MeshMgr::get_ref() { return MeshRef(); }

void   MeshMgr::render(const RenderInfo &id, int lod) { 
	i->render(id, lod);
}
void   MeshMgr::load(Unique id) {
	i->load(id);
}
float  MeshMgr::get_radius(Unique id) { 
	return i->get_radius(id); 
}
Unique MeshMgr::get_texture(const RenderInfo &id) {
	return i->get_texture(id);
}
Matrix MeshMgr::get_matrix(const RenderInfo &id, const std::string &sub_obj) {
	return i->get_matrix(id, sub_obj);
}
void MeshMgr::generate_shadow_volume(Unique id,
	                    const Vector &light, 
	                    const Matrix &mtx, 
			    mesh::ShadowVolume &sv) {
	i->generate_shadow_volume(id, light, mtx, sv);
}

int MeshMgr::count() const {
	return i->count();
}
void MeshMgr::purge() {
	i->purge();
}

int MeshMgr::get_num_triangles() {
	return i->get_num_triangles();
}

int MeshMgr::get_num_vertices() {
	return i->get_num_vertices();
}

void MeshMgr::reset_stats() {
	i->reset_stats();
}

}
}
