#include "os/compat.h"
#include <sstream>
#include "os/gl.h"
#include "os/debug.h"
#include "gfx/mesh.h"
#include "gfx/exceptions.h"
#include "main/types_param.h"
#include "main/types_ops.h"

namespace reaper {
namespace gfx {
namespace mesh {
namespace {

void throw_sub_obj_error(const std::string &id,const std::string &name)
{
	std::ostringstream ss;
	ss << "ParametricMesh::get_matrix() Unable to locate subobject " << id;
	ss << " in object " << name << '\n';

	throw gfx_fatal_error(ss.str());
}

inline const std::string get_next_id(const std::string &id,const std::string &name)
{
	return id.substr(0,id.find_first_of('_',name.size()+1));
}

reaper::debug::DebugOutput dout("gfx::MeshFile",0);

} // end anonymous namespace

PlainMesh::PlainMesh()
{}

PlainMesh::PlainMesh(Unique id)
{
        try {
		name = id.get_text();
		read(Matrix::id());
        }
        catch(gfx_fatal_error &e) {
                throw gfx_fatal_error(string(e.what()) + " reading object: " + id.get_text());
        }
}


void PlainMesh::render(int lod) const
{
	material.use();
	render_gmt.render();
        for(SubObjCont::const_iterator i = sub_objs.begin();i != sub_objs.end(); ++i) {
                i->render(lod);
	}
}	

void PlainMesh::generate_shadow_volume(const Vector &light, 
	                    const Matrix &mtx, 
	                    ShadowVolume &sv) const
{
	geometry.generate_shadow_volume(light, mtx, sv);
        for(SubObjCont::const_iterator i = sub_objs.begin();i != sub_objs.end(); ++i) {
                i->generate_shadow_volume(light, mtx, sv);
	}
}

ParametricMesh::ParametricMesh() 
: mtx(Matrix::id()), param(-1) 
{} 

ParametricMesh::ParametricMesh(Unique id)
{
        try {
		mtx = Matrix::id();
		param = -1;
		name = id.get_text();
		read(mtx);
        }
        catch(gfx_fatal_error &e) {
                throw gfx_fatal_error(string(e.what()) + " reading object: " + name);
        }
}

Matrix ParametricMesh::get_matrix(const std::string &sub_obj, const MtxParams &p) const
{
	if(sub_obj == name)
		return param == -1 ? mtx : mtx * p[param];

	const string subobj_id = get_next_id(sub_obj,name);

	for(const_sub_iterator i = sub_objs.begin(); i != sub_objs.end(); ++i) {
		if(subobj_id == i->name) {
			const Matrix &sub_mtx = i->get_matrix(sub_obj,p);
			return mtx * (param == -1 ? sub_mtx : p[param] * sub_mtx);
		}
	}

	throw_sub_obj_error(sub_obj,name);
	return Matrix();
}

void ParametricMesh::render(int lod, const MtxParams &p) const
{
	if (param != -1) {
		glMultMatrix(mtx * p[param]);
	}
	material.use();
	render_gmt.render();
	for(const_sub_iterator i = sub_objs.begin();i != sub_objs.end(); ++i) {
		i->render(lod,p);
	}
}

void ParametricMesh::render(int lod) const
{
	throw gfx_fatal_error(string("ParametricMesh::render(): Object ") + name + " needs parameters!");
}

}
}
}
