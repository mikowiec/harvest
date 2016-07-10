/*
 * $Author: peter $
 * $Date: 2002/01/09 13:57:34 $
 * $Log: pm.h,v $
 * Revision 1.34  2002/01/09 13:57:34  peter
 * Yet Another Vector Implementation, this time SimpleYetFlexible(tm)  ;)
 *
 * Revision 1.33  2001/08/06 12:16:17  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.32.2.2  2001/08/03 13:43:55  peter
 * pragma once obsolete...
 *
 * Revision 1.32.2.1  2001/07/31 17:34:05  peter
 * testgren...
 *
 * Revision 1.32  2001/07/30 23:43:20  macke
 * Häpp, då var det kört.
 *
 * Revision 1.31  2001/07/09 13:33:07  peter
 * gcc-3.0 fixar
 *
 * Revision 1.30  2001/07/06 01:47:16  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.29  2001/05/15 01:28:09  truls
 * terrain->render does it all...
 *
 * Revision 1.28  2001/05/14 15:05:46  truls
 * time flows like a river
 *
 * Revision 1.27  2001/05/10 11:05:38  truls
 * Added some functionality and wiped away alot of oldies. Also made good cleanup!
 *
 * Revision 1.26  2001/05/10 01:46:13  truls
 * Major progressive improvements
 *
 * Revision 1.25  2001/05/07 11:10:03  truls
 * Avoid deletion
 *
 * Revision 1.24  2001/05/06 19:52:30  truls
 * Yihaa, starting to work as it should...
 *
 * Revision 1.23  2001/05/06 01:40:30  truls
 * progress across the atlantic
 *
 * Revision 1.22  2001/05/05 18:33:35  truls
 * Simplifying...
 *
 * Revision 1.21  2001/04/28 19:24:01  truls
 * Completely New Makeprog Function. Treap improved. 35 - 80 % Faster treap.update(). Correct Quadric Calculation. Penalties Correct. Etc.
 *
 * Revision 1.20  2001/04/20 08:02:50  macke
 * Camera -> OldCamera (endast namnbyte här och där) samt la till lite headerfiler som inte fås automagiskt längre..
 *
 * Revision 1.19  2001/04/20 07:27:41  macke
 * Headerfiler...
 *
 * Revision 1.18  2001/04/18 22:53:33  truls
 * Generic collapse-find in progress...
 *
 * Revision 1.17  2001/04/18 18:09:54  truls
 * Building ficofac anti-naiv
 *
 * Revision 1.16  2001/04/18 05:07:30  macke
 * Tokstädning
 *
 * Revision 1.15  2001/04/17 22:56:44  truls
 * still hunting bugs
 *
 * Revision 1.14  2001/04/12 00:19:25  truls
 * much ado about nothing
 *
 * Revision 1.13  2001/04/10 21:55:49  truls
 * slowheap, file-split etc
 *
 * Revision 1.12  2001/04/08 23:29:05  truls
 * don't use the pm, it's respawing!
 *
 * Revision 1.11  2001/04/08 17:48:53  truls
 * blah
 *
 * Revision 1.10  2001/04/07 23:11:43  truls
 * camera dependency etc
 *
 * Revision 1.9  2001/04/06 23:28:56  truls
 * new update again
 *
 * Revision 1.8  2001/04/06 18:43:48  truls
 * pm-update förändringar. tunnlar pm-pekare genom gfx-gröten. mm.
 *
 * Revision 1.7  2001/03/20 17:09:11  peter
 * ändrade till lite konsekventare namngivning... (main/3d_types.h)
 *
 * Revision 1.6  2001/03/02 17:34:20  peter
 * gcc gnäller på nästlade kommentarer...
 *
 * Revision 1.5  2001/02/25 12:24:29  peter
 * rotade runt i main/types.h ...
 *
 * Revision 1.4  2001/02/24 02:29:37  macke
 * inlining av v_num_pos()
 *
 * Revision 1.3  2001/02/21 00:01:16  macke
 * GL_EXT_COMPILED_VERTEX_ARRAY samt uppstädning vad gäller inkluderande av headerfiler
 *
 * Revision 1.2  2001/02/19 07:58:55  peter
 * Ska ju bli Vertex av det...
 *
 * Revision 1.1  2001/02/19 02:53:54  macke
 * PM för terrängen, riktig dimma, multitexture fix och lite mer
 *
 */

