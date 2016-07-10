
#ifndef REAPER_MISC_TUPLE_H
#define REAPER_MISC_TUPLE_H

namespace reaper {
namespace misc {

struct Nil
{
	Nil() { }
	template<class T0, class T1, class T2,
	         class T3, class T4, class T5>
	Nil(T0 t0, T1 t1, T2 t2,
	    T3 t3, T4 t4, T5 t5)
	{ }
};

template<class H, class T>
struct Cons
{
	H hd;
	T tl;
	typedef H Head;
	typedef T Tail;
	Cons(H h, T t)
	 : hd(h), tl(t)
	{ }
	template<class T0, class T1, class T2,
	         class T3, class T4, class T5>
	Cons(T0 t0, T1 t1, T2 t2,
	     T3 t3, T4 t4, T5 t5)
	 : hd(t0), tl(t1, t2, t3, t4, t5, Nil())
	{ }
	
};

template<class T0, class T1, class T2, class T3, class T4, class T5>
struct Flatten
{
	typedef Cons<T0, typename Flatten<T1, T2, T3, T4, T5, Nil>::t> t;
};

template<>
struct Flatten<Nil, Nil, Nil, Nil, Nil, Nil>
{
	typedef Nil t;
};

template<int N, class List>
struct GetAt
{
	typedef typename GetAt<N-1, typename List::Tail>::t t;
	static t get(typename List::Head h, typename List::Tail t) {
		return GetAt<N-1, typename List::Tail>::get(t.hd, t.tl);
	}
};

template<class List>
struct GetAt<0, List>
{
	typedef typename List::Head t;
	static t get(typename List::Head h, typename List::Tail) {
		return h;
	}
};



template<class T0 = Nil, class T1 = Nil, class T2 = Nil,
	 class T3 = Nil, class T4 = Nil, class T5 = Nil>
struct Tuple : public Flatten<T0, T1, T2, T3, T4, T5>::t
{
	typedef Flatten<T0, T1, T2, T3, T4, T5>::t List;
	Tuple(T0 t0 = T0(), T1 t1 = T1(),
	      T2 t2 = T2(), T3 t3 = T3(),
	      T4 t4 = T4(), T5 t5 = T5())
	 : List(t0, t1, t2, t3, t4, t5)
	{ }
	
	template<int N>
	typename GetAt<N, List>::t get()
	{
		return GetAt<N, List>::get(hd, tl);
	}

	T0 fst() const { return hd; }
	T1 snd() const { return tl.hd; }
	T2 trd() const { return tl.tl.hd; }
};

template<class T>
void bind(T& t, Tuple<T> tt)
{
	t0 = tt.hd;
}

template<class T0, class T1>
void bind(T0& t0, T1& t1, Tuple<T0, T1> tt)
{
	t0 = tt.hd;
	t1 = tt.tl.hd;
}

template<class T0, class T1, class T2>
void bind(T0& t0, T1& t1, T2& t2, Tuple<T0, T1, T2> tt)
{
	t0 = tt.hd;
	t1 = tt.tl.hd;
	t2 = tt.tl.tl.hd;
}

template<class T0, class T1, class T2, class T3>
void bind(T0& t0, T1& t1, T2& t2, T3& t3, Tuple<T0, T1, T2, T3> tt)
{
	t0 = tt.hd;
	t1 = tt.tl.hd;
	t2 = tt.tl.tl.hd;
	t3 = tt.tl.tl.tl.hd;
}

template<class T0, class T1, class T2, class T3, class T4>
void bind(T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, Tuple<T0, T1, T2, T3, T4> tt)
{
	t0 = tt.hd;
	t1 = tt.tl.hd;
	t2 = tt.tl.tl.hd;
	t3 = tt.tl.tl.tl.hd;
	t4 = tt.tl.tl.tl.tl.hd;
}

template<class T0, class T1, class T2, class T3, class T4, class T5>
void bind(T0& t0, T1& t1, T2& t2, T3& t3, T4& t4, T5& t5, Tuple<T0, T1, T2, T3, T4, T5> tt)
{
	t0 = tt.hd;
	t1 = tt.tl.hd;
	t2 = tt.tl.tl.hd;
	t3 = tt.tl.tl.tl.hd;
	t4 = tt.tl.tl.tl.tl.hd;
	t5 = tt.tl.tl.tl.tl.tl.hd;
}


Nil tuple() { return Nil(); }

template<class T>
Tuple<T> tuple(T t) { return Tuple<T>(t); }

template<class T0, class T1>
Tuple<T0, T1>
tuple(T0 t0, T1 t1) { return Tuple<T0, T1>(t0, t1); }

template<class T0, class T1, class T2>
Tuple<T0, T1, T2>
tuple(T0 t0, T1 t1, T2 t2)
{ return Tuple<T0, T1, T2>(t0, t1, t2); }

template<class T0, class T1, class T2, class T3>
Tuple<T0, T1, T2, T3>
tuple(T0 t0, T1 t1, T2 t2, T3 t3)
{ return Tuple<T0, T1, T2, T3>(t0, t1, t2, t3); }

template<class T0, class T1, class T2, class T3, class T4>
Tuple<T0, T1, T2, T3, T4>
tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4)
{ return Tuple<T0, T1, T2, T3, T4>(t0, t1, t2, t3, t4); }

template<class T0, class T1, class T2, class T3, class T4, class T5>
Tuple<T0, T1, T2, T3, T4, T5>
tuple(T0 t0, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
{ return Tuple<T0, T1, T2, T3, T4, T5>(t0, t1, t2, t3, t4, t5); }


}
}

#endif

