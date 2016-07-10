#ifndef REAPER_MESH_PMVERTEX_H
#define REAPER_MESH_PMVERTEX_H

#include <list>
#include <vector>
#include "main/types.h"

namespace reaper 
{
namespace gfx
{
namespace mesh
{


class Contraction;

struct RefineInfo {
	float bsr; // Bounding sphere radius
};

class TexCoord {
public:

	TexCoord()
	{
		a[0] = a[1] = a[2] = a[3] = a[4] = a[5] = 1.0;
	};

	float u, v;
	float a[6];

	TexCoord operator+(const TexCoord& b);
	TexCoord operator*(const float f);
	TexCoord operator=(const TexCoord& b);
	void set_alpha(float param);

};

// Geometry Core Class nice to pass around amongst the renderers
class GeomCore {
public:
	Point point;
	Point color;
	Vector normal;
        std::vector<TexCoord> tc;
};

enum MorphState { FIXED, SPLIT, COL, ZOMBIE };

class MorphNode: public GeomCore {
public:
	int id;
	int src;
	MorphState state;
	float alpha;

	int rendi;

	MorphNode(int i, MorphState s, float a = 0.0, int d = -1) : 
                id(i), state(s), alpha(a), src(d)
	{}

        ~MorphNode() {}

};

typedef std::list<MorphNode> moli;


class PMSkeleton {
public:
	int vertex_count;
	Point* vertices;
};


class PMVertex : public GeomCore {
public:
	moli::iterator active; // list stringing active vertices V
	bool is_active;
	int parent;		// -1 if this vertex is in M0
	int vt;			// -1 if this vertex is in ^M
	int vu;			// -1 if this vertex is in ^M
	
	// Remaining fields encode vsplit information, defined if vt 6= 0.
	int flr[2];				
	int fn[4];				// required neighbours fn0; fn1; fn2; fn3
	int fni[4];				// Indicates which index points to flr
	float flr_area;			// Used for quick-n-dirty screen-space-error calculation
	RefineInfo refine_info;

	PMVertex& operator=(const Contraction& c);

	PMVertex(){};
	PMVertex(const Contraction& c);		

	bool has_children();
	bool has_parent();
	
	void report(int i) const;

	void render();
};


struct PMFace {
	std::list<int>::iterator active; // list stringing active faces F
	bool is_active;
	int matid;			// material identifier
	Point normal;
	Point color;

	// Remaining fields are used if the face is active.
	int vertices[3];	// ordered counter-clockwise
	int nf[3];	// neighbouring faces[i] across from vertices[i]

	void report(int i) const;
};

}
}
}

#endif
