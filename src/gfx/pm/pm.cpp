
#include "os/compat.h"
#include <iomanip>

#include "pm.h"
#include "treap.h"
#include "os/gl.h"
#include "os/debug.h"
#include "gfx/gfx.h"
#include "gfx/io.h"
#include "badheap.h"
#include "misc/sequence.h"

namespace reaper 
{
namespace gfx
{
namespace pm
{

void operator+=(Matrix3d &a, const Matrix3d &b)
{
	for(int i=0;i<3;++i){
		for(int j=0;j<3;++j){
			a.e[i][j] += b.e[i][j];
		}
	}
};

Matrix3d operator+(const Matrix3d &a, const Matrix3d &b)
{
	Matrix3d m;
	for(int i=0;i<3;++i){
		for(int j=0;j<3;++j){
			m.e[i][j] = a.e[i][j] + b.e[i][j];
		}
	}
	return m;
};


/*
double Vertex::len(void)
{
	return sqrt(e[0]*e[0]+e[1]*e[1]+e[2]*e[2]);
}

void Vertex::normalize(void)
{
	double r = sqrt(e[0]*e[0]+e[1]*e[1]+e[2]*e[2]);
	e[0] = e[0] / r;
	e[1] = e[1] / r;
	e[2] = e[2] / r;
}

Vertex operator+(Vertex a, Vertex b)
{
	Vertex r(a.e[0]+b.e[0], a.e[1]+b.e[1], a.e[2]+b.e[2]);
	return r;
}

Vertex operator-(Vertex a, Vertex b)
{
	Vertex r(a.e[0]-b.e[0], a.e[1]-b.e[1], a.e[2]-b.e[2]);
	return r;
}


Vertex operator*(double r, Vertex a)
{
	a.e[0]*=r;
	a.e[1]*=r;
	a.e[2]*=r;

	return a;
}
*/
Quadric operator*=(Quadric &q, double r)
{
	for(int i=0;i<3;++i) {
		for(int j=0;j<3;++j) q.a.e[i][j] *= r;
		q.b.e[i] *= r;
	}
	q.c *= r;
	return q;
}

/*
bool operator==(const Pairdat &a, const Pairdat &b)
{
	return (a.v[0] == b.v[0]) && (a.v[1] == b.v[1]);
}
*/
bool similar(const Pairdat &a, const Pairdat &b)
{
	return( (a.v[0] == b.v[0] && a.v[1] == b.v[1]) ||
		    (a.v[1] == b.v[0] && a.v[0] == b.v[1]));
}
/*
Vertex scalap(const Vertex &a, const Vertex &b)
{
	Vertex r;
	r.e[0] = (a.e[0]*b.e[0]+a.e[1]*b.e[1]+a.e[2]*b.e[2]);
	return r;
}
*/

/*
double scalarp(Vertex a, Vertex b)
{
	return a.e[0]*b.e[0]+a.e[1]*b.e[1]+a.e[2]*b.e[2];
}
*/

Facedat::Facedat(int a, int b, int c)
{
	v[0] = a;
	v[1] = b;
	v[2] = c;
	a = true;
}


ostream& operator<<(ostream& os, const wsVertex3& v)
{
	os << "(" << v.e[0] << ", " << v.e[1] << ", " << v.e[2] << ")";
	return os;
}

ostream& operator<<(ostream& os, const Facedat& fd)
{
	os << "(" << fd.v[0] << ", " << fd.v[1] << ", " << fd.v[2] << ")";
	return os;
}


ostream& operator<<(ostream& os, const Vertex& v)
{
	os << "Point: " << v.p;
	os << "\nColor: " << v.c;
	return os << "\nTexCr: " << v.t;	
}

ostream& operator<<(ostream& os, const Quadric& q)
{
	int i,j;
	for(i=0;i<3;++i){
		for(j=0;j<3;++j){
			os.precision(4);
			os << q.a.e[i][j] << '\t';
		}
		os << q.b.e[i] << '\n';
	}
	os << q.c;
	return os;
}


double Quadric::cost(const int id, const Vdatvec &vdl)
{

	wsVertex3 tv;
	wsVertex3 v_conv(vdl[id].position.p);

	//cout << v.p;

	//cout << "Calculating the cost of a Quadric: " << endl << *this << endl;
	

	double t1 = 0.0, t2 = 0.0;
	//A*v
	tv = a*v_conv;
	/*
	for(i=0;i<3;++i){
		for(j=0;j<3;++j){
			tv[i] += (double) a.e[i][j] * (double) v.p[j];
		}
	}
	*/

	cout << std::setw(1) << std::setprecision(15);

	/*
	cout << "Av: " << endl;
	for(i = 0;i<3;++i)
		cout << tv.e[i] << ", ";
	cout << endl;
	*/
	//v[transponat]*(A*v)
	/*
	for(i=0;i<3;++i){
		t1 += (double) v.p[i] * tv.e[i];
	}
	*/

	t1 = scalarp(v_conv, tv);
	
	//2*b[transponat]*v
	/*
	for(i=0;i<3;++i){
		t2 += (double) v.p[i] * (double) b.p[i];
	}
	t2 *= 2;
	*/

	t2 = 2*scalarp(v_conv, b);

	/*
	cout << "cost. vTAv: / 2*bT: / d*d: " << endl;
	cout << t1 << endl;
	cout << t2 << endl;
	cout << c << endl;
	cout << "cost. sum: " << t1 + t2 + c << endl;
	*/

	return t1 + t2 + c;
}
/*
Quadric operator+(const Quadric &a, const Quadric &b)
{
	Quadric q;
	q.a = a.a + b.a;
	q.b = a.b + b.b;
	q.c = a.c + b.c;
	return q;
}
*/

ostream& operator<<(ostream& os, const Pairdat& pd)
{
	os << "(" << pd.v[1] << " -> " << pd.v[0] << ") with cost: " << pd.cost;
	return os;
}

void Verdat::reminc(int vid)
{
	incoming.erase(std::find(incoming.begin(), incoming.end(), vid));
}

void Verdat::retreat(std::vector<Verdat> &vdl, const int &vid)
{
	vvi = vid;
	for(lint::iterator i=incoming.begin();i != incoming.end(); ++i){
		vdl[(*i)].retreat(vdl, vid);
	}
}





ostream& operator<<(ostream& os, Casdat& cd)
{
	os << "contraction: " << cd.pos << "->" << cd.is << "\tfl / fr: ";
	os << cd.f_left << " / " << cd.f_right << endl;
	os << "v_left / v_right:\t" << cd.v_left << " / " << cd.v_right << endl;
	os << "fn0 / fn1:\t" << cd.fn[0] << " / " << cd.fn[1] << endl;
	os << "fn2 / fn3:\t" << cd.fn[2] << " / " << cd.fn[3] << endl;
	
	return os;
}



float Pmd::angle(int vo, int v1, int v2)
{
	Vertex a, b;

	a = vdl[v1].position - vdl[vo].position;
	b = vdl[v2].position - vdl[vo].position;

	float r = (acos(scalarp(a,b)/(a.len()*b.len()))) * (180.0/3.14159);

	return r;
}

bool Pmd::left_of(int fid, Casdat &cd)
{
	int v0 = -1, v1 = -1;

	for(int i = 0; i < 3; ++i)
		if(cd.pos == findcurvi(fl[fid].v[i]))
			v1 = i;
		else if(cd.is == findcurvi(fl[fid].v[i]))
			v0 = i;



	return ((v0 - v1 == 1) || (v0 - v1 == -2));
}

bool Pmd::check_vertex(int vi)
{
	debug::DebugOutput dout("checkvertex");
	lint::iterator li;
	int i;

	for(li = vdl[vi].fl.begin(); li != vdl[vi].fl.end(); ++li)
		for(i = 0; i < 3; ++i)
			if(findcurvi(fl[*li].v[i]) == findcurvi(fl[*li].v[(i+1)%3])){
				dout << "Face " << *li << " at " << vi << " is collapsed." << endl;
				cin.get();
			}

	for(li = vdl[vi].pairs.begin(); li != vdl[vi].pairs.end(); ++li)
		if(vpd[*li].v[0] == vpd[*li].v[1]){
			dout << "Pair " << *li << " at " << vi << " is already used." << endl;
			cin.get();
		}

	return false;
}


void Pmd::sofex(char * text)
{
	debug::DebugOutput dout("SOFTEXCEPTION");
	dout << text << endl;
	cin.get();
}

bool Pmd::faces_aligned(int f0, int f1)
{
	int i;
	int j;
	int cnt = 0;

	if( (f0 < 0) || (f1 < 0) )sofex("faces_aligned got negative index");


	for(i = 0; i < 3; ++i)
		for(j = 0; j < 3; ++j)
			if(findcurvi(fl[f0].v[i]) == findcurvi(fl[f1].v[j]))
				++cnt;

	return (cnt > 1);
}

bool Pmd::vcon(int v1, int v2)
{
	lint::iterator li;

	for(li = vdl[v1].pairs.begin();li != vdl[v1].pairs.end();++li)
		if((vpd[*li].v[0] == v2) || (vpd[*li].v[1] == v2))
			return true;

	return false;
}


int Pmd::con_cycle(Pairdat &pd, int v1, int v2)
{
	debug::DebugOutput dout("con_cycle");
	int cur_ver = v1;
	int last = v1;
	int next_ver;
	int count = 0;

	for(;;){
		next_ver = ficocov(pd, cur_ver, last);

		if(next_ver == v2){
			if(count > 5){
				dout << "intermediate vertices: " << count << endl;
				cin.get();
			}
			return count;
		}

		if(next_ver == -1){
			if(count > 5){
				dout << "intermediate vertices: " << count << endl;
				cin.get();
			}
			return -1;
		}

		++count;
		last = cur_ver;
		cur_ver = next_ver;
	}
}



void Pmd::report_config(void)
{
	debug::DebugOutput dout("pm::report_config");

	dout << "splits/conts: " << nr_of_splits << "/" << nr_of_conts;
	dout << " polys: " << poly_count;
	dout << ", pm.size: " << progmesh.size() << endl;
}

void Pmd::analyze_contractions(void)
{
	Vcas::iterator vci;

	for(vci = cas.begin(); vci != cas.end(); ++vci)
		if(!((*vci).split)){
			cout << "Still A Contraction: " << endl;
			cout << *vci << endl;

			cout << "virtual: " << findcurvi((*vci).pos)  << "->" << findcurvi((*vci).is) << endl;

			cout << "v_left: ";
			if((*vci).v_left != -1)cout << findcurvi((*vci).v_left);
			else cout << "-1";
			cout << endl;

			cout << "v_right: ";
			if((*vci).v_right != -1)cout << findcurvi((*vci).v_right);
			else cout << "-1";
			cout << endl;

			if((*vci).f_left == -1){
				cout << "f_left is -1" << endl;
			} else {
				if(fl[(*vci).f_left].active){
					cout << "f_left: (";
					cout << findcurvi( fl[(*vci).f_left].v[0]) << ", ";
					cout << findcurvi( fl[(*vci).f_left].v[1]) << ", ";
					cout << findcurvi( fl[(*vci).f_left].v[2]) << ")" << endl;
				} else {
					cout << "f_left not active" << endl;
				}
			}

			if((*vci).f_right == -1){
				cout << "f_right is -1" << endl;
			} else {
				if(fl[(*vci).f_right].active){
					cout << "f_right: (";
					cout << findcurvi( fl[(*vci).f_right].v[0]) << ", ";
					cout << findcurvi( fl[(*vci).f_right].v[1]) << ", ";
					cout << findcurvi( fl[(*vci).f_right].v[2]) << ")" << endl;
				} else {
					cout << "f_right not active" << endl;
				}
			}

			if((*vci).fn[0] == -1){
				cout << "fn[0] is -1";
			} else {
				cout << "fn[0]: (";
				cout << findcurvi( fl[(*vci).fn[0]].v[0]) << ", ";
				cout << findcurvi( fl[(*vci).fn[0]].v[1]) << ", ";
				cout << findcurvi( fl[(*vci).fn[0]].v[2]) << ")";
				if(fl[(*vci).fn[0]].active)	cout << " not active";
			}
			cout << endl;

			if((*vci).fn[1] == -1){
				cout << "fn[1] is -1";
			} else {
				cout << "fn[1]: (";
				cout << findcurvi( fl[(*vci).fn[1]].v[0]) << ", ";
				cout << findcurvi( fl[(*vci).fn[1]].v[1]) << ", ";
				cout << findcurvi( fl[(*vci).fn[1]].v[2]) << ")";
				if(fl[(*vci).fn[1]].active)	cout << " not active";
			}
			cout << endl;
 
			if((*vci).fn[2] == -1){
				cout << "fn[2] is -1";
			} else {
				cout << "fn[2]: (";
				cout << findcurvi( fl[(*vci).fn[2]].v[0]) << ", ";
				cout << findcurvi( fl[(*vci).fn[2]].v[1]) << ", ";
				cout << findcurvi( fl[(*vci).fn[2]].v[2]) << ")";
				if(!fl[(*vci).fn[2]].active) cout << " not active";
			}
			cout << endl;

			if((*vci).fn[3] == -1){
				cout << "fn[3] is -1";
			} else {
				cout << "fn[3]: (";
				cout << findcurvi( fl[(*vci).fn[3]].v[0]) << ", ";
				cout << findcurvi( fl[(*vci).fn[3]].v[1]) << ", ";
				cout << findcurvi( fl[(*vci).fn[3]].v[2]) << ")";
				if(fl[(*vci).fn[3]].active)	cout << " not active";
			}
			cout << endl;

			cin.get();
		}

}
		

void Pmd::view_neighbours(int vi){
	lint::iterator li;
	debug::DebugOutput dout("vneighbours");

	dout << vi << ": ";

	for(li = vdl[vi].pairs.begin(); li != vdl[vi].pairs.end(); ++li)
		if(vpd[*li].v[0] == vi)
			dout << "(" << *li << ")" << vpd[*li].v[1] << ", ";
		else
			dout << "(" << *li << ")" << vpd[*li].v[0] << ", ";

	dout << endl;
}


void Pmd::view_faces(int vi)
{
	lint::iterator li;
	int i;
	debug::DebugOutput dout("vfaces");

	dout << "Vertex: " << vi << " faces: " << endl;
	for(li = vdl[vi].fl.begin(); li != vdl[vi].fl.end(); ++li){
		dout << *li << ": ";
		for(i = 0; i < 3; ++i)	
			dout << findcurvi(fl[*li].v[i]) << ", ";
		dout << endl;
	}
}

int Pmd::ficocov(Pairdat &pd, int vi, int prev) // Find Contraction-Common Vertex
{
	debug::DebugOutput dout("ficocov");
	int vtmp = -1;
	lint::iterator li;

	for(li = vdl[vi].pairs.begin(); li != vdl[vi].pairs.end(); ++li){

		if(vpd[*li].v[0] != vi)
			vtmp = vpd[*li].v[0];
		else if(vpd[*li].v[1] != vi)
			vtmp = vpd[*li].v[1];
		else
			sofex("ficocov: Invalid Pair!");

		if((vtmp != pd.v[0]) && (vtmp != pd.v[1]) && (vtmp != prev)){
			if(vcon(vtmp, pd.v[0]) && vcon(vtmp, pd.v[1]))
				return vtmp;
		}
	}

	return -1;
}

int Pmd::othervertex(int pdi, int vi)
{
	if(vpd[pdi].v[0] == vi)
		return vpd[pdi].v[1];
	else if(vpd[pdi].v[1] == vi)
		return vpd[pdi].v[0];
	else
		sofex("othervertex: MISSUSE!!!");

	return -1;
}

int Pmd::simple_ficofac(Pairdat &pd, lint &colfac, sint &colpar)
{

	debug::DebugOutput dout("ficofac");

	int i;
	lint::iterator li;
	int thisone;
	lint closev;

	if( (findcurvi(pd.v[0]) != pd.v[0]) || (findcurvi(pd.v[1]) != pd.v[1]) ){
		cout << "EXCEPTION: ficofac!" << endl;
		cin.get();
	}

	// Find thisone
	for(li = vdl[pd.v[0]].pairs.begin(); li != vdl[pd.v[0]].pairs.end(); ++li)
		if(vpd[*li] == pd){
			thisone = *li;
			break;
		}


	// Finding common faces for pd.v[0] and pd.v[1]
	for(li = vdl[pd.v[0]].fl.begin(); li != vdl[pd.v[0]].fl.end(); ++li)
		if(std::find(vdl[pd.v[1]].fl.begin(), vdl[pd.v[1]].fl.end(), *li) != vdl[pd.v[1]].fl.end())
			colfac.push_back(*li);

	// Finding pd-close vertices
	for(li = colfac.begin();li != colfac.end(); ++li){
		if( (findcurvi(fl[*li].v[0]) != pd.v[0]) && (findcurvi(fl[*li].v[0]) != pd.v[1]))
			closev.push_back(findcurvi(fl[*li].v[0]));
		else if( (findcurvi(fl[*li].v[1]) != pd.v[0]) && (findcurvi(fl[*li].v[1]) != pd.v[1]))
			closev.push_back(findcurvi(fl[*li].v[1]));
		else if( (findcurvi(fl[*li].v[2]) != pd.v[0]) && (findcurvi(fl[*li].v[2]) != pd.v[1]))
			closev.push_back(findcurvi(fl[*li].v[2]));
		else sofex("ficofac: No Base Triangle!");
		
	}

	// Return true if contraction is too complex
	for(li = closev.begin(); li != closev.end(); ++li)
		if(ficocov(pd, *li, -1) != -1)
			return 1;

	for(li = vdl[pd.v[1]].pairs.begin(); li != vdl[pd.v[1]].pairs.end(); ++li){
		i = othervertex(*li, pd.v[1]);
		if(
			(i == pd.v[0]) ||
			(std::find(closev.begin(), closev.end(), i) != closev.end())
		  )
			colpar.insert(*li); 
	}

	return 0;
}

bool Pmd::no_flip_overs(int cid)
{

	bool val = true; 

	vdl[cas[cid].pos].vvi = cas[cid].pos;

	if(cas[cid].f_right != -1)
		if(left_of(cas[cid].f_right, cas[cid]))
			val = false;

	if(cas[cid].f_left != -1)
		if(!left_of(cas[cid].f_left, cas[cid]))
			val = false;

	vdl[cas[cid].pos].vvi = cas[cid].vis;

	return val;
}

bool Pmd::face_made_of(int fid, int v0, int v1, int v2)
{
	bool b0 = false;
	bool b1 = false; 
	bool b2 = false;
	
	int fiv[3];

	fiv[0] = findcurvi(fl[fid].v[0]);
	fiv[1] = findcurvi(fl[fid].v[1]);
	fiv[2] = findcurvi(fl[fid].v[2]);


	if((fid < 0) || (v0 < 0) || (v1 < 0) || (v2 < 0))
		sofex("indexing below zero");

	if( (v0 == fiv[0]) || (v0 == fiv[1]) || (v0 == fiv[2]) ) b0 = true;
	if( (v1 == fiv[0]) || (v1 == fiv[1]) || (v1 == fiv[2]) ) b1 = true;
	if( (v2 == fiv[0]) || (v2 == fiv[1]) || (v2 == fiv[2]) ) b2 = true;

	return (b0 && b1 && b2);
}

bool Pmd::face_includes_pair(int fid, int v0, int v1, int &other_one)
{
	bool b0, b1;

	int fiv[3];

	fiv[0] = findcurvi(fl[fid].v[0]);
	fiv[1] = findcurvi(fl[fid].v[1]);
	fiv[2] = findcurvi(fl[fid].v[2]);


	b0 = b1 = false;

	if((fid < 0) || (v0 < 0) || (v1 < 0))
		sofex("indexing below zero");

	if( (v0 == fiv[0]) || (v0 == fiv[1]) || (v0 == fiv[2]) ) b0 = true;
	if( (v1 == fiv[0]) || (v1 == fiv[1]) || (v1 == fiv[2]) ) b1 = true;

	return (b0 && b1);
}

bool Pmd::face_includes_vertex(int fid, int vid)
{
	for(int i = 0; i < 3; ++i)
		if(findcurvi(fl[fid].v[i]) == vid)
			return true;

	return false;
}

bool Pmd::ficofac(Pairdat &pd, vint &affac, Casdat &cd, sint &colpar)
{
	lint cdv, old_cdv;
	int thisone;
	lint closev;
	lint::iterator li;

	debug::DebugOutput dout("ficofac");


	// Initialize Casdat
	cd.cf[0][0] = cd.cf[0][1] = cd.cf[1][0] = cd.cf[1][1] = -1;
	cd.f_left = cd.f_right = -1;
	cd.fn[0] = cd.fn[1] = cd.fn[2] = cd.fn[3] = -1;


	int i;

	if( (findcurvi(pd.v[0]) != pd.v[0]) || (findcurvi(pd.v[1]) != pd.v[1]) ){
		cout << "EXCEPTION: ficofac!" << endl;
		cin.get();
	}

	// Find thisone
	
	for(li = vdl[pd.v[0]].pairs.begin(); li != vdl[pd.v[0]].pairs.end(); ++li)
		if(vpd[*li] == pd){
			thisone = *li;
			break;
		}

	// Finding common faces for pd.v[0] and pd.v[1]
	i = 0;
	lint tmpfac;
	tmpfac.clear();

	for(li = vdl[pd.v[0]].fl.begin(); li != vdl[pd.v[0]].fl.end(); ++li)
		if(std::find(vdl[pd.v[1]].fl.begin(), vdl[pd.v[1]].fl.end(), *li) != vdl[pd.v[1]].fl.end()){
			cd.cf[i][0] = *li;
			++i;
			tmpfac.push_back(*li);

			if(left_of(*li, cd)){
				cd.f_left = *li;
			} else {
				cd.f_right = *li;
			}
			

		}


	// Finding pd-close vertices
	for(li = tmpfac.begin();li != tmpfac.end(); ++li)
		for(i = 0; i < 3; ++i){
			if( (findcurvi(fl[*li].v[i]) != pd.v[0]) && (findcurvi(fl[*li].v[i]) != pd.v[1])){
				closev.push_back(findcurvi(fl[*li].v[i]));
				if(*li == cd.f_left)
					cd.v_left = findcurvi(fl[*li].v[i]);
				else
					cd.v_right = findcurvi(fl[*li].v[i]);
			}
		}

	int other_one = -1;

	// Setup cd.fn[]
	if(cd.f_left != -1)
		for(li = vdl[cd.v_left].fl.begin(); li != vdl[cd.v_left].fl.end(); ++li)
			if(face_includes_pair(*li, cd.pos, cd.v_left, other_one) && (*li != cd.f_left)){
				cd.fn[0] = *li;
				cd.vn[0] = other_one;
			} else  if(face_includes_pair(*li, cd.is, cd.v_left, other_one) && (*li != cd.f_left)){
				cd.fn[1] = *li;
				cd.vn[1] = other_one;
			}

	if(cd.f_right != -1)
		for(li = vdl[cd.v_right].fl.begin(); li != vdl[cd.v_right].fl.end(); ++li)
			if(face_includes_pair(*li, cd.pos, cd.v_right, other_one) && (*li != cd.f_right)){
				cd.fn[2] = *li;
				cd.vn[2] = other_one;
			} else  if(face_includes_pair(*li, cd.is, cd.v_right, other_one) && (*li != cd.f_right)){
				cd.fn[3] = *li;
				cd.vn[3] = other_one;
			}


	// Stop non-trivial contractions
	for(li = closev.begin(); li != closev.end(); ++li)
		if(ficocov(pd, *li, -1) >= 0)return true;

	// Colpar
	colpar.insert(thisone);
	for(li = vdl[cd.pos].pairs.begin(); li != vdl[cd.pos].pairs.end(); ++li)
		if( (othervertex(*li, cd.pos) == cd.v_left) || (othervertex(*li, cd.pos) == cd.v_right) )
			colpar.insert(*li);

		vint::iterator vi;
		
		// Affac must be updated
		
		for(li = vdl[pd.v[1]].fl.begin(); li != vdl[pd.v[1]].fl.end(); ++li)
			if(std::find(tmpfac.begin(), tmpfac.end(), *li) == tmpfac.end()){
				affac.push_back(*li);
			}

		for(vi = vdl[pd.v[1]].ofl.begin(); vi != vdl[pd.v[1]].ofl.end(); ++vi)
			if(std::find(tmpfac.begin(), tmpfac.end(), *vi) == tmpfac.end()){
				affac.push_back(*vi);
			}
		
	return false;
}

bool Pmd::face_neighbour(int fi1, int fi2)
{
	int i, j;
	int cnt = 0;

	if( (fi1 < 0) || (fi2 < 0) )return false;

	for(i = 0; i < 3; ++i)
		for(j = 0; j < 3; ++j)
			if( findcurvi(fl[fi1].v[i]) == findcurvi(fl[fi2].v[j]))
				++cnt;
	
	if(cnt > 1)
		return true;
	else
		return false;
}

void Pmd::print_cas(void)
{
	for(std::vector<Casdat>::iterator i = cas.begin(); i != cas.end(); ++i)cout << (*i) << "\n";
}

void Pmd::print_info(void)
{
	//cout << "\nMesh has" << af.size() << " active faces.\n";
	cout << "vector<CASDAT> size: " << cas.size() << "\n";
	cout << s.size() << " possible splits.\n";
	cout << c.size() << " possible contractions.\n";
}

void Pmd::quad(int id, Quadric &q)
{

	Quadric qt;

	qt.clear();
	q.clear();

	debug::DebugOutput dout("quad");

	Verdat cv = vdl[id];
	wsVertex3 wscv = vdl[id].position.p;

	wsVertex3 v1, v2, v3, kpk1, kpk2, r;
	int i1, i2, j1, j2, j3;
	lint edgevs;
	bool b1, b2;
	double penalty = 1.0;
	lint::iterator i;
	bool edgever = false;

	//cout << "Quadric for Vertex: " << id << " with attached faces:";
	//for(i = cv.fl.begin(); i != cv.fl.end();++i)cout << (*i) << ", ";
	//cout << "\n";

	//cout << "New Quadric Begins to Calculate...\n";
	
	// Edge-vertex penalty...

	
	for(i = cv.fl.begin(); i != cv.fl.end();++i){


		i1 = findcurvi(fl[*i].v[0]);
		i2 = findcurvi(fl[*i].v[1]);

		if(i1 == id)i1 = findcurvi(fl[*i].v[2]);
		if(i2 == id)i2 = findcurvi(fl[*i].v[2]);
		b1 = b2 = false;

		// i1 & i2 hold the "other ones"

		for(lint::iterator j = cv.fl.begin(); j != cv.fl.end();++j){
			if(i == j)continue;

			j1 = findcurvi(fl[*j].v[0]);
			j2 = findcurvi(fl[*j].v[1]);
			j3 = findcurvi(fl[*j].v[2]);

			if(!b1)if(i1 == j1 || i1 == j2 || i1 == j3) b1 = true;
			if(!b2)if(i2 == j1 || i2 == j2 || i2 == j3) b2 = true;
		}

		if(!b1){
			edgevs.push_back(i1);
			edgevs.push_back(i2);
		}
		if(!b2){
			edgevs.push_back(i2);
			edgevs.push_back(i1);
		}

	}

	if(edgevs.empty())
		penalty = 1.0;
	else {
		//cout << "Vertex: " << id << " and size of edgevs is now: " << edgevs.size() << endl;
		edgever = true;
		penalty = 100.0;
		//cout << "VERTEX " << id << "is an edge node!\n";
	}

	while(!edgevs.empty()){
		v1 = vdl[id].position.p;//cout << "#: " << id << ", ";
		v2 = vdl[edgevs.back()].position.p;// cout << edgevs.back() << ", ";
		edgevs.pop_back();
		v3 = vdl[edgevs.back()].position.p;// cout << edgevs.back() << "\n";
		edgevs.pop_back();

		//cout << v1 << v2 << v3 << "\n";

		kpk1 = v2 - v1;
		kpk2 = v3 - v1;

		kpk1 = crossp(kpk1, kpk2);

		
		r = crossp(kpk1, kpk2);


		// beräknar d-kompnenten
		double d = r.e[0]*(-wscv.e[0]) + r.e[1]*(-wscv.e[1]) + r.e[2]*(-wscv.e[2]);

		double abb = sqrt(r.e[0]*r.e[0] + r.e[1]*r.e[1] + r.e[2]*r.e[2]); //normalisera

		r.e[0] = r.e[0]/abb;
		r.e[1] = r.e[1]/abb;
		r.e[2] = r.e[2]/abb;
		d      = d/abb;
		
		// Sätter upp A-matrisen
		q.a.e[0][0] += r.e[0]*r.e[0]; q.a.e[0][1] += r.e[0]*r.e[1]; q.a.e[0][2] += r.e[0]*r.e[2];
		q.a.e[1][0] += r.e[1]*r.e[0]; q.a.e[1][1] += r.e[1]*r.e[1]; q.a.e[1][2] += r.e[1]*r.e[2];
		q.a.e[2][0] += r.e[2]*r.e[0]; q.a.e[2][1] += r.e[2]*r.e[1]; q.a.e[2][2] += r.e[2]*r.e[2];

		// Beräknar b-vektorn
		q.b.e[0] += r.e[0]*d; q.b.e[1] += r.e[1]*d; q.b.e[2] += r.e[2]*d;

		q.c += (d*d);
	
	}

	q*=penalty;
	
	// Okej, kant-verticer är kompenserade, fortsätter med vanliga...
	for(i = cv.fl.begin();i!=cv.fl.end();++i){
		// Tar fram aktuella verticer
		v1 = vdl[fl[*i].v[0]].position.p;
		v2 = vdl[fl[*i].v[1]].position.p;
		v3 = vdl[fl[*i].v[2]].position.p;		

		// Tar fram två polygon-vektorer
		kpk1 = v2 - v1;
		kpk2 = v3 - v1;

		// beräknar kryss-produkten
		r = crossp(kpk1, kpk2); 


		// beräknar d-kompnenten
		double d = r.e[0]*(-wscv.e[0]) + r.e[1]*(-wscv.e[1]) + r.e[2]*(-wscv.e[2]);

		//dout << "and the d:" << d << endl;

		double abb = sqrt(r.e[0]*r.e[0] + r.e[1]*r.e[1] + r.e[2]*r.e[2]); //normalisera

		r.e[0] = r.e[0]/abb;
		r.e[1] = r.e[1]/abb;
		r.e[2] = r.e[2]/abb;
		d      = d/abb;

		// Sätter upp A-matrisen
		q.a.e[0][0] += r.e[0]*r.e[0]; q.a.e[0][1] += r.e[0]*r.e[1]; q.a.e[0][2] += r.e[0]*r.e[2];
		q.a.e[1][0] += r.e[1]*r.e[0]; q.a.e[1][1] += r.e[1]*r.e[1]; q.a.e[1][2] += r.e[1]*r.e[2];
		q.a.e[2][0] += r.e[2]*r.e[0]; q.a.e[2][1] += r.e[2]*r.e[1]; q.a.e[2][2] += r.e[2]*r.e[2];

		// Beräknar b-vektorn
		q.b.e[0] += r.e[0]*d; q.b.e[1] += r.e[1]*d; q.b.e[2] += r.e[2]*d;

		// Sparar d-värdet 
		q.c += d*d;

		//cin.get();


	}

	//cout << "\n\nAnd the result: \n\n\n" <<q;
	//cin.get();

	

	//return q;
}

void Pmd::print_vertices(void)
{
	int j = 0;
	for(Vdatvec::iterator i = vdl.begin(); i != vdl.end(); ++i){
		cout << j 
			 << "@" 
			 << (*i).position
			 << " points to: " 
			 << (*i).vvi 
			 << " with faces #" << (*i).fl.size() << ": ";
		for(lint::iterator k = (*i).fl.begin(); k != (*i).fl.end(); ++k) cout << (*k) << ", ";
		cout << "\n";
		++j;
	}
}

void Pmd::print_faces(void)
{
	int j = 0;
	for(Facelist::iterator i = fl.begin(); i != fl.end(); ++i){
		cout << j << ": " << (*i) << '\n';
		++j;
	}
}

void Pmd::print_quads(void)
{
	Quadric q;
	for(int i = 0; i < vdl.size(); ++i){
		quad(i, q);
		cout << "Vertex " << i  << ": " << q << "\n";
	}
}

bool Pmd::structural_singularity(Pairdat p)
{
	return ((vdl[p.v[0]].fl.size() < 2) || (vdl[p.v[1]].fl.size() < 2));
}

bool Pmd::cas_within_view(int cid)
{
	wsVertex3 QP = vdl[findcurvi(cas[cid].pos)].position.p - cam.eye;


	return (within_view(findcurvi(cas[cid].pos)) || within_view(findcurvi(cas[cid].is))) &&
		   (QP.len() < 3000);
}

bool Pmd::within_view(int vid)
{
	wsVertex3 QP = vdl[vid].position.p - cam.eye;

	double scalarp_QP_cam_e = scalarp(QP, cam_e);

	if(scalarp_QP_cam_e < 0)return false;

	wsVertex3 QR = scalarp_QP_cam_e * cam_e;

	double vid_len = sqrt( QP.len_2() - QR.len_2() );

	double allowed_len = QR.len() * tan_max_cam_angle;

	return vid_len < allowed_len;
}

double Pmd::alpha_err_split(int cid)
{
	wsVertex3 y = cam.eye - vdl[cas[cid].vpos].position.p;

	wsVertex3 x = vdl[cas[cid].vpos].position.p - vdl[cas[cid].vis].position.p;

	return x.len() / (y.len()*y.len()*y.len());
}

double Pmd::alpha_err_cont(int cid)
{
	int vis = findcurvi(cas[cid].is);
	int vpos = findcurvi(cas[cid].pos);

	wsVertex3 y = cam.eye - vdl[vpos].position.p;

	wsVertex3 x = vdl[vpos].position.p - vdl[vis].position.p;

	//cout << "alpha_err_cont (x/y/v):" << endl << x.len() << endl << y.len() << endl << (x.len() / (y.len()*y.len()*y.len())) << endl; cin.get();

	return x.len() / (y.len()*y.len()*y.len());
}


bool Pmd::spe_cont_ok(int cid)
{
	return alpha_err_cont(cid) < alpha_threshold;
}

bool Pmd::spe_split_ok(int cid)
{
	return alpha_err_split(cid) > alpha_threshold;
}

inline int Pmd::findcurvi(int vid)
{
	while(vdl[vid].vvi != vid) vid = vdl[vid].vvi;
	return vdl[vid].vvi;
}

void Pmd::addv(Vertex v)
{
	Verdat nv(v);
	nv.vvi = vdl.size();
	vdl.push_back(nv);

	if(nv.position.p.x > max_x)max_x = nv.position.p.x;
	if(nv.position.p.x < min_x)min_x = nv.position.p.x;
	if(nv.position.p.y > max_y)max_y = nv.position.p.y;
	if(nv.position.p.y < min_y)min_y = nv.position.p.y;
	if(nv.position.p.z > max_z)max_z = nv.position.p.z;
	if(nv.position.p.z < min_z)min_z = nv.position.p.z;

}

void Pmd::addv(double a, double b, double c)
{
	Verdat nv;
	nv.position.p.x = a;
	nv.position.p.y = b;
	nv.position.p.z = c;
	nv.vvi = vdl.size();
	vdl.push_back(nv);

	if(a > max_x)max_x = a;
	if(a < min_x)min_x = a;
	if(b > max_y)max_y = b;
	if(b < min_y)min_y = b;
	if(c > max_z)max_z = c;
	if(c < min_z)min_z = c;
}


void Pmd::addf(int a, int b, int c)
{

	Facedat nf(a,b,c);
	fl.push_back(nf);
	
	int nfi = fl.size() - 1;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
	//Lagra för vertex att den nya triangeln är associerad med var och en
	vdl[a].fl.push_back(nfi);
	vdl[b].fl.push_back(nfi);
	vdl[c].fl.push_back(nfi);

	vdl[a].ofl.push_back(nfi);
	vdl[b].ofl.push_back(nfi);
	vdl[c].ofl.push_back(nfi);

	++poly_count;
	++nr_of_polys;
}

void Pmd::set_alpha_threshold(double val)
{
	alpha_threshold = val;
}

void Pmd::set_num_con_lim(double val)
{
	num_con_limit = val;
}

void Pmd::set_cam(OldCamera nc)
{
	cam = nc;

	tan_max_cam_angle = tan((3.14/180.0)*(max(cam.horiz_fov, cam.vert_fov)/2.0));
	Vector v = cam.center - cam.eye;

	cam_e.e[0] = v.x;
	cam_e.e[1] = v.y;
	cam_e.e[2] = v.z;
	cam_e.normalize();
}

bool Pmd::tri_collapse(int fid){

	if(fid < 0)return true;

	int a = findcurvi(fl[fid].v[0]);
	int b = findcurvi(fl[fid].v[1]);
	int c = findcurvi(fl[fid].v[2]);

	return (a == b || a == c || b == c);

}

bool Pmd::contract(int cid)
{
	int vpos = findcurvi(cas[cid].pos);
	int vis  = cas[cid].is;

	cas[cid].vis = vis;
	cas[cid].vpos = vpos;

	// Update poly_count
	poly_count-=2;

	// Make contraction
	vdl[vpos].vvi = vis;

	// update active faces
	remove_active_faces(cid);

	// No longer ecol
	cas[cid].split = true;

	nr_of_splits++;
	nr_of_conts--;

	return false;

}


bool Pmd::split(int cid)
{
	int vpos = cas[cid].vpos;

	// Update poly_count
	poly_count+=2;

	// Make split
	vdl[vpos].vvi = vpos;

	// Update faces and progmesh
	add_active_faces(cid);

	// No longer a split
	cas[cid].split = false;

	nr_of_splits--;
	nr_of_conts++;

	return false;
}



bool Pmd::remove_active_faces(int cid)
{
	

	//Remove from the progmesh
	if(cas[cid].f_left != -1){
	
		fl[cas[cid].f_left].active = false;

		fl[progfac.back()].pfi = fl[cas[cid].f_left].pfi;
		progfac[fl[cas[cid].f_left].pfi] = progfac.back();

		fl[progfac.back()].pmi = fl[cas[cid].f_left].pmi;
		progmesh[fl[cas[cid].f_left].pmi + 2] = progmesh.back();//[progmesh.size() - 3];
		progmesh.pop_back();
		progmesh[fl[cas[cid].f_left].pmi + 1] = progmesh.back();//progmesh[progmesh.size() - 2];
		progmesh.pop_back();
		progmesh[fl[cas[cid].f_left].pmi + 0] = progmesh.back();//progmesh[progmesh.size() - 1];
		progmesh.pop_back();
		
		progfac.pop_back();
	}	
 

	if(cas[cid].f_right != -1){

		fl[cas[cid].f_right].active = false;

		fl[progfac.back()].pfi = fl[cas[cid].f_right].pfi;
		progfac[fl[cas[cid].f_right].pfi] = progfac.back();
		
		fl[progfac.back()].pmi = fl[cas[cid].f_right].pmi;
		progmesh[fl[cas[cid].f_right].pmi + 2] = progmesh.back();//[progmesh.size() - 3];
		progmesh.pop_back();
		progmesh[fl[cas[cid].f_right].pmi + 1] = progmesh.back();//progmesh[progmesh.size() - 2];
		progmesh.pop_back();
		progmesh[fl[cas[cid].f_right].pmi + 0] = progmesh.back();//progmesh[progmesh.size() - 1];
		progmesh.pop_back();
		
		progfac.pop_back();
	}

	vint::iterator vi;

	for(vi = cas[cid].af.begin();vi != cas[cid].af.end(); ++vi){
		if(fl[*vi].active){
			progmesh[fl[*vi].pmi + 0] = findcurvi(fl[*vi].v[0]);
			progmesh[fl[*vi].pmi + 1] = findcurvi(fl[*vi].v[1]);
			progmesh[fl[*vi].pmi + 2] = findcurvi(fl[*vi].v[2]);
		}
	}
	


	return false;
}

bool Pmd::add_active_faces(int cid)
{

	
	if(cas[cid].f_left != -1){
		fl[cas[cid].f_left].active = true;
		fl[cas[cid].f_left].pfi = progfac.size();
		progfac.push_back(cas[cid].f_left);
		
		fl[cas[cid].f_left].pmi = progmesh.size();
		progmesh.push_back(findcurvi(fl[cas[cid].f_left].v[0]));
		progmesh.push_back(findcurvi(fl[cas[cid].f_left].v[1]));
		progmesh.push_back(findcurvi(fl[cas[cid].f_left].v[2]));
		
	}

	if(cas[cid].f_right != -1){
		fl[cas[cid].f_right].active = true;
		fl[cas[cid].f_right].pfi = progfac.size();
		progfac.push_back(cas[cid].f_right);
		
		fl[cas[cid].f_right].pmi = progmesh.size();
		progmesh.push_back(findcurvi(fl[cas[cid].f_right].v[0]));
		progmesh.push_back(findcurvi(fl[cas[cid].f_right].v[1]));
		progmesh.push_back(findcurvi(fl[cas[cid].f_right].v[2]));
		
	}


	vint::iterator vi;

	for(vi = cas[cid].af.begin();vi != cas[cid].af.end(); ++vi){
		if(fl[*vi].active){
			progmesh[fl[*vi].pmi + 0] = findcurvi(fl[*vi].v[0]);
			progmesh[fl[*vi].pmi + 1] = findcurvi(fl[*vi].v[1]);
			progmesh[fl[*vi].pmi + 2] = findcurvi(fl[*vi].v[2]);
		}
	}

	return false;
}

bool Pmd::validc(int cont)
{
	// Verify contraction-vertices existance

	int tmp;

	int vpos = findcurvi(cas[cont].pos);
	int vis  = findcurvi(cas[cont].is);

	if( 

	// f_left, f_right
	
		(
			(cas[cont].f_left == -1) ||
			(
				(face_includes_pair(cas[cont].f_left, vpos, vis, tmp)) &&
				( 
					(cas[cont].fn[0] == -1 ) ||
					faces_aligned(cas[cont].fn[0], cas[cont].f_left)
				) &&
				(
					(cas[cont].fn[1] == -1 ) ||
					faces_aligned(cas[cont].fn[1], cas[cont].f_left)
				)
			)
		) &&

		(
			(cas[cont].f_right == -1) ||
			(
				(face_includes_pair(cas[cont].f_right, vpos, vis, tmp)) &&
				( 
					(cas[cont].fn[2] == -1 ) ||
					faces_aligned(cas[cont].fn[2], cas[cont].f_right)
				) &&
				(
					(cas[cont].fn[3] == -1 ) ||
					faces_aligned(cas[cont].fn[3], cas[cont].f_right)
				)
			)
		) &&


		(
			// In Viewport?
			(!within_view(vpos) && !within_view(vis)) ||
			// Screen-space error?
			spe_cont_ok(cont)
		
		) &&
		
		// fn Active?
		((cas[cont].fn[0] == -1) || ((fl[cas[cont].fn[0]].active) && (!tri_collapse(cas[cont].fn[0]) ) ) ) &&
		((cas[cont].fn[1] == -1) || ((fl[cas[cont].fn[1]].active) && (!tri_collapse(cas[cont].fn[1]) ) ) ) &&
		((cas[cont].fn[2] == -1) || ((fl[cas[cont].fn[2]].active) && (!tri_collapse(cas[cont].fn[2]) ) ) ) &&
		((cas[cont].fn[3] == -1) || ((fl[cas[cont].fn[3]].active) && (!tri_collapse(cas[cont].fn[3]) ) ) ) 

	  )
		return true;


	return false;
}

bool Pmd::valids(int cont)
{
	// Verify split-vertices existance
	int vpos = findcurvi(cas[cont].pos);
	int vis  = findcurvi(cas[cont].is);

	if(
	
		(
			(cas[cont].fn[0] == -1) || 
			((fl[cas[cont].fn[0]].active) /*&& face_includes_vertex(cas[cont].fn[0], cas[cont].is)*/ ) 
		) &&
		(
			(cas[cont].fn[1] == -1) || 
			((fl[cas[cont].fn[1]].active) /*&& face_includes_vertex(cas[cont].fn[1], cas[cont].is)*/ ) 
		) &&
		(
			(cas[cont].fn[2] == -1) || 
			((fl[cas[cont].fn[2]].active) /*&& face_includes_vertex(cas[cont].fn[2], cas[cont].is)*/ ) 
		) &&
		(
			(cas[cont].fn[3] == -1) || 
			((fl[cas[cont].fn[3]].active) /*&& face_includes_vertex(cas[cont].fn[3], cas[cont].is)*/ ) 
		) &&
	

		(within_view(vis) || within_view(vpos)) &&

	// Screen-space error

		spe_split_ok(cont)
	  )
		return true;
	
	return false;
}

void Pmd::makeprog(void)
{
	debug::DebugOutput dout("pm::makeprog");
	
	/**************************************************************************/
	/* Throw all pairs (edges) on the heap                                    */
	/**************************************************************************/

	Treap treap;	// The most important thing about this procedure

	Facelist::iterator fi;	// Iterates through the faces of the mesh
	Vpd tmp_vpd;		// Vector containing the pairs to be throwed on the heap;
	Pairdat pd, tpd;	// Pairinfo (temporary)
	int i, vi1, vi2;
	Casdat cd;
	
	// Identify pairs
	tmp_vpd.clear();
	for(fi = fl.begin(); fi != fl.end(); ++fi)
		for(i = 0; i < 3; ++i){
			vi1 = i;
			vi2 = (i+1) % 3;
			pd.v[0] = (*fi).v[vi1];
			pd.v[1] = (*fi).v[vi2];
			pd.cost = 0.0;
			pd.fl.clear();

			tmp_vpd.push_back(pd);
		}

	dout << "Size of tmp_vpd: " << tmp_vpd.size() << endl;

	Pairdat vpdi;		// Iterates through the vpd
	Quadric q0, q1;			// Quadrics to calculate costs
	double cost0, cost1;	// Temporary costs
	lint::iterator li, lj;	// Iterates through indexed lists
	int dbgc = 0;

	double maximum_cost = 0.0;


	// Calculate costs and throw on heap
	while(!tmp_vpd.empty()){//vpdi = tmp_vpd.begin(); vpdi != tmp_vpd.end(); ++vpdi){

		vpdi = tmp_vpd.back();
		tmp_vpd.erase(tmp_vpd.end() - 1);

		if(treap.find(vpdi))continue;

		if(++dbgc % 500 ==0){
			cout << "\r                                                                          ";
			cout << "\rLeft to throw: " << tmp_vpd.size();
		}
		

		// Construct the necessary quadrics
		quad(vpdi.v[0], q0);
		quad(vpdi.v[1], q1);

		// Cost of contracting v1 -> v0
		cost0 = q1.cost((vpdi).v[0], vdl);

		// Cost of contracting v0 -> v1
		cost1 = q0.cost((vpdi).v[1], vdl);

		if(sqrt(cost0*cost0 + cost1*cost1) < 0.0001){
			cost0 = cost1 = 0.0;
			if(vpdi.v[0] > vpdi.v[1])
				std::swap(vpdi.v[0], vpdi.v[1]);
		} else if(cost0 >  cost1){
		// Always contract v1 -> v0
			std::swap((vpdi).v[0], (vpdi).v[1]);
			std::swap(cost0, cost1);
		} 

		if(cost0 < 0)continue;// sofex("Negative Quadric Value.");

		if(cost0 > maximum_cost)maximum_cost = cost0;

		vpdi.cost = cost0;


		// Insert contraction in treap and update vertex information
		
		if(!treap.insert(vpdi)){

				(vpdi).ov[0] = (vpdi).v[0];
				(vpdi).ov[1] = (vpdi).v[1];

				vpd.push_back(vpdi);
				vdl[(vpdi).v[0]].pairs.push_back(vpd.size()-1);
				vdl[(vpdi).v[1]].pairs.push_back(vpd.size()-1);
				

		}
		

	}

	cout << endl;

	tmp_vpd.clear();

	dout << "Size of treap: " << treap.size() << endl;
	dout << "Size of vpd: " << vpd.size() << endl;

	/**************************************************************************/
	/* Pop the top element of the heap and contract accordingly               */
	/**************************************************************************/

	Verdat * vd0;
	Verdat * vd1;
	lint::iterator hit0, hit1;
	lint rem;

	sint::iterator si;

	lint cf;
	sint colpar;

	Pairdat sf;

	sf.v[0] = 12945;
	sf.v[1] = 12943;
	sf.cost = 0;

	int dcount = 0;

	int rejected = 0;
	int accepted = 0;



	int treap_max_size = treap.size();
	int delta_treap = treap_max_size / 20;

	while(!treap.empty()){
		pd = treap.pop();

		++dcount;

		if(pd.cost > num_con_limit)continue;

		// show progress in console
		if(dcount % 100 == 0){
			cout << "\rpmd::makeprog2::progress: |";
			for(i = 0; i < (treap_max_size - treap.size()); i += delta_treap)
				cout << "*";
			for(;i < treap_max_size; i+=delta_treap)
				cout << " ";
			cout << "| r/a: " << rejected << " / " << accepted;
		}

		
		if(pd.v[0] != findcurvi(pd.v[0])) sofex("Unupdated contractions at v0");

		if(pd.v[1] != findcurvi(pd.v[1])) sofex("Unupdated contractions at v1");

		if(findcurvi(pd.v[1]) == findcurvi(pd.v[0])) sofex("Nonsense contraction!");
		colpar.clear();
//		cd.fl.clear();
		cd.af.clear();
		cd.pos = pd.v[1];
		cd.is = pd.v[0];

		if(ficofac(pd, cd.af, cd, colpar)){
			maximum_cost+= 100.0;
			pd.cost = maximum_cost;
			treap.insert(pd);
			++rejected;
			continue;
		} else {
			++accepted;
		}
					
		rem.clear();
		
		// Remove the collapsing contractions

		for(si = colpar.begin(); si != colpar.end(); ++si){

			vd0 = &vdl[vpd[*si].v[0]];
			vd1 = &vdl[vpd[*si].v[1]];
			hit0 = find(vd0->pairs.begin(), vd0->pairs.end(), *si);
			hit1 = find(vd1->pairs.begin(), vd1->pairs.end(), *si);
			if(hit0 == vd0->pairs.end()) dout << "vd0 Failure" << endl;
			if(hit1 == vd1->pairs.end()) dout << "vd1 Failure" << endl;
			vd0->pairs.erase(hit0);
			vd1->pairs.erase(hit1);
			if(!(vpd[*si] == pd)){
				if(treap.remove(vpd[*si]))
					sofex("failed to remove collapsing contraction");
			}
		}

		// Remove the collapsing faces
		rem.clear();

		int vv;

		if(cd.f_left != -1)
			for(i = 0; i < 3; ++i){
				vv = findcurvi(fl[cd.f_left].v[i]);
				hit0 = std::find(vdl[vv].fl.begin(), vdl[vv].fl.end(), cd.f_left);
				if(hit0 != vdl[vv].fl.end())
					vdl[vv].fl.erase(hit0);
			}

		if(cd.f_right != -1)
			for(i = 0; i < 3; ++i){
				vv = findcurvi(fl[cd.f_right].v[i]);
				hit0 = std::find(vdl[vv].fl.begin(), vdl[vv].fl.end(), cd.f_right);

				if(hit0 != vdl[vv].fl.end())
					vdl[vv].fl.erase(hit0);
			}

		// Make the contraction and store it on the CAS
		vdl[pd.v[1]].vvi = pd.v[0];
		cd.pos = pd.v[1];	
		cd.is  = pd.v[0];	
		cd.split = false;	// Set all operations to contractions by default
		cas.push_back(cd);

		// The pairs of v1 now belongs to v0
		for(li = vdl[pd.v[1]].pairs.begin(); li != vdl[pd.v[1]].pairs.end(); ++li)
			if(std::find(vdl[pd.v[0]].pairs.begin(), vdl[pd.v[0]].pairs.end(), *li) == vdl[pd.v[0]].pairs.end()) {
				vdl[pd.v[0]].pairs.push_back(*li);
			} else {
				dout << "pair already at v0! (" << *li << ") / " << vpd[*li] << endl;
				cin.get();
			}

		// v1 no longer contains any contractions, since they all belong to v0 now!
		vdl[pd.v[1]].pairs.clear();

		// Faces for v1 now belongs to v0
		for(li = vdl[pd.v[1]].fl.begin(); li != vdl[pd.v[1]].fl.end(); ++li)
			if(std::find(vdl[pd.v[0]].fl.begin(), vdl[pd.v[0]].fl.end(), *li) == vdl[pd.v[0]].fl.end()) {
				vdl[pd.v[0]].fl.push_back(*li);
			} else {
				dout << "face already at v0! (" << *li << ")" << endl;
				cin.get();
			}

		// Find faces with neighbouring collapses
		lint afl;
		afl.clear();
		for(li = vdl[pd.v[0]].fl.begin(); li != vdl[pd.v[0]].fl.end(); ++li)
			afl.push_back(*li);

		// Find vertices with changing Quadrics
		lint avl;
		avl.clear();
		for(li = afl.begin(); li != afl.end(); ++li)
			for(i = 0; i < 3; ++i)
				if(std::find(avl.begin(), avl.end(), fl[*li].v[i]) == avl.end())
					avl.push_back(findcurvi(fl[*li].v[i]));


		// Find pairs that contain quadric-changing vertices
		lint apl;
		apl.clear();
		for(li = avl.begin(); li != avl.end(); ++li)
			for(lj = vdl[*li].pairs.begin(); lj != vdl[*li].pairs.end(); ++ lj)
				if(std::find(apl.begin(), apl.end(), *lj) == apl.end())
					apl.push_back(*lj);

	/**************************************************************************/
	/* Update all affected pairs on the heap                                  */
	/**************************************************************************/
	
		lint::iterator cp;
		bool mod_con;
		for(cp = apl.begin(); cp != apl.end(); ++cp){

			mod_con = false;

			// Does the contraction contain the v[1] node?
			// Then it will be replaced with the corresponding new contraction
			

			if(vpd[*cp].v[0] == pd.v[1]){
				tpd = vpd[*cp];
				mod_con = true;
				vpd[*cp].v[0] = pd.v[0];
			}

			if(vpd[*cp].v[1] == pd.v[1]){
				if(!mod_con){
					tpd = vpd[*cp];
					mod_con = true;
				}
				vpd[*cp].v[1] = pd.v[0];

			
			}

			// Construct the necessary quadrics
			quad(vpd[*cp].v[0], q0);
			quad(vpd[*cp].v[1], q1);
	
			// Cost of contracting v1 -> v0
			cost0 = q1.cost(vpd[*cp].v[0], vdl);

			// Cost of contracting v0 -> v1
			cost1 = q0.cost(vpd[*cp].v[1], vdl);
	
			// Always contract v1 -> v0
			if(cost0 >  cost1){
				if(!mod_con){
					tpd = vpd[*cp];
					mod_con = true;
				}
				std::swap(vpd[*cp].v[0], vpd[*cp].v[1]);
				std::swap(vpd[*cp].ov[0], vpd[*cp].ov[1]);
				std::swap(cost0, cost1);
			} 

			vpd[*cp].cost = cost0;

			if(sqrt(cost0*cost0) < 0.0001)
				cost0 = 0.0;

			if(cost0 < 0){
				dout << "I got a cost here that is: " << cost0 << endl;
				cin.get();
			}

			//Enter element into treap
			
			if(mod_con){
				treap.remove(tpd);
				if(treap.insert(vpd[*cp])){
					cout << "treap.insert() - already there: " << endl;
					cout << *cp << " - " << vpd[*cp] << endl;
					cout << "because of: " << endl;
					cout << pd;
					cin.get();
				}
			} else {
				treap.update(vpd[*cp]);
			}

		} //End of updating-heap-loop

		if(pd == sf)cin.get();
		
	
	} // End of while(!treap.empty()) loop

	cout << endl;
	dout << "Restoring Mesh Configuration..." << endl;

	for(i = 0; i < vdl.size(); ++i)
		vdl[i].vvi = i;

	dout << "Setting all faces as active..." << endl;

	for(i = 0; i < fl.size(); ++i)
		fl[i].active = true;

	dout << "Adding meshdata to the progmesh-struct..." << endl;

	for(i = 0; i < fl.size(); ++i){
		fl[i].pmi = progmesh.size();
		progmesh.push_back(fl[i].v[0]);
		progmesh.push_back(fl[i].v[1]);
		progmesh.push_back(fl[i].v[2]);
	}

	dout << "Adding facedata to the progfac-struct..." << endl;
	for(i = 0; i < fl.size(); ++i){
		fl[i].pfi = progfac.size();
		progfac.push_back(i);
	}

	// Verify progmesh and progfac integrity
	for(i = 0; i < fl.size(); ++i)
		if(fl[i].active){
			if(fl[i].pmi != (3*fl[i].pfi)){
				cout << "pmi / 3*pfi: " << fl[i].pmi << " / " << 3*fl[i].pfi << endl;
				cin.get();
			}
		}


	dout << "Size of CAS is now: " << cas.size() << endl;
	
	nr_of_splits = 0;
	nr_of_conts = cas.size();
	cas_index = 0;
	
	mesh_made = true;

	dout << "Creating CAS-grid." << endl;

	update_cas_grid();

	dout << "Done!" << endl;

	return;
}

bool Pmd::singlecontract(void)
{
	if(cas_index < cas.size()){
		contract(cas_index);
		++cas_index;
	}



/*
	for(int i = 0; i < cas.size(); ++i)
		if(!cas[i].split){
			contract(i);
			return false;
		}
*/
	return true;


}

bool Pmd::singlesplit(void)
{

	if(cas_index > 0){
		split(cas_index);
		--cas_index;
	}
/*
	for(int i = 0; i < cas.size(); ++i)
		if(cas[i].split){
			split(i);
			return false;
		}
*/
	return true;
}


void Pmd::set_poly_count(float percent)
{

	debug::DebugOutput dout("setpolycount");


	
	if(poly_count < (nr_of_polys * (percent*0.01)))
		while(poly_count < (nr_of_polys * (percent*0.01)))
			singlesplit();
	else
		while((nr_of_conts > 10) && (poly_count > (nr_of_polys * (percent*0.01)))){
				singlecontract();
		}
}

void Pmd::set_number_of_polys(int cnt)
{
	if(cas.empty())sofex("Dah! You've gotta run 'makeprog' first...");

	while((poly_count < cnt) || (cas_index < 1))singlesplit();
	while((poly_count > cnt) || (cas_index >= cas.size()))singlecontract();
}

void Pmd::update_cas_grid(void)
{

	const int delta_cas = 10;
	int i;
	double x, z;

	double dx = (max_x - min_x) / delta_cas;
	double dz = (max_z - min_z) / delta_cas;

	int cx, cz;
	

	for(i = 0; i < cas.size(); ++i){

		x = vdl[cas[i].pos].position.p.x - min_x;
		z = vdl[cas[i].pos].position.p.z - min_z;

		cx = static_cast<int>(floor(x / dx));
		cz = static_cast<int>(floor(z / dz));

		if(cx > delta_cas - 1)cx = delta_cas - 1;
		if(cz > delta_cas - 1)cz = delta_cas - 1;
		/*
		cout << "min_x/min_z" << min_x << " / " << min_z << endl;
		cout << "dx/dz :" << dx << " / " << dz << endl;
		cout << "x/z: " << x << " / " << z << endl;
		cout << "cx/cz: " << cx << " / " << cz << endl;
		*/
		//cin.get();

		cas_grid[cx][cz].push_back(i);

	}
/*
	for(i = 0; i < delta_cas; ++i)
		for(int j = 0; j < delta_cas; ++j)
			cout << i << ", " << j << ": " << cas_grid[i][j].size() << endl;

	cin.get();
	*/

		
}

void Pmd::update_fast(void)
{
	int i, j;
	const int delta_cas = 10;
	double dx = (max_x - min_y) / delta_cas;
	double dz = (max_z - min_z) / delta_cas;

	vint::iterator vi;

	if(mesh_made)
		for(i=0;i<delta_cas;++i)
			for(j=0;j<delta_cas;++j)
				if(!cas_grid[i][j].empty())
					if(cas_within_view(cas_grid[i][j].front())){
						for(vi = cas_grid[i][j].begin(); vi != cas_grid[i][j].end(); ++vi)
							if(!cas[*vi].split){
								if(validc(*vi)){
									contract(*vi);
								}
							} else {
								if(valids(*vi)){
									split(*vi);
								}
							}

//							++dc;
					}

//	cout << "used: " << dc << " squares." << endl;

}
		
			

void Pmd::update(void)
{

	static int step_counter = 0;
	static int multistep = 1;
	int delta = cas.size() / update_steps;
	int upper_limit;

	if(mesh_made){

		upper_limit = (multistep == update_steps)?cas.size():delta*multistep;

		/*
	cout << "multistep: " << multistep << endl;
	cout << "step_counter: " << step_counter << endl;
	cout << "delta: " << delta << endl;
	cout << "cas.size(); " << cas.size() << endl;
	cout << "upper_limit: " << upper_limit << endl;
		*/


		while(step_counter < upper_limit){

			if(!cas[step_counter].split){
				if(validc(step_counter)){
					contract(step_counter);
				}
			} else {
				if(valids(step_counter)){
					split(step_counter);
				}
			}
			++step_counter;

		}

		++multistep;
		if(multistep > update_steps){
			multistep = 1;
			step_counter = 0;
		}
	}
/*
	cout << "progmesh.size(): " << progmesh.size() << endl;
	cout << "gone thru: " << dc << endl;
	cout << " ----------------------------- " << endl;

	for(i = 0;i < cas.size();++i){
		if(!cas[i].split){
			if(validc(i)){
				contract(i);
			}
		} else {
			if(valids(i)){
				split(i);
			}
		}
	}
*/
	
	if(cas.size() > 0)
	
	for(int i = 0; i < fl.size(); ++i)
		if(fl[i].active){
			progmesh[fl[i].pmi + 0] = findcurvi(fl[i].v[0]);
			progmesh[fl[i].pmi + 1] = findcurvi(fl[i].v[1]);
			progmesh[fl[i].pmi + 2] = findcurvi(fl[i].v[2]);
		}
	
}


Vertex Pmd::vpos(int vid)
{
	return vdl[vdl[vid].vvi].position;
}

Facedat Pmd::lastface(void)
{
	return *(fl.end()-1);
}

Vertex Pmd::lastvertex(void)
{
	return (*(vdl.end()-1)).position;
}


void Pmd::report(void)			//Report stats about the mesh
{
	debug::DebugOutput dout;//("test progressive mesh",0);


	if(!ready_to_update){
		dout << "Progressive Mesh Reporting - Not Ready" << endl;
		return;
	}

	dout << "PM Reporting - #contractions: " << nr_of_conts << ", #splits: " << nr_of_splits << endl;
}

}
}
}
