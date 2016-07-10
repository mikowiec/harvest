
/* $Id: sequence.h,v 1.34 2003/06/04 16:35:59 pstrand Exp $ */

#ifndef REAPER_MISC_SEQUENCE_H
#define REAPER_MISC_SEQUENCE_H

#include <iterator>
#include <algorithm>

/// Reaper!
namespace reaper
{
/// Misc
namespace misc
{

class out_of_bounds { };


/** Sequence type.
    Is only a pair of iterators, with a few helper members/functions.
*/
template<class Iter, typename Val = typename std::iterator_traits<Iter>::value_type>
class Seq {
public:
	typedef Val value_type;

	Iter first, second;

	Seq(const Seq& s) : first(s.first), second(s.second) { }
	Seq(Iter i, Iter j) : first(i), second(j) { }
//	template<class C>
//	Seq(const C& c) : first(c.begin()), second(c.end()) { }

	value_type value() const {
		if (*this)
			return *first;
		else
			throw out_of_bounds();
	}
	value_type operator*() const {
		return value();
	}
	Seq& operator++() {
		++first;
		return *this;
	}
	Seq operator++(int) {
		Seq s = *this;
		++first;
		return s;
	}
	Iter operator->() {
		return first;
	}
	operator bool() const {
		return first != second;
	}
	long size() const {
		long i = 0;
		Seq s = *this;
		while (s++)	i++;
		return i;
	}
};



template<class T>
Seq<typename T::iterator> seq(T& t) {
	typedef typename T::iterator Ti;
	return Seq<Ti>(t.begin(), t.end());
}

template<class T>
Seq<typename T::reverse_iterator> rseq(T& t) {
	typedef typename T::reverse_iterator Ti;
	return Seq<Ti>(t.rbegin(), t.rend());
}


template<class T>
Seq<typename T::const_iterator> cseq(const T& t) {
	typedef typename T::const_iterator Tci;
	return Seq<Tci>(t.begin(), t.end());
}

template<class T>
Seq<typename T::const_reverse_iterator> rcseq(const T& t) {
	typedef typename T::const_reverse_iterator Tci;
	return Seq<Tci>(t.rbegin(), t.rend());
}



template<class T>
std::ostream& operator<<(std::ostream& os, Seq<T>& s) {
	copy(s, std::ostream_iterator<typename Seq<T>::value_type>(os, " "));
	return os;
}


template<class T, class F>
F for_each_d(Seq<T> seq, F f)
{
	while (seq) {
		f(*seq);
		++seq;
	}
	return f;
}

template<class T, class F>
void map_snd(Seq<T> seq, F f) {
	for (;seq; ++seq)
		f(seq->second);
}


// Resten är bara wrappers för olika algoritmer för sekvenser,
// för att spara ett argument...
// Lägg gärna till efter behov.

template<class T, class F>
F copy(Seq<T> seq, F f) {
	return std::copy(seq.first, seq.second, f);
}

template<class T, class F>
F for_each(Seq<T> seq, F f) {
	return std::for_each(seq.first, seq.second, f);
}

template<class T, class O, class F>
O transform(Seq<T> s, O o, F f) {
	return std::transform(s.first, s.second, o, f);
}

template<class T, class E>
T find(Seq<T> s, E e) {
	return std::find(s.first, s.second, e);
}

template<class T, class F>
T find_if(Seq<T> s, F f) {
	return std::find_if(s.first, s.second, f);
}

template<class Iter, class Pred>
Iter unique(Seq<Iter> s, Pred p) {
	return std::unique(s.first, s.second, p);
}

template<class Iter, class Out, class Pred>
Out unique_copy(Seq<Iter> s, Out out, Pred p) {
	return std::unique_copy(s.first, s.second, out, p);
}


template<class Iter, class Val>
Iter remove(Seq<Iter> s, Val v) {
	return std::remove(s.first, s.second, v);
}

template<class Iter, class Pred>
Iter remove_if(Seq<Iter> s, Pred p) {
	return std::remove_if(s.first, s.second, p);
}

template<class Iter, class Pred>
Iter remove_copy_if(Seq<Iter> s, class Out o, Pred p) {
	return std::remove_copy_if(s.first, s.second, o, p);
}

template<class Iter, class Val>
void fill(Seq<Iter> s, const Val& v) {
	std::fill(s.first, s.second, v);
}

template<class Iter>
void sort(Seq<Iter> s) {
	std::sort(s.first, s.second);
}

template<class Iter, class Pred>
void sort(Seq<Iter> s, Pred p) {
	std::sort(s.first, s.second, p);
}

template<class Iter, class Val>
Iter lower_bound(Seq<Iter> s, Val v) {
	return std::lower_bound(s.first, s.second, v);
}

template<class Iter, class Val, class F>
Iter lower_bound(Seq<Iter> s, Val v, F f) {
	return std::lower_bound(s.first, s.second, v, f);
}

template<class Iter, class Val>
Iter upper_bound(Seq<Iter> s, Val v) {
	return std::upper_bound(s.first, s.second, v);
}

template<class Iter, class Val, class F>
Iter upper_bound(Seq<Iter> s, Val v, F f) {
	return std::upper_bound(s.first, s.second, v, f);
}

template<class F, class S>
struct fst_eq {
	F k;
	fst_eq(F f) : k(f) { }
	bool operator()(std::pair<F,S> p) const {
		return p.first == k;
	}
};

template<class Cont, class Pred>
void erase_if(Cont& c, Pred p)
{
	typedef typename Cont::iterator iter;
	for (Seq<iter> ss(c.begin(), c.end()); ss;) {
		typename Cont::iterator i = ss.first++;
		if (p(*i))
			c.erase(i);
	}
}

template<class Pred>
class Dealloc
{
	Pred p;
public:
	Dealloc(Pred pp) : p(pp) { }
	template<class T>
	bool operator()(T* t)
	{
		bool r = p(t);
		if(r)
			delete t;
		return r;
	}
};

template<class Cont, class Pred>
void erase_dealloc_if(Cont& c, Pred p)
{
	erase_if(c, Dealloc<Pred>(p));
}


}
}

#endif


