#include <iostream>

#include "os/compat.h"
#include "main/types_ops.h"
#include "types.h"

namespace reaper{

namespace phys{

Edge::Edge(Vertex* tail, Vertex* head) : vhead(head), vtail(tail){
	Vector tmp = *vhead - *vhead;
	vector = tmp/length(tmp);
};
	
	
Plane::Plane(const Vector& norm, const Vertex& p){
	unit_norm = norm/length(norm);
	unit_norm.w = -p.x*unit_norm.x - p.y*unit_norm.y - p.z*unit_norm.z;
};

/// Assemble polyhedron from mesh
Polyhedron* build_polyhedron(const Mesh& m){

	Polyhedron* object = new Polyhedron();
	
	Vertex **verts = new (Vertex*)[m.num_verts];
	
	for(int i=0; i<m.num_verts; i++){
		verts[i] = new Vertex(m.vert_coords[i][0], 
				      m.vert_coords[i][1], 
				      m.vert_coords[i][2]);
		object->verts.push_back(verts[i]);
	}
	
	for(int i=0; i<m.num_edges; i++){
		Edge *edge = new Edge(verts[m.edge_verts[i][0]],
				      verts[m.edge_verts[i][1]]);

		verts[m.edge_verts[i][0]]->edges.push_back(edge);
		verts[m.edge_verts[i][1]]->edges.push_back(edge);

		object->edges.push_back(edge);
	}

	int c = 0;

	for(int i=0; i<m.num_faces; i++){
		Face *face = new Face();
		for(int j=0; j<m.num_face_verts[i]; j++){
			Vertex *prev, *v1 = verts[m.face_verts[c]];

			int pid = (j == 0 ? m.face_verts[c+m.num_face_verts[i]-1] : m.face_verts[c-1]);

			prev = verts[pid];

			face->verts.push_back(v1);

//			cout << "Vertex edge num#: " << v1->edges.size() << " for " << m.face_verts[c] << "\n";

//			cout << "Testing against " << pid << "\n";

			typedef Vertex::EdgeIterator EI;

			for(EI k = v1->begin_edges(); k != v1->end_edges(); ++k){
				Edge *edge = *k;
				if(v1 == edge->vhead && prev == edge->vtail){
						edge->fright = face;
						face->edges.push_back(edge);
						face->right_edges.push_back(edge);
				} else if (v1 == edge->vtail && prev == edge->vhead){
						edge->fleft = face;
						face->edges.push_back(edge);
						face->left_edges.push_back(edge);
				}
			}
			c++;
		}
		Vector v1(*(face->verts[0]));
		Vector v2(*(face->verts[1]));
		Vector v3(*(face->verts[2]));
		Vector n = cross(v1-v2, v3-v2);
		face->norm = n/length(n);

//		cout << face->edges.size() << "\n";
				
		object->faces.push_back(face);
	}

	typedef Polyhedron::VertexIterator VI;

	for(VI i = object->begin_vertices(); i != object->end_vertices(); i++){
		Vertex *vert = *i;
		
		typedef Vertex::EdgeIterator EI;

		for( EI j = vert->begin_edges(); j != vert->end_edges(); j++){
			Edge *cobnd = *j;

			Vector norm;

			if(vert == cobnd->vhead)
				norm = cobnd->vector;
			else
				norm = -cobnd->vector;

			Cell *cell = new Cell(Plane(norm, *vert), cobnd);

			vert->add_cell(cell);

			object->cells.push_back(cell);
		}
	}

	typedef Polyhedron::EdgeIterator EI;

	for(EI i = object->begin_edges(); i != object->end_edges(); i++){
		Edge *edge = *i;
		
		Vector norm = edge->vector;
		Cell *cell = new Cell(Plane(norm, *(edge->vtail)), edge->vtail);
		edge->add_cell(cell);
		object->cells.push_back(cell);

		cell = new Cell(Plane(-norm, *(edge->vhead)), edge->vhead);
		edge->add_cell(cell);
		object->cells.push_back(cell);

		norm = cross(edge->fright->norm, edge->vector);
		cell = new Cell(Plane(norm, *(edge->vtail)), edge->fright);
		edge->add_cell(cell);
		object->cells.push_back(cell);

		norm = cross(edge->vector, edge->fleft->norm);
		cell =  new Cell(Plane(norm, *(edge->vtail)), edge->fleft);
		edge->add_cell(cell);
		object->cells.push_back(cell);
	}
	
	typedef Polyhedron::FaceIterator FI;

	for(FI i = object->begin_faces(); i != object->end_faces(); i++){
		Face *face = *i;

		typedef Face::EdgeIterator EI;

		for(EI j = face->begin_edges(); j != face->end_edges(); ++j){
			Edge *edge = *j;
			Vector norm;

			if(edge->fright == face)
				norm = cross(edge->vector, face->norm);
			else
				norm = cross(face->norm, edge->vector);

			Cell* cell = new Cell(Plane(norm, *(edge->vtail)), edge);

			face->add_cell(cell);

			object->cells.push_back(cell);
		}

	}


	return object;
}

}

}



