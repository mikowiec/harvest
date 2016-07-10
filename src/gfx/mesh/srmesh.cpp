#include "os/compat.h"

#include "srmesh.h"
#include "shared.h"
#include <iostream>
#include <fstream>

#include "main/types_io.h"
#include "main/types_ops.h"

namespace reaper 
{
namespace gfx
{
namespace mesh
{

using namespace std;

SRMesh::SRMesh()
{
    vertices.clear();
    faces.clear();
    active_vertices.clear();
    active_faces.clear();
    tex.tex.clear();
}

template<class T>
void bin_write(ostream &os, const T &t) { os.write( (char*) &t, sizeof(T)); }

template<class T>
void bin_read(istream &is, T &t) { is.read( (char*) &t, sizeof(T)); }


int SRMesh::write_file(char* file_name, bool replace)
{

    int vertice_count = vertices.size();
    int face_count = faces.size();
    int tex_count = tex.cnt();

    ofstream outFile;

    if(replace)
        outFile.open(file_name, ios::out | ios::trunc | ios::binary );
    else
        outFile.open(file_name, ios::out | ios::binary);

    if(!outFile) throw MeshException("SRMesh::write_file: cannot open file");

    bin_write(outFile, tex_count);
    bin_write(outFile, vertice_count);
    bin_write(outFile, face_count);

    // Write textures
    for(int ti = 0; ti < tex_count; ti++)
        outFile.write( (char*)& tex.tex[ti], sizeof (Texture));

    // Write vertices
    for(int vi = 0; vi < vertices.size(); vi++){
        const PMVertex &v = vertices[vi];

        bin_write(outFile, v.point);
        bin_write(outFile, v.color);
        bin_write(outFile, v.normal);

        for(int i = 0; i < tex_count; i++) {
            bin_write(outFile, v.tc[i]);
        }

        bin_write(outFile, v.is_active);
        bin_write(outFile, v.parent);
        bin_write(outFile, v.vt);
        bin_write(outFile, v.vu);
        bin_write(outFile, v.flr);
        bin_write(outFile, v.fn);
        bin_write(outFile, v.fni);
        bin_write(outFile, v.flr_area);
        bin_write(outFile, v.refine_info.bsr);
    }

    // Write faces
    for(int fi = 0; fi < faces.size(); fi++) {
        bin_write(outFile, faces[fi]);
    }

    outFile.close();

    return 0;
}


void SRMesh::read_file(char* file_name)
{
    int vi, fi, ti;

    PMFace* pmf = new PMFace;
    int tex_count, vertice_count, face_count;
    Texture tmp_tex;
    tex.clear();
    ifstream readFile(file_name, ios::in | ios::binary);

    if(!readFile) throw MeshException("SRMesh::read_file: cannot open file");

    bin_read(readFile, tex_count);
    bin_read(readFile, vertice_count);
    bin_read(readFile, face_count);

    // Read textures
    for(ti = 0; ti < tex_count; ti++){
        readFile.read ((char*) &tmp_tex, sizeof (Texture));
        tmp_tex.data = 0;
        tex.tex.push_back(tmp_tex);
    }

    if(!readFile.good()) throw MeshException("SRMEsh::readfile: read error 1");

    // Read vertices
    vertices.resize(vertice_count);
    for(vi = 0; vi < vertice_count; vi++){

        PMVertex v;
        v.tc.resize(tex_count);

        bin_read(readFile, v.point);
        bin_read(readFile, v.color);
        bin_read(readFile, v.normal);

        for(int i = 0; i < tex_count; i++) {
            bin_read(readFile, v.tc[i]);
        }

        bin_read(readFile, v.is_active);
        bin_read(readFile, v.parent);
        bin_read(readFile, v.vt);
        bin_read(readFile, v.vu);
        bin_read(readFile, v.flr);
        bin_read(readFile, v.fn);
        bin_read(readFile, v.fni);
        bin_read(readFile, v.flr_area);
        bin_read(readFile, v.refine_info.bsr);
        v.active = 0;

        vertices[vi] = v;
    }

    if(!readFile.good()) throw MeshException("SRMEsh::readfile: read error 2");

    // Read faces
    faces.resize(face_count);
    for(fi = 0; fi < face_count; fi++){
        bin_read(readFile, *pmf);
        pmf->active = 0;
        faces[fi] = *pmf;
    }

    if(!readFile.good()) throw MeshException("SRMEsh::readfile: read error 3");

    readFile.close();

    // Setup active elements
    for(vi = 0; vi < vertice_count; vi++)
        if(vertices[vi].is_active)
            add_active_vertex(vi);

    for(fi = 0; fi < face_count; fi++)
        if(faces[fi].is_active)
            add_active_face(fi);
}

void SRMesh::load(char* file_name)
{
    // Reset the mesh
    vertices.clear();
    faces.clear();
    active_vertices.clear();
    active_faces.clear();

    // Read the stored mesh
    read_file(file_name);
}

void SRMesh::save(char* file_name)
{
    write_file(file_name, true);
}


void SRMesh::render()
{
    render(NULL);
}


void SRMesh::render(PMVertex* pmv)
{
    if(active_faces.size() == 0)
        return;

    lint::iterator li;
    int x, y, z, i, j;
    Point p[3];

    float colorBronzeSpec[4] = { 1.0, 1.0, 0.4, 1.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, colorBronzeSpec);
    glMaterialf(GL_FRONT, GL_SHININESS, 70.0);

    glBegin(GL_TRIANGLES);

    for(li = active_faces.begin(); li != active_faces.end(); li++){

        for(int j = 0; j < 3; j++)
            p[j] = vertices[get_active_vertex_relative(faces[*li].vertices[j])].point;

        glColor3f(faces[*li].color.x, faces[*li].color.y, faces[*li].color.z);

        float color[4];
        for(int ci = 0; ci < 3; ci++)color[ci] = faces[*li].color[ci]*0.4;
        color[4] = 1.0;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
        glMaterialfv(GL_FRONT, GL_AMBIENT, color);
        glMaterialfv(GL_FRONT, GL_SPECULAR, color);

        Point n = cross( p[1] - p[0], p[2] - p[0]);
        n = n/length(n);

        glNormal3f(n.x, n.y, n.z);

        for(int j = 0; j < 3; j++)
            glVertex3f(p[j].x, p[j].y, p[j].z);
    }

    // Draw collpsing...

    glDisable(GL_LIGHTING);

    if(pmv != NULL){

        for(i = 0; i < 2; i++)if((*pmv).flr[i] != -1){

            if(i == 0) {
                glColor3f(0.0, 1.0, 0.0);
            } else {
                glColor3f(1.0, 0.0, 0.0);
            }

            for(j = 0; j < 3; j++){
                x = vertices[get_active_vertex_relative(faces[(*pmv).flr[i]].vertices[j])].point.x;
                y = vertices[get_active_vertex_relative(faces[(*pmv).flr[i]].vertices[j])].point.y;
                z = vertices[get_active_vertex_relative(faces[(*pmv).flr[i]].vertices[j])].point.z;
                glVertex3f(x, y, z);
            }

            glColor3f(1.0, 0.0, 1.0);
            Point p = vertices[(*pmv).vu].point;
            glVertex3f(p.x, p.y+2.7, p.z);
            glVertex3f(p.x-1.9, p.y-1.9, p.z);
            glVertex3f(p.x+1.9, p.y-1.9, p.z);
        }

        for(i = 0; i < 4; i++)if((*pmv).fn[i] != -1){

            glColor3f(0.0, 1.0, 1.0);

            for(j = 0; j < 3; j++){
                x = vertices[get_active_vertex_relative(faces[(*pmv).fn[i]].vertices[j])].point.x;
                y = vertices[get_active_vertex_relative(faces[(*pmv).fn[i]].vertices[j])].point.y;
                z = vertices[get_active_vertex_relative(faces[(*pmv).fn[i]].vertices[j])].point.z;
                glVertex3f(x, y, z);
            }

            x = vertices[get_active_vertex_relative(faces[(*pmv).fn[i]].vertices[(*pmv).fni[i]])].point.x;
            y = vertices[get_active_vertex_relative(faces[(*pmv).fn[i]].vertices[(*pmv).fni[i]])].point.y;
            z = vertices[get_active_vertex_relative(faces[(*pmv).fn[i]].vertices[(*pmv).fni[i]])].point.z;
            glColor3f(0.0, 0.0, 1.0);
            glVertex3f(x,     y+2.7, z);
            glVertex3f(x-1.9, y-1.9, z);
            glVertex3f(x+1.9, y-1.9, z);
        }
    }

    glEnd();
}

bool SRMesh::vertex_spawned(int vid)
{
    int fl = vertices[vid].flr[0];
    int fr = vertices[vid].flr[1];

    if( (fl == -1) && (fr == -1) ) throw MeshException("SRMesh::vertex_spawned: fl and fr both nonexisting.");

    if(fl != -1)return faces[fl].is_active;
    else return faces[fr].is_active;
}

void SRMesh::add_active_vertex(int vind)
{
    if(vind == -1)return;		
    if(vind > vertices.size()){
        throw MeshException("SRMesh::add_active_vertex invalid index");
    }

    vertices[vind].active = active_vertices.insert(active_vertices.end(), MorphNode(vind, FIXED));
    (*vertices[vind].active).tc.resize(tex.cnt());
    vertices[vind].is_active = true;
}

void SRMesh::add_active_vertex_at(int vind, moli::iterator dst, MorphState s)
{
    if(vind == -1)return;
    if(vind > vertices.size()){
        throw MeshException("SRMesh::add_active_vertex invalid index");
    }

    vertices[vind].active = active_vertices.insert(dst, MorphNode(vind, s));
    (*vertices[vind].active).tc.resize(tex.cnt());
    vertices[vind].is_active = true;
}

void SRMesh::add_active_face(int find)
{
    if(find == -1)return;

    faces[find].active = active_faces.insert(active_faces.end(), find);
    faces[find].is_active = true;
}

void SRMesh::remove_active_vertex(int vind)
{
    if(vind == -1)return;

    if(!vertices[vind].is_active){
        throw MeshException("cannot remove inactive");
    }
    active_vertices.erase(vertices[vind].active);
    vertices[vind].is_active = false;
}

void SRMesh::remove_active_face(int find)
{
    if(find == -1)return;

    active_faces.erase(faces[find].active);
    faces[find].is_active = false;
}

int SRMesh::get_active_vertex_relative(int i) const
{
    if(i == -1) {
        throw MeshException("get_active_vertex_relative: vertex lacks parent.");
    }
    if(vertices[i].is_active) {
        return i;
    } else {
        return get_active_vertex_relative(vertices[i].parent);
    }
}

int SRMesh::get_face_vertex(int fid, int vid) const
{
    return get_active_vertex_relative(faces[fid].vertices[vid]);
}

void SRMesh::split_children_at(int vind, moli::iterator& dst, MorphState s)
{
    PMVertex& pmv = vertices[vind];

    // Activate children
    add_active_vertex_at(pmv.vt, dst, s);
    add_active_vertex_at(pmv.vu, dst, s);

    // Activate fl/fr
    add_active_face(pmv.flr[0]);
    add_active_face(pmv.flr[1]);

    // Deactivate v
    remove_active_vertex(vind);

    // Fix neighbouring faces
    for(int i = 0; i < 4; i++)if(pmv.fn[i] != -1)
        faces[pmv.fn[i]].nf[pmv.fni[i]] = pmv.flr[i<2?0:1];
}

void SRMesh::split(int vind)
{
    PMVertex& pmv = vertices[vind];

    // Activate children
    add_active_vertex(pmv.vt);
    add_active_vertex(pmv.vu);

    // Activate fl/fr
    add_active_face(pmv.flr[0]);
    add_active_face(pmv.flr[1]);

    // Deactivate v
    remove_active_vertex(vind);

    // Fix neighbouring faces
    for(int i = 0; i < 4; i++)if(pmv.fn[i] != -1)
        faces[pmv.fn[i]].nf[pmv.fni[i]] = pmv.flr[i<2?0:1];
}

void SRMesh::col_parent_at(int vind, moli::iterator& dst)
{
    PMVertex& pmv = vertices[vind];

    //Deactivate children
    remove_active_vertex(pmv.vt);
    remove_active_vertex(pmv.vu);

    //Deactivate fl/fr
    remove_active_face(pmv.flr[0]);
    remove_active_face(pmv.flr[1]);

    //Activate v
    add_active_vertex_at(vind, dst);

    //Fix neighbouring faces
    if(pmv.fn[0] != -1)faces[pmv.fn[0]].nf[pmv.fni[0]] = pmv.fn[1];
    if(pmv.fn[1] != -1)faces[pmv.fn[1]].nf[pmv.fni[1]] = pmv.fn[0];
    if(pmv.fn[2] != -1)faces[pmv.fn[2]].nf[pmv.fni[2]] = pmv.fn[3];
    if(pmv.fn[3] != -1)faces[pmv.fn[3]].nf[pmv.fni[3]] = pmv.fn[2];
}


void SRMesh::col(int vind, bool add_entering)
{
    PMVertex& pmv = vertices[vind];

    //Deactivate children
    remove_active_vertex(pmv.vt);
    remove_active_vertex(pmv.vu);

    //Deactivate fl/fr
    remove_active_face(pmv.flr[0]);
    remove_active_face(pmv.flr[1]);

    //Activate v
    if(add_entering)add_active_vertex(vind);

    //Fix neighbouring faces
    if(pmv.fn[0] != -1)faces[pmv.fn[0]].nf[pmv.fni[0]] = pmv.fn[1];
    if(pmv.fn[1] != -1)faces[pmv.fn[1]].nf[pmv.fni[1]] = pmv.fn[0];
    if(pmv.fn[2] != -1)faces[pmv.fn[2]].nf[pmv.fni[2]] = pmv.fn[3];
    if(pmv.fn[3] != -1)faces[pmv.fn[3]].nf[pmv.fni[3]] = pmv.fn[2];
}

void SRMesh::switch_face_neighbour(int src, int old_dest, int new_dest, char* caller)
{
    if(src == -1) return;
    if(src == new_dest)
        throw MeshException("SRMesh::switch_face_neighbour: Cannot be my own neighbour!\n");

    int i;

    for(i = 0; i < 3; i++){
        if( (faces[src].nf[i] == new_dest) && (new_dest != -1)){
            throw MeshException("SRMesh::switch_face_neighbour: new_dest already exist");
        }
    }

    //cout << "\nSWITCH:\n\n\nsrc: " << src << "\told_dest: " << old_dest << "\tnew_dest: " << new_dest << "\n";	
    bool chk = false;

    for(i = 0; i < 3; i++){
        //	cout << "facing: " << i << " is " << faces[src].nf[i] << ", ";
        if(faces[src].nf[i] == old_dest){
            faces[src].nf[i] = new_dest;
            chk = true;
        }
    }

    if(chk)return;

    cout << "\nSWITCH:\n\n\nsrc: " << src << "\told_dest: " << old_dest << "\tnew_dest: " << new_dest << "\n";	

    faces[src].report(src);

    cout << "\n" << caller << "\n";
    throw MeshException("SRMesh::switch_face_neigbour: no matching neigbour.");
}


}
}
}
