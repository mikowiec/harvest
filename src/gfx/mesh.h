#ifndef REAPER_GFX_MESH_H
#define REAPER_GFX_MESH_H

#include <vector>
#include <list>

#include "main/types.h"
#include "gfx/displaylist.h"
#include "gfx/static_geom.h"
#include "gfx/geometry.h"
#include "misc/unique.h"

namespace reaper {
class MtxParams;

namespace gfx {
namespace texture { class Texture; }
namespace lowlevel { class MeshData; }
namespace mesh {

using reaper::gfx::misc::DisplayList;
using reaper::gfx::misc::StaticGeometry;
using std::string;
using std::vector;
using reaper::misc::Unique;

class MeshBase
{
public:
	virtual const string& get_name() const = 0;
	virtual const string& get_texture() const = 0;
	virtual float get_radius() const = 0;
	virtual int get_num_vertices() const = 0;
	virtual int get_num_triangles() const = 0;
	virtual void generate_shadow_volume(const Vector &light, 
	                                    const Matrix &mtx, 
	                                    ShadowVolume &sv) const = 0;

	// NOTE: Parametric and animated objects need more arguments to render properly. 
	virtual void render(int lod) const = 0;
	virtual ~MeshBase() { }
};

class PlainMesh;
class ParametricMesh;

// abomination due to VC6
template <typename T>
class SubObjReader
{
public:
	void read_it(const string &name, T &mesh, std::list<string> &sub_names, Matrix &mtx);
};

class PlainMeshBase : public MeshBase
{
public:
	void apply_material() const { material.use(); }
	const string& get_name() const { return name; }
	const string& get_texture() const { return texture; }
	float get_radius() const { return bnd_sph_radius; }
	int get_num_vertices() const { return n_vertices; }
	int get_num_triangles() const { return n_triangles; }
protected:
	string   name;
        Material material;
	string   texture;
        float    bnd_sph_radius;     
	StaticGeometry render_gmt;
	Geometry geometry;
	int n_vertices;
	int n_triangles;

	friend class SubObjReader<PlainMesh>;
	friend class SubObjReader<ParametricMesh>;
	template<class R>
	friend Matrix partial_reader(R, PlainMeshBase&, lowlevel::MeshData&);
};


/// Holds all info needed for rendering of one object type        
class PlainMesh : public PlainMeshBase
{
public:
        PlainMesh(Unique id);

        void render(int lod) const;
	void generate_shadow_volume(const Vector &light, 
	                            const Matrix &mtx, 
	                            ShadowVolume &sv) const;
private:
	typedef vector<PlainMesh> SubObjCont;

	SubObjCont sub_objs;

        PlainMesh();
	void read(Matrix); ///< Read from file	

	friend class SubObjReader<PlainMesh>;
};


class ParametricMesh : public PlainMeshBase
{
public:
	ParametricMesh(Unique id);

	void render(int lod, const MtxParams &p) const;
	Matrix get_matrix(const std::string &sub_obj, const MtxParams &p) const;

	void generate_shadow_volume(const Vector &light, 
	                            const Matrix &mtx, 
				    ShadowVolume &sv) const {} //FIXME: Implement
private:
	typedef vector<ParametricMesh> SubObjCont;
	typedef SubObjCont::iterator sub_iterator;
	typedef SubObjCont::const_iterator const_sub_iterator;
        
	Matrix mtx;		///< Position matrix for this subobject
        int param;		///< Parametrisation index (-1 for no parametrization)
	SubObjCont sub_objs;	///< Sub-objects
	
	ParametricMesh();
	void read(Matrix);      ///< Read from file	
	void render(int lod) const;

	friend class SubObjReader<ParametricMesh>;
};

class Sheep : public MeshBase
{
public:
	Sheep();

	const string& get_name()    const { return name; }
	const string& get_texture() const { return name; }
	      float   get_radius()  const { return 1.1f; }

	// FIXME: calculate properly for sheep
        int get_num_vertices()  const { return 420; }
        int get_num_triangles() const { return 900; }

        void render(int lod) const;

	void generate_shadow_volume(const Vector &light, 
	                            const Matrix &mtx, 
				    ShadowVolume &sv) const {} //FIXME: Implement
private:
	static const string name;
	DisplayList list;
};

}
}
}
#endif

/*
 * $Author: pstrand $
 * $Date: 2002/09/13 07:45:55 $
 * $Log: mesh.h,v $
 * Revision 1.16  2002/09/13 07:45:55  pstrand
 * gcc-varningar
 *
 * Revision 1.15  2002/05/21 03:07:51  fizzgig
 * envmap update options (gfx_renderer::environment_mapping: 0->off 1->incremental 2->instant)
 * preliminary support for shadow volumes (now requests stencil buffer as well)
 *
 * Revision 1.14  2002/04/30 07:26:41  pstrand
 * friend _class_ and ref.to.tmp.
 *
 * Revision 1.13  2002/04/23 22:50:18  fizzgig
 * mesh-loading cleanup
 *
 * Revision 1.12  2002/03/04 10:22:49  fizzgig
 * sheep!
 *
 * Revision 1.11  2002/02/08 11:27:52  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.10  2002/01/29 04:09:59  peter
 * matrisfippel
 *
 * Revision 1.9  2002/01/23 04:42:47  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.8  2002/01/22 23:44:02  peter
 * reversed last two revs
 *
 * Revision 1.6  2002/01/16 00:25:27  picon
 * terrängbelysning
 *
 * Revision 1.5  2002/01/10 23:09:07  macke
 * massa bök
 *
 * Revision 1.4  2001/08/06 12:16:14  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 *
 */

