#include "os/compat.h"
#include "pairs.h"
#include <set>
#include <iostream>

namespace reaper 
{
namespace gfx
{
namespace mesh
{
	using std::cout;

bool Pairs::is_empty()
{
	return pairset.empty();
}

void Pairs::report(bool verbose)
{
	cout << "\n";
	cout << "-------------------\n";
	cout << "| Pairs reporting |" << "\n";
	cout << "-------------------" << "\n";
	cout << "Pair count:\t" << size() << "\n";

	if(verbose){
		std::set<Pair>::const_iterator si;
		for(si = pairset.begin(); si != pairset.end(); si++)
			cout << "(" << (*si).a << ", " << (*si).b << ")" << "\n";
	}

	cout << "\n";
}		

}
}
}
