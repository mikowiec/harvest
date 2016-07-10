#include "os/compat.h"

#include "res/res.h"
#include "gfx/mesh.h"
#include "gfx/io.h"
#include "os/debug.h"
#include "main/types_ops.h"
#include "misc/parse.h"

#include "res/urr.h"

#include <list>

using namespace std;

namespace reaper {
namespace gfx {
namespace mesh {


namespace {

void pretransform(Matrix &mtx, vector<Point> &points, vector<Vector> &normals)
{
	for(int i = 0; i<points.size(); i++) {
		points[i] = mtx * points[i];
		normals[i] = mtx * normals[i];
		normd(normals[i]);
	}
}

float calc_bound_radius(vector<Point> &points, const Matrix& mtx = Matrix::id())
{
	float r = 0;
	for(int i = 0; i<points.size(); i++) {
		float vr = length(mtx * points[i]);
		if(vr > r)
			r = vr;
	}
	return r;
}

reaper::debug::DebugOutput dout("gfx::MeshFileRead",0);

} // end anonymous namespace

template <typename T>
void SubObjReader<T>::read_it(const string &name, T &mesh, list<string> &sub_names, Matrix &mtx)
{
        for(list<string>::const_iterator i = sub_names.begin(); i != sub_names.end(); ++i) {
		T sub_obj;
		sub_obj.name = name + '_' + *i;
		sub_obj.read(mtx);
		if(sub_obj.bnd_sph_radius > mesh.bnd_sph_radius) {
			mesh.bnd_sph_radius = sub_obj.bnd_sph_radius;
		}
		mesh.n_vertices  += sub_obj.n_vertices;
		mesh.n_triangles += sub_obj.n_triangles;
		mesh.sub_objs.push_back(sub_obj);
	}
}

using namespace reaper::res::urr;

void read_material_data(Reader* r, Material &material)
{
	get_vec(r, "diffuse_color", material.diffuse_color, 4);
	get_vec(r, "ambient", material.ambient_color, 4);
	get_vec(r, "emmision_color", material.emission_color, 4);
	get_vec(r, "specular_color", material.specular_color, 4);
	material.shininess = get<float>(r, "shininess");
	material.ambient_color = Color(1,1,1);
}


template<class R>
Matrix partial_reader(R r, PlainMeshBase& mesh, lowlevel::MeshData& md)
{
	Point pos;
	Quaternion dir;
	Vector scl;
	get_vec(r, "transform", pos, 3);
	get_vec(r, "rotation", dir, 4);
	get_vec(r, "scale", scl, 3);
	Matrix mtx(Matrix(pos) * Matrix(dir) * Matrix(scl.x,scl.y,scl.z));

	read_material_data(r, mesh.material);

	mesh.texture = get<std::string>(r, "texture");

	lowlevel::read_mesh(get<std::string>(r, "mesh"), md);
	mesh.n_vertices = md.triangles.size() * 3;
	mesh.n_triangles = md.triangles.size();
	return mtx;
}

void PlainMesh::read(Matrix parent_mtx)
{
	std::auto_ptr< res::urr::Urr > lvl(res::urr::mk("objects/"+name));
	std::auto_ptr< res::urr::Reader > rd(lvl->reader());

	lowlevel::MeshData md;

	Matrix mtx = partial_reader(rd.get(), *this, md);

	mtx = parent_mtx * mtx;
	pretransform(mtx, md.points, md.normals);
	bnd_sph_radius = calc_bound_radius(md.points);

	render_gmt.init(&md.points[0], 0, &md.normals[0], &md.texcoords[0],
			true, md.points.size(), &md.triangles[0].v1, n_vertices);	
	geometry.init(md.points, md.triangles);

	std::list<std::string> sub_names;
	get_push_vec(rd, "subobjects", sub_names);
	string base = get<std::string>(rd, "basename", name);
	SubObjReader<PlainMesh>().read_it(base, *this, sub_names, mtx);

}

void ParametricMesh::read(Matrix parent_mtx)
{
	std::auto_ptr< res::urr::Urr > lvl(res::urr::mk("objects/"+name));
	std::auto_ptr< res::urr::Reader > rd(lvl->reader());

	lowlevel::MeshData md;

	mtx = partial_reader(rd.get(), *this, md);

	param = get<int>(rd, "parametric");

	if(param == -1) {
		mtx = parent_mtx * mtx;
		pretransform(mtx, md.points, md.normals);
	}
	bnd_sph_radius = calc_bound_radius(md.points, parent_mtx);

	render_gmt.init(&md.points[0], 0, &md.normals[0], &md.texcoords[0],
			true, md.points.size(), &md.triangles[0].v1, n_vertices);	

	std::list<std::string> sub_names;
	get_push_vec(rd, "subobjects", sub_names);
	string base = get<std::string>(rd, "basename", name);
	SubObjReader<ParametricMesh>().read_it(base, *this, sub_names, mtx);
}

}
}
}