#ifndef REAPER_GFX_PM_H
#define REAPER_GFX_PM_H

#include "main/types_ops.h"
#include "main/types_io.h"
#include <stdio.h>
#include <vector>
#include <list>
#include <cmath>
#include <queue>
#include <iostream>
#include <set>
#include "gfx/camera.h"
#include "pm_types.h"
#include "badheap.h"

using namespace std;

namespace reaper 
{
namespace gfx
{
namespace pm
{

/*************************************************************************
* typedefs: Oh, I'm saving space now!                                    *
*************************************************************************/

typedef std::vector<int> vint;

typedef std::list<int> lint;

typedef std::set<int> sint;

/*************************************************************************
* Class Matrix3d: 3x3-matrix with simple operations			 *
*************************************************************************/

class Matrix3d {
public:
	double e[3][3];

	Matrix3d(void){
		for(int i=0;i<3;++i) for(int j=0;j<3;++j) e[i][j] = 0.0;
	}

	Matrix3d(double  a[3][3]){
		for(int i=0;i<3;++i) for(int j=0;j<3;++j) e[i][j] = a[i][j];
	}

};

/*************************************************************************
* Class Vertex:								 *
*************************************************************************/
class Vertex
{
public:
	Point p;
	Color c;
	TexCoord t;	

	Vertex(void) : p(0,0,0)
	{
	}

	Vertex(double a, double b, double c) : p(a,b,c)
	{
	}

	Vertex(const Point &P, const Color &C, const TexCoord &T) : p(P), c(C), t(T)
	{
	}

	template<class T>
	Vertex(const Vec<T> &P) : p(P.x, P.y, P.z)
	{
	}

	void set(double a, double b, double c)
	{
		p.x = a; 
		p.y = b; 
		p.z = c;
	}

	void normalize(void) {
		p = norm(p);
	}

	double len(void) const {
		return length(p);
	}
};

inline Vertex crossp(const Vertex &a, const Vertex &b)
{
	return (cross(a.p, b.p).to_pt());
}

inline Vertex operator+(const Vertex &a, const Vertex &b)
{
	return a.p + b.p;
}

inline Vertex operator-(const Vertex &a, const Vertex &b)
{
	return a.p - b.p;
}

inline Vertex operator*(double r,Vertex a)
{
	a.p *= r;
	return a;
}

inline double scalarp(const Vertex &a, const Vertex &b)
{
	return dot(a.p,b.p);
}



/*************************************************************************
* Class Facedat: VDATLIST indexes for a heap                             *
*************************************************************************/

class Facedat {
public:
	int v[3];
	int pmi;
	int pfi;
	bool active;
	int vfi;
	Facedat(void){}
	Facedat(int a, int b, int c);	
};

typedef std::vector<Facedat> Facelist;


class sgt {
public:
	bool operator()(const Pairdat &a, const Pairdat &b) const
	{
		return (a > b);
	}

	
	bool operator()(const Vpdi &a, const Vpdi &b) const
	{
		return !(*a < *b);
	}
};

//typedef boost::splay_heap<Vpdi, sgt>::pointer Pdhp;
//typedef boost::splay_heap<Pairdat, sgt>::pointer Pairdathp;


class Hads{
public:
//	Pairdathp	hp;
	int	vi;

	bool operator==(const Hads &a) const
	{
		return vi == a.vi;
	}
};

typedef std::list<Hads> Hadslst;

/*************************************************************************
* Class Verdat: Stores individual vertex data                            *
*************************************************************************/

class Verdat {
private:
	//Quadric		Q;   // Quadric
public:
	lint		pairs; // pairs on vpd affecting this vertex
	int			vvi; // Virtual vertex index
	Vertex		position; // Absolute position
	lint		fl;  // list of original faces
	vint		ofl;
	Hadslst		pl; //array of pairlistiterators acting like pointers
	lint		qfl; // list of faces for which are applyable to quadric-meassures.
	lint		incoming; // list of vertices relaying to this one

	Slowheap_index_list spairs; // Pointers into the slowheap;

