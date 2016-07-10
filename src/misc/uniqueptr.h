#ifndef REAPER_MISC_UNIQUEPTR_H
#define REAPER_MISC_UNIQUEPTR_H

#include <stdio.h>

namespace reaper {
namespace misc {


template<class T>
class UniquePtr
{
	struct I {
		T* ptr;
		int count;
		I() : ptr(0), count(0) { }
	};
	static I inst;
public:
	template<class A>
	static UniquePtr create(A a) {
		inst.ptr = new T(a);
		return UniquePtr();
	}
	static UniquePtr create() {
		inst.ptr = new T();
		return UniquePtr();
	}
	static void destroy() {
		delete inst.ptr;
		inst.ptr = 0;
	}
	UniquePtr() { UniquePtr<T>::inst.count++;  }

	~UniquePtr() { inst.count--; }

	T& operator*() { return *inst.ptr; }
	T* operator->() { return inst.ptr; }
	const T& operator*() const { return *inst.ptr; }
	const T* operator->() const { return inst.ptr; }

	int count() const { return inst.count; }
	bool valid() const { return inst.ptr != 0; }
};


}
}

#endif
