
#include "misc/test_main.h"


#include "misc/resmgr.h"
#include "misc/sequence.h"

using namespace std;
using namespace misc;

int gen = 'A';

struct Res {
	static const int N;
	typedef char T;
	static bool make(char& i) {
		i = gen++;
		return true;
	}

};

const int Res::N = 3;

ResourceMgr<Res> res;

struct Frob {
	ostream& os;
	Frob(ostream& o) : os(o) { }
	int operator()(const std::pair<int,char>& p) {
		os << "(" << p.first << "," << p.second << ")";
		return 0;
	}
};

ostream& operator<<(ostream& os, const ResourceMgr<Res>& rm)
{
	for_each(cseq(rm.res), Frob(os));
	os << "   ";
	copy(cseq(rm.idx), ostream_iterator<int>(debug::std_debug, " "));
	return os;
}
ostream& operator<<(ostream& os, pair<int,char> p) {
		os << "(" << p.first << "," << p.second << ")";
	return os;
}

int test_main()
{
	res.init();
	int i0 = res.alloc();
	debug::std_debug << res << '\n';
	int i1 = res.alloc();
	debug::std_debug << res << '\n';
	int i2 = res.alloc();
	debug::std_debug << res << '\n';
	int i3 = res.alloc();
	debug::std_debug << res << '\n';
	debug::std_debug << make_pair(i0, res.get(i0)) << ' '
	                 << make_pair(i1, res.get(i1)) << ' '
	                 << make_pair(i2, res.get(i2)) << '\n';
			 
	res.release(i1);
	debug::std_debug << res << '\n';
	int i6 = res.alloc();
	debug::std_debug << res << ' ' << i6 << '\n';
	return 0;
}

