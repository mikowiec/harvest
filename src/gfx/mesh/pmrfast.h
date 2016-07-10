#ifndef REAPER_MESH_PMRTSFAST_H
#define REAPER_MESH_PMRTSFAST_H

#include "pmrealtimesys.h"

namespace reaper 
{
namespace gfx
{
namespace mesh
{

class PMRFast : public PMRealTimeSys
{
private:

	void force_vsplit(moli::iterator& li);
	void ecol(int vi, moli::iterator& li);
	void backtrace_ecol(int vi, moli::iterator& li);
	void vsplit(int vi, moli::iterator* li);


public:
	PMRFast(SRMesh& srmesh):PMRealTimeSys(srmesh){};

	void refine();

	void spec_report(bool verbose){};


};
		

}
}
}

#endif