	void reminc(int vid); // remove vertex vid from incoming
	void retreat(std::vector<Verdat> &vdlt,const int &vid); // reroute all incoming to vid

	Vertex pos(void);
	Verdat(void){}
	Verdat(Vertex v):position(v){} //OH! look how nice it is with initializers!
};

// Used for the VDATLIST
typedef std::vector<Verdat> Vdatvec;

/*************************************************************************
* Class Quadric: Using the (A,b,c) representation                        *
*************************************************************************/

class Quadric {
public:
	wsMatrix3x3	a;
	wsVertex3	b;
	double		c;

	void clear(void){	a.e[0][0] = a.e[0][1] = a.e[0][2] =
						a.e[1][0] = a.e[1][1] = a.e[1][2] =
						a.e[2][0] = a.e[2][1] = a.e[2][2] =
						b.e[0] =    b.e[1] =    b.e[2] = 
						c =         0.0;}

	Quadric(void)
	{
		c = 0.0; //The others are zeroed by default-constructors.
	}

	double cost(const int id, const Vdatvec &vdl);
};


/*************************************************************************
* Class Camdat: Camera data                                              *
*************************************************************************/

class Camdat {
public:
	Vertex	origo;//(-10.0, 0.0, 0.0);
	Vertex	lookat;
	Vertex	up;
	double		horizang;
	double		vertang;

	Camdat(void):origo(-10.0, 0.0, 0.0), horizang(180), vertang(180){};
};

/*************************************************************************
* Class Casdat: Contraction and Split - data                             *
*************************************************************************/

struct Casdat {

	int f_left, f_right; // face to the left and right respectively
	int v_left, v_right; // vertices to the left and right
	int fn[4];
	int vn[4];
	int vis;
	int vpos;

	int	pos;	//which vertex it actually applies to in the VDATLIST
	int     is;	//which index it gets subsequent to the contraction
	//lint	fl;	//a list of indexes to the faces that are collapsed
	vint	af; // These babies are just slightly modified.
	bool	split;	//flagging cas-status

	int		cf[2][2];	// Center Facees
};

typedef std::vector<Casdat> Vcas;

/*************************************************************************
* Class Pmd: The grand progressive class and interface                   *
*************************************************************************/


class Pmd {
private:

	bool				mesh_made;

	vint				cas_grid[10][10];

	double				num_con_limit; // Numerical Contraction Limit

	double				max_x;
	double				max_y;
	double				max_z;
	double				min_x;
	double				min_y;
	double				min_z;

	int					cas_delta; // nr of grid

	double				tan_max_cam_angle; // store semi-static angle;
	wsVertex3			cam_e;

	double				alpha_threshold;

	int					nr_of_conts; // during last update
	int					nr_of_splits; // during last update

	int					update_steps;

	bool				ready_to_update;  // Ready to update?

	OldCamera			cam;	//I am a Camera. Camera, Camera...
	Vcas				cas;	//Contractions and Splits
	lint				s;		//Possible splits
	lint				c;		//Possible contraction
	int					poly_count; // number of active polys
	int					nr_of_polys; // total number of faces

	int					cas_index;

	Vpd vpd;			// Vector containing indexed pair-info


	class	BuildProblem{ };	// makeprog exception

	double alpha_err_split(int cid);
	double alpha_err_cont(int cid);

	bool validc(int cont);		//Valid contraction?
	bool valids(int cont);		//Valid split?
	bool tri_collapse(int fid);	//Is face #fid collapsed?
	bool spe_cont_ok(int cid);	//Screen-Space error ok after contraction cid
	bool spe_split_ok(int cid);	//Screen-Space error too big and improved after split cid
	bool within_view(int vid);	//Is the indicated vertex within the view frustum
	bool cas_within_view(int cid);
	bool structural_singularity(Pairdat p); // Does the contraction result in a mesh-collapse?

	bool face_neighbour(int fi1, int fi2); // is fi1 neighbour with fi2?
	bool faces_aligned(int f0, int f1);
	bool face_includes_pair(int fid, int v0, int v1, int &other_one); // fid next to v0->v1 ?
	bool face_made_of(int fid, int v0, int v1, int v2); // Face made of v0, v1 & v2 ?
	bool face_includes_vertex(int fid, int vid);

