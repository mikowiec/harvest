
/* $Id: stlhelper.h,v 1.23 2002/05/31 14:40:37 pstrand Exp $ */

#ifndef REAPER_MISC_STLHELPER_H
#define REAPER_MISC_STLHELPER_H

#include <algorithm>
#include <functional>

///
namespace reaper
{
///
namespace misc
{

// Funktionsobjektskomposition
// finns tydligen i sgi's stl, tror inte det är standard dock...

template<class F1, class F2>
class compose_t
 : public std::unary_function<typename F1::argument_type,
 			      typename F2::result_type>
{
	F1 func1;
	F2 func2;
public:
	compose_t(F1 f1, F2 f2) : func1(f1), func2(f2) { }
	typename F2::result_type operator()(typename F1::argument_type a) {
		return func1(func2(a));
	}
};

template<class F1, class F2>
compose_t<F1, F2> compose(F1 f1, F2 f2) {
	return compose_t<F1, F2>(f1, f2);
}


template <class T, class A>
class apply_val0 : public std::unary_function<A, void> {
	const A& val;
public:
	apply_val0(const A& v) : val(v) { }
	void operator()(T* t) {
		(*t)(val);
	}
};


template <class T, class A, class R>
class apply_val_t : public std::unary_function<A, R> {
	typedef R (T::*memptr)(A a);
	memptr mem;
	A val;
public:
	apply_val_t(memptr p, A v) : mem(p), val(v) { }
	void operator()(T* t) {
		(t->*mem)(val);
	}
};

template<class T, class A, class R>
apply_val_t<T, A, R> apply_val(R (T::*m)(A), A a) {
	return apply_val_t<T, A, R>(m, a);
}


template <class T, class A1, class A2, class R>
class apply_val2_t : public std::binary_function<A1, A2, R> {
	typedef R (T::*memptr)(A1, A2);
	memptr mem;
	const A1& val1;
	const A2& val2;
public:
	apply_val2_t(memptr p, const A1& v1, const A2& v2)
	 : mem(p), val1(v1), val2(v2)
	{ }
	void operator()(T* t) {
		(t->*mem)(val1, val2);
	}
};

template<class T, class A1, class A2, class R>
apply_val2_t<T, A1, A2, R> apply_val(R (T::*m)(A1, A2), const A1& a1, const A2& a2) {
	return apply_val2_t<T, A1, A2, R>(m, a1, a2);
}






template <class T, class A, class R>
class apply_to_t : public std::unary_function<A, R> {
	typedef R (T::*memptr)(A a);
	T* ptr;
	memptr mem;
public:
	apply_to_t(T* t, memptr p) : ptr(t), mem(p) { }
	void operator()(A a) {
		(ptr->*mem)(a);
	}
};

template<class T, class A, class R>
apply_to_t<T, A, R> apply_to(T* t, R (T::*m)(A)) {
	return apply_to_t<T, A, R>(t, m);
}

template<class A, class R>
class bind_t {
	typedef R (*funptr)(A a);
	funptr fun;
	A arg;
public:
	bind_t(funptr f, A a) : fun(f), arg(a) {}
	R operator()() {
		return fun(arg);
	}
};

template<class A, class R>
bind_t<A,R> bind(R (*f)(A a), A a) {
	return bind_t<A,R>(f,a);
}

// Visual STL binder2nd lacks copy constructor
template<class F>
class binder2nd : public std::unary_function<typename F::first_argument_type, typename F::result_type> 
{
	typedef typename F::first_argument_type arg;
	typedef typename F::result_type res;
public:
	binder2nd(const binder2nd<F> &b) : op(b.op), value(b.value) {}
	binder2nd(const F& f, const typename F::second_argument_type& v) : op(f), value(v) {}
	res operator()(const arg& a) const { return (op(a, value)); }
protected:
	F op;
	typename F::second_argument_type value;
};

template<class F, class T> 
binder2nd<F> bind2nd(const F& f, const T& a) {
	typedef typename F::second_argument_type arg_t;
	return binder2nd<F>(f, arg_t(a)); 
}

template<class A, class R>
class unary_object
 : public std::unary_function<A, R>
{
public:
	virtual R operator()(A) = 0;
};

template<class A, class B, class R>
class binary_object
 : public std::binary_function<A, B, R>
{
public:
	virtual R operator()(A, B) = 0;
};

template<class C>
class PushBack
{
	C& c;
public:
	PushBack(C& cc) : c(cc) { }
	PushBack& operator<<(const typename C::value_type& t) {
		c.push_back(t);
		return *this;
	}
};

template<class C>
PushBack<C> push_back(C& c)
{
	return PushBack<C>(c);
}

template<class C, class V>
void erase(C& c, const V& v)
{
	c.erase(std::remove(c.begin(), c.end(), v), c.end());
}

template<class Iter, class F>
void map_snd(Iter c, Iter e, F f)
{
	for (;c != e; ++c)
		f(c->second);
}

}
}

#endif


