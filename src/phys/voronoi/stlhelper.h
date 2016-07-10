#ifndef REAPER_PHYS_STLHELPER_H
#define REAPER_PHYS_STLHELPER_H

#include <functional>
#include <iterator>

namespace std // DANGER! Extending std and will probably screw up your life 4ever
{
/*
template <class _Ret, class _Tp, class _Arg>
class const_mem_fun1_ref_t<_Ret, _Tp, _Arg&> : public binary_function<_Tp,_Arg,_Ret> {
	typedef _Ret (_Tp::*_fun_type)(_Arg&) const;
public:
	explicit const_mem_fun1_ref_t(_fun_type __pf) : _M_f(__pf) {}
	_Ret operator()(const _Tp& __r, _Arg &__x) const { return (__r.*_M_f)(__x); }
private:
	_fun_type _M_f;
};
*/
}

namespace hlp
{

template<class I, class T>
class cast_iterator :
public std::iterator<std::iterator_traits<I>::iterator_category,
		     T,
		     std::iterator_traits<I>::distance_type>
{
	I i;
public:
	cast_iterator(const I &i) : i(i) {}

	bool operator==(const cast_iterator &j) { return j.i == i; } 
	bool operator!=(const cast_iterator &j) { return j.i != i; } 

	const T &operator*()  const { return dynamic_cast<T&>(*i); }
	const T *operator->() const { return dynamic_cast<T*>(&(*i)); }

	T operator*()  { 
		return dynamic_cast<T>(*i);
	}
	
	T *operator->() { return dynamic_cast<T*>(&(*i)); }

	cast_iterator operator+(const distance_type &j) const { return i + j; }
	distance_type operator-(const cast_iterator &j) const { return i - j.i; }

	T &operator[](const distance_type &j) { return i[j]; }
	const T &operator[](const distance_type &j) const { return i[j]; }

//	void operator++(int) { return i++; }
	cast_iterator &operator++()    { ++i; return *this; }

//	void operator--(int) { return i--; }
	cast_iterator &operator--()    { --i; return *this; }
};

template <class Op1, class Op2>
class unary_compose : 
public std::unary_function<typename Op2::argument_type,
                           typename Op1::result_type> 
{
protected:
	Op1 f;
	Op2 g;
public:
	unary_compose(const Op1& x, const Op2& y) 
		: f(x), g(y) {}
	typename Op1::result_type operator()(const typename Op2::argument_type& x) const {
		return f(g(x));
	}
};

template <class Op1, class Op2>
inline unary_compose<Op1, Op2> 
compose1(const Op1 &f, const Op2 &g)
{
	return unary_compose<Op1, Op2>(f, g);
}

template<class Arg, class Arg2, class Arg3, class Res>
struct trinary_function
{
	typedef Arg first_argument_type;
	typedef Arg2 second_argument_type;
	typedef Arg3 third_argument_type;
	typedef Res result_type;
};

//  template<class F, class G>
//  class compose_t : 
//  public std::unary_function<typename G::argument_type, typename F::result_type>
//  {
//  	F f;
//  	G g;
//  public:
//  	compose_t(F x, G y) : f(x), g(y) {}
//  	result_type operator()(argument_type a) { return f(g(a)); }
//  };

//  template<class F, class G>
//  compose_t<F, G> compose(const F &f, const G &g){ 
//  	return compose_t<F, G>(f, g);
//  }

// Member Function Adapters

template<class A, class A2,class A3, class R>
class mem_fun2_t : public trinary_function<A*, A2, A3, R>
{
	R (A::*pmf)(A2, A3);
public:
	explicit mem_fun2_t(R (A::*p)(A2, A3)) : pmf(p) {}
	R operator()(A* a, A2 a2, A3 a3) { return (a->*pmf)(a2, a3); }
};

template<class A, class A2, class A3, class R>
mem_fun2_t<A, A2, A3, R> mem_fun2(R (A::*f)(A2, A3))
{
	return mem_fun2_t<A, A2, A3, R>(f);
}

template<class A, class A2,class A3, class R>
class const_mem_fun2_t : public trinary_function<const A*, A2, A3, R>
{
	R (A::*pmf)(A2, A3) const;
public:
	explicit const_mem_fun2_t(R (A::*a)(A2, A3) const) : pmf(a) {}
	R operator()(const A* a, A2 a2, A3 a3) const { return (a->*pmf)(a2, a3); }
};

template<class A, class A2, class A3, class R>
const_mem_fun2_t<A, A2, A3, R> const_mem_fun2(R (A::*f)(A2, A3) const)
{
	return const_mem_fun2_t<A, A2, A3, R>(f);
}

template<class A, class A2,class A3, class R>
class mem_fun2_ref_t : public trinary_function<A, A2, A3, R>
{
	R (A::*pmf)(A2, A3);
public:
	explicit mem_fun2_ref_t(R (A::*a)(A2, A3)) : pmf(a) {}
	R operator()(A &a, A2 a2, A3 a3) { return ((&a)->*pmf)(a2, a3); }
};

template<class A, class A2,class A3, class R>
mem_fun2_ref_t<A, A2, A3, R> mem_fun2_ref(R (A::*f)(A2, A3))
{
	return mem_fun2_ref_t<A, A2, A3, R>(f);
}	

template<class A, class A2, class A3, class R>
class const_mem_fun2_ref_t : public trinary_function<const A, A2, A3, R>
{
	R (A::*pmf)(A2, A3) const;
public:
	explicit const_mem_fun2_ref_t(R (A::*p)(A2, A3) const) : pmf(p) {}
	R operator()(const A &a, A2 a2, A3 a3) const { return ((&a)->*pmf)(a2, a3); }
};

template<class A, class A2,class A3, class R>
const_mem_fun2_ref_t<A, A2, A3, R> const_mem_fun2_ref(R (A::*f)(A2, A3) const)
{
	return const_mem_fun2_ref_t<A, A2, A3, R>(f);
}

// Binders

template<class T>
class binder1st3 : public std::binary_function<typename T::second_argument_type, 
	                                       typename T::third_argument_type,
				               typename T::result_type>
{
protected:
	T op;
	typename T::first_argument_type a;
public:
	binder1st3(const T& x, const typename T::first_argument_type& v)
		: op(x), a(v) {}
	result_type operator()(const first_argument_type& a2, const second_argument_type& a3) const
		{ return op(a, a2, a3); }
};

template<class TriOp, class T>
binder1st3<TriOp> bind1st3(const TriOp& op, const T& a)
{
	return binder1st3<TriOp>(op, a);
}

}

#endif
