
/* $Id: smartptr.h,v 1.27 2003/06/04 16:36:00 pstrand Exp $ */

#ifndef REAPER_MISC_SMARTPTR_H
#define REAPER_MISC_SMARTPTR_H

#include <typeinfo>

namespace reaper {
namespace misc {

struct Internal
{
	int* count;
	Internal() : count(0) { }
	void init(int* c) { count = c; *count = 1; }
	void kill() { count = (int*)0xdead; }
	int inc() { return ++(*count); }
	int dec() { return --(*count); }
};

struct External
{
	int* count;
	External() : count((int*)0xfeed) { }
	void init() {
		count = new int(1);
	}
	void kill() {
		if (count != (int*)0xfeed && count != (int*)0xdead)
			delete count;
		count = (int*)0xdead;
	}
	External(const External& e) : count(e.count) { }
	int inc() { return ++(*count); }
	int dec() { return --(*count); }
};

struct Normal {
	template<class T>
	static void dealloc(T* p) {
		delete p;
	}
};

struct Array {
	template<class T>
	static void dealloc(T* p) {
		delete [] p;
	}
};

template<class T>
class WeakPtr
{
	T* ptr;
public:
	WeakPtr() : ptr(0) { }
	WeakPtr(int) : ptr(0) { } // FIXME add check

	template<class U>
	WeakPtr(U* u) : ptr(u) { }

	template<class U>
	WeakPtr(const WeakPtr<U>& wp)
	{
		ptr = wp.get();
	}

	template<class U>
	WeakPtr& operator=(const WeakPtr<U>& wp)
	{
		ptr = wp.get();
		return *this;
	}


	WeakPtr& operator=(int zero)
	{
		if (zero != 0)
			throw "Don't even think about it!";
		ptr = 0;
		return *this;
	}


	const T* operator->() const { return ptr; }
	T* operator->() { return ptr; }

	bool operator==(const T* p) const {
		return ptr == p;
	}

	bool operator!=(const T* p) const {
		return ptr != p;
	}


	template<class U>
	bool operator==(const WeakPtr<U>& sp) const {
		return ptr == sp.get();
	}

	template<class U>
	bool operator!=(const WeakPtr<U>& sp) const {
		return ptr != sp.get();
	}
	T* get() const { return ptr; }
};



template<class T, class RefCountType = External, class DeleteWith = Normal>
class SmartPtr
{
	typedef RefCountType R;
	typedef DeleteWith D;
	RefCountType count;

	SmartPtr(T* p, const RefCountType& c) : ptr(p), count(c) { }
	void dealloc() {
		DeleteWith::dealloc(ptr);
		count.kill();
		ptr = 0;
	}

public:
	int get_count()
	{
		if (count.count == (int*)0xfeed)
			return -1;
		if (count.count == (int*)0xdead)
			return -2;
		return *count.count;
	}

	typedef WeakPtr<T> Weak;
	T* ptr;
	template<class U>
	void assign(const SmartPtr<U, R, D>& sp) {
		if (sp == 0) {
			ptr = 0;
			return;
		}
		ptr = sp.get();
		count = sp.getcount();
		count.inc();
	}
	SmartPtr() : ptr(0) { }

	SmartPtr(int n) : ptr(0)
	{
		if (n != 0)
			throw "Don't even think about it!";
	}

	template<class U>
	SmartPtr(U* p, int* c) : ptr(p)
	{
		count.init(c);
	}

	template<class U>
	SmartPtr(U* p) : ptr(p)
	{
		count.init();
	}
	~SmartPtr()
	{
		invalidate();
	}

#if _MSC_VER < 1300
	// non-const for vc to accept both, may cause evil things...
	SmartPtr(SmartPtr& sp)
#else
	SmartPtr(const SmartPtr& sp)
#endif
	{
		assign(sp);
	}

	template<class U>
	SmartPtr(const SmartPtr<U, R, D>& sp)
	{
		assign(sp);
	}

#if _MSC_VER < 1300
	// non-const for vc to accept both, may cause evil things...
	SmartPtr& operator=(SmartPtr& sp)
#else
	SmartPtr& operator=(const SmartPtr& sp)
#endif
	{
		if (ptr == sp.get()) {
			return *this;
		}
		invalidate();
		assign(sp);
		return *this;
	}

	template<class U>
	SmartPtr& operator=(const SmartPtr<U, R, D>& sp)
	{
		if (ptr == sp.get()) {
			return *this;
		}
		invalidate();
		assign(sp);
		return *this;
	}

	SmartPtr& operator=(int zero)
	{
		if (zero != 0)
			throw "Don't even think about it!";
		invalidate();
		return *this;
	}

	bool operator==(const T* p) const {
		return ptr == p;
	}

	bool operator!=(const T* p) const {
		return ptr != p;
	}
	template<class U>
	bool operator==(const WeakPtr<U>& p) const {
		return ptr == p.get();
	}
	template<class U>
	bool operator!=(const WeakPtr<U>& p) const {
		return ptr != p.get();
	}

	template<class U>
	bool operator==(const SmartPtr<U, R, D>& sp) const {
		return ptr == sp.get();
	}

	template<class U>
	bool operator!=(const SmartPtr<U, R, D>& sp) const {
		return ptr != sp.get();
	}

	T* operator->() { return ptr; }
	const T* operator->() const { return ptr; }

	T& operator*() { return *ptr; }
	const T& operator*() const { return *ptr; }

	// just convenient
	SmartPtr<T, R, D> clone() const { return SmartPtr<T, R, D>(ptr); }

	void invalidate() {
		if (ptr && count.dec() == 0) {
			dealloc();
		}
		ptr = 0;
	}
	void force_free() {
		if (ptr)
			dealloc();
	}
	bool valid() const {
		return count.count != (int*)0xdead && ptr != 0;
	}

	T* release() const {
		count.kill();
		T* p = ptr;
		ptr = 0;
		return p;
	}

	// NOT for external use..
	T* get() const { return ptr; }
	RefCountType getcount() const { return count; }

	WeakPtr<T> get_weak_ptr() {
		return WeakPtr<T>(ptr);
	}

	// Just a temporary "solution" to a problem, will be removed.
	template<class N>
	SmartPtr& dynamic_assign(const SmartPtr<N, R, D>& sp)
	{
		ptr = dynamic_cast<T*>(sp.get());
		count = sp.getcount();
		count.inc();
		return *this;
	}
};


}
}


#endif

