/* $Id: resmgr.h,v 1.10 2002/01/26 20:46:02 peter Exp $ */

#ifndef REAPER_MISC_RESMGR_H
#define REAPER_MISC_RESMGR_H

#include <vector>

namespace reaper {
namespace misc {


template<class T, int N>
class ResourceMgr
{
	std::pair<int, typename T::T> res[N];
	int idx[N];
	int next_free, max;
	bool is_init, init_res;
public:
	T data;

	ResourceMgr() : next_free(0), max(N), is_init(false), init_res(false)
	{
	}
	~ResourceMgr() { }
	bool init()
	{
		if (is_init)
			return init_res;
		is_init = true;
		for (int i = 0; i < max; ++i) {
			idx[i] = i;
			res[i].first = i;
			if (! data.make(res[i].second)) {
				max = i;
				return false;
			}
		}
		return (init_res = true);
	}
	void release_all()
	{
		for (int i = 0; i < max; ++i)
			data.release(res[i].second);
	}
	int alloc()
	{
		if (next_free == max)
			return -1;
		int id = res[next_free++].first;
		return id;
	}
	void release(int rel_l)
	{
		int free_r = --next_free;
		int free_l = res[free_r].first;
		int rel_r = idx[rel_l];

		std::swap(idx[rel_l], idx[free_l]);
		std::swap(res[rel_r], res[free_r]);
	}
	typename T::T& get(int i)
	{
		return res[idx[i]].second;
	}
	int size() const { return next_free; }
};


}
}

#endif