	bool no_flip_overs(int cid);
	bool left_of(int fid, Casdat &cd); // face fid left of cd?

	int con_cycle(Pairdat &pd, int v1, int v2); // Does pd contain a cycle? How serious is it?
	int othervertex(int pdi, int vi); // Find the other vertex of a contraction
	bool vcon(int v1, int v2);	// Is vertex v1 and v2 connected?
	bool check_vertex(int vi);	// is the vertex non-optimal?

	bool remove_active_faces(int cid);	//Remove active faces associated with cid (contraction id)
	bool add_active_faces(int cid);	//Add active faces associated with cid (contraction id)

	bool contract(int cid);		//contract mesh
	bool split(int cid);		//split mesh

	bool ficofac(Pairdat &pd, vint &affac, Casdat &cd, sint &colpar);
	int ficocov(Pairdat &pd, int vi, int from); // Find Contraction-Common Vertex
	int simple_ficofac(Pairdat &pd, lint &colfac, sint &colpar); // Will fail if contraction is too complex

	float angle(int vo, int v1, int v2); // Angle between vo-v1 and vo-v2

	void sofex(char * text);	// Soft-Exception: Output Errormessage and wait for carrige-return

	void view_neighbours(int vi);   // print the neighbours of a vertex
	void view_faces(int vi);	// print the faces of a vertex



public:
	Vdatvec				vdl;	//VDATLISTen!
	Facelist			fl;		//Face data

	vint				progmesh; // The Progressive Mesh Data
	vint				freespots; // Freespots in the progmesh

	vint				progfac;




	void quad(int id, Quadric &q); //return the corresponding  quadric.
	Facedat lastface(void);			//returns last faces
	Vertex lastvertex(void);		//returns last vertex;

	inline int  findcurvi(int vid);	//Find the current vertex id of the virtual vertex id vid

	void set_num_con_lim(double val);	//set initial highest cost accepted for a contraction
	void set_alpha_threshold(double val);
	void set_cam(OldCamera nc);			//set cam-coordinates

	void makeprog(void);			//when vertices and faces are uploaded, make progressive
	void update(void);				//if camera is changed make an update
	void update_fast(void);				
	void update_cas_grid(void);

	bool singlecontract(void);		//Make a single contraction
	bool singlesplit(void);			//Make a single split
	void addv(Vertex v);			//add a vector
	void addv(double a, double b, double c);
	void addf(int a, int b, int c);	//add a face
	void set_poly_count(float percent);// Decide visual poly percentage
	void set_number_of_polys(int cnt);
	void set_cas_delta(int val){cas_delta = val;};
	void set_update_steps(int val){update_steps = val;};


	void print_vertices(void);
	void print_faces(void);
	void print_info(void);		//View current Pmd-configuration and status
	void print_cas(void);
	void print_quads(void);
	void report(void);			//Report stats about the mesh
	void analyze_contractions(void);

	void report_config(void);	// Report current mesh configuration

	Vertex vpos(int vid);     // Returns the virtual position
	
	// Returns the index in vdl for the virtual position
	inline int v_num_pos(const int vid) { return vdl[vid].vvi; }

	Pmd(void):
		mesh_made(false), num_con_limit(100.0),
		max_x(0), max_y(0), max_z(0), min_x(0), min_y(0), min_z(0),
		update_steps(1), ready_to_update(false),
		cam(Point(), Point(), Vector(), 0.0, 0.0), poly_count(0), nr_of_polys(0)
	{ }
	int polycount(void){return poly_count;}
};

// FUNCTIONS
/*
Vertex crossp(const Vertex &a, const Vertex &b);

Vertex operator+(Vertex a, Vertex b);
Vertex operator-(Vertex a, Vertex b);
*/
ostream& operator<<(ostream& os, const Facedat& fd);
//ostream& operator<<(ostream& os, const Vertex& vec);
ostream& operator<<(ostream& os, const Vertex& vec);
ostream& operator<<(ostream& os, const Pairdat& pd);

ostream& operator<<(ostream& os, const wsVertex3 &v);

bool similar(const Pairdat &a, const Pairdat &b);

}
}
}

#endif
