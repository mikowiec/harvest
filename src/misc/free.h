
#ifndef REAPER_MISC_FREE_H
#define REAPER_MISC_FREE_H


#include <map>

namespace reaper {
namespace misc {

namespace {
struct delete_it_t {
	template<class T, class A>
		void operator()(std::pair<A, T> p) const {
		delete p.second;
	}

	template<class T>
	void operator()(T* t) const {
		delete t;
	}
};

#ifdef MSVC
static delete_it_t delete_it;
#else
delete_it_t delete_it;
#endif
}

template<class T>
inline void zero_delete(T& t)
{
	if (t) {
		delete t;
		t = 0;
	}
}

}
}

#endif

