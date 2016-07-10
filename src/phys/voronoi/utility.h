#ifndef REAPER_PHYS_UTILITY_H
#define REAPER_PHYS_UTILITY_H

#include <cmath>
#include <vector>
#include <functional>
#include <iterator>
#include <algorithm>

#include "main/types.h"
#include "main/types.h"

namespace reaper
{
namespace phys
{

template<class A, class B, class C>
class CloserTo : std::binary_function<A, B, bool>
{
	const C &c;
public:
	CloserTo(const C &c) : c(c) {}
	result_type operator()(const first_argument_type &a, const second_argument_type &b) const { 
		return a.distance_to(c) < b.distance_to(c); 
	} 
};
/*
template<class A, class B, class C>
class CloserTo<A*, B*, C> : std::binary_function<A*, B*, bool>
{
	const C &c;
public:
	CloserTo(const C &c) : c(c) {}
	result_type operator()(const first_argument_type a, const second_argument_type b) const { 
		return a->distance_to(c) < b->distance_to(c); 
	} 
};
*/
template<class I, class T> I find_closest(I first, I last, const T &obj)
{
	return std::min_element(first, 
				last, 
				CloserTo<std::iterator_traits<I>::value_type, 
				         std::iterator_traits<I>::value_type, 
                                         T>(obj));
}





template<class T>
class deref_fun_t : 
public std::unary_function<typename T::argument_type, typename T::result_type>
{
	const T &t;
public:
	deref_fun_t(const T &t) : t(t) {}
	result_type operator()(argument_type *a) { return t(*a); };
};

template<class T>
deref_fun_t<T> deref_fun(const T &t) {
	return deref_fun_t<T>(t);
}

template<class T>
class deref_fun2_t : 
public std::binary_function<typename T::first_argument_type, 
                            typename T::second_argument_type, 
                            typename T::result_type>
{
	const T &t;
public:
	deref_fun2_t(const T &t) : t(t) {}
	result_type operator()(first_argument_type *a, second_argument_type *b) const { return t(*a, *b); };
};

template<class T>
deref_fun2_t<T> deref_fun2(const T &t) {
	return deref_fun2_t<T>(t);
}




template<class T, class A, class R>
class deref_mem_fun_t : public std::unary_function<A, R>
{
	typedef R (T::*memptr)(A &a);
	T* ptr;
	memptr mem;
public:
	deref_mem_fun_t(T* t, memptr p) : ptr(t), mem(p) {}
	R operator()(A *a) { return (ptr->*mem)(*a); };
};

template<class T, class A, class R>
deref_mem_fun_t<T, A, R> deref_mem_fun(R (T::*m)(A&), T *t) {
	return deref_mem_fun_t<T, A, R>(m, t);
}

template<class T, class A, class B, class R>
class deref_mem_fun2_t : public std::binary_function<A, B, R>
{
	typedef R (T::*memptr)(A &a, B &b);
	T* ptr;
	memptr mem;
public:
	deref_mem_fun2_t(T* t, memptr p) : ptr(t), mem(p) {}
	R operator()(A *a, B *b) { return (ptr->*mem)(*a, *b); };
};

template<class T, class A, class B, class R>
deref_mem_fun2_t<T, A, B, R> deref_mem_fun2(R (T::*m)(A&, B&), T *t) {
	return deref_mem_fun2_t<T, A, B, R>(m, t);
}

//  template<class A, class B, class EqA = equal_to<A>, class EqB=equal_to<B>>
//  class EqualTwo : public binary_function<A, B, bool>
//  {
//  public:
//  	CompareTwo(const A &a, const &B b) : a(a), b(b) {}

//  	bool operator()(const A &x, const B &y) const { return EqA(x, a) && EqB(y, b); } 
//  protected:
//  	const A a;
//  	const B b;
//  };

template<class I, class T, class B=T>
class DerefIterator : 
public std::iterator<std::iterator_traits<I>::iterator_category, B, 
                     std::iterator_traits<I>::distance_type> 
{
public:
	DerefIterator(I j) : i(j) {}

	bool operator==(const DerefIterator &j) { return j.i == i; } 
	bool operator!=(const DerefIterator &j) { return j.i != i; } 

	const B &operator*()  const { return static_cast<B&>(*(*i)); }
	const B *operator->() const { return static_cast<B*>(*i); }

	B &operator*()  { return static_cast<B&>(*(*i)); }
	B *operator->() { return static_cast<B*>(*i);    }

	DerefIterator operator+(const distance_type &j) const { return i + j; }
	distance_type operator-(const DerefIterator &j) const { return i - j.i; }

	B &operator[](const distance_type &j) { return *i[j]; }
	const B &operator[](const distance_type &j) const { return *i[j]; }

	void operator++(int) { i++; }
	void operator++()    { ++i; }
protected:
	I i;
};

}
}

#endif



