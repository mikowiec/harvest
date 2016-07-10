#include <functional>
#include "misc/sequence.h"

namespace reaper {
namespace misc {


template <typename Iter, typename Pr>
class skip_iterator : public std::iterator_traits<Iter>
{
	Iter i;
	Iter end;
	Pr pr;

	void skip() {
		while(i != end && pr(*i)) {
			++i;
		}
	}

public:
	typedef typename std::iterator_traits<Iter>::value_type value_type;

	skip_iterator(const skip_iterator &si) 
		: i(si.i), end(si.end), pr(si.pr) {}
	skip_iterator(const Iter &i_, const Iter &end_, const Pr &p) 
		: i(i_), end(end_), pr(p) { skip(); }

	value_type operator*() const { return *i; }
	value_type* operator->() { return &*i; }

	skip_iterator& operator++() { 
		if(i != end) {
			++i;
			skip();
		}
		return *this;
	}

	skip_iterator operator++(int) {
		skip_iterator clone = *this;
		++(*this);
		return clone;
	}

	bool operator==(const skip_iterator &t) const {
		return i == t.i;
	}

	bool operator!=(const skip_iterator &t) const {
		return i != t.i;
	}
};

template <typename Iter, typename Pr>
inline skip_iterator<Iter, Pr> skip(Iter i, Iter end, Pr pr)
{
	return skip_iterator<Iter, Pr>(i, end, pr);
}

template <typename Iter>
inline skip_iterator<Iter, std::binder1st<std::equal_to<typename Iter::value_type> > >
	skip_eq(Iter i, Iter end, const typename Iter::value_type &t) 
{	
	return skip(i, end, std::bind1st(std::equal_to<typename Iter::value_type>(), t));
}

// skip-sequences

template <typename Iter, typename Pr>
inline Seq<skip_iterator<Iter, Pr> > skip_seq(Iter i, Iter end, Pr pr)
{
	return Seq<skip_iterator<Iter, Pr> >(skip(i, end, pr),
	                                     skip(end, end, pr));		
}

template <typename Iter, typename Pr>
inline Seq<skip_iterator<Iter, Pr> > skip_seq(const Seq<Iter> &s, Pr pr)
{
	return Seq<skip_iterator<Iter, Pr> >(skip(s.first, s.second, pr), 
	                                     skip(s.second, s.second, pr));		
}

template <typename Iter>
inline Seq<skip_iterator<Iter, std::binder1st<std::equal_to<typename Iter::value_type> > > >
	skip_seq_eq(Iter i, Iter end, const typename Iter::value_type &t)
{
	return Seq<skip_iterator<Iter, std::binder1st<std::equal_to<typename Iter::value_type> > > >
	        (skip_eq(i, end, t), skip_eq(end, end, t));		
}

template <typename Iter>
inline Seq<skip_iterator<Iter, std::binder1st<std::equal_to<typename Iter::value_type> > > >
	skip_seq_eq(const Seq<Iter> &s, const typename Iter::value_type &t)
{
	return Seq<skip_iterator<Iter, std::binder1st<std::equal_to<typename Iter::value_type> > > >
		(skip_eq(s.first, s.second, t), skip_eq(s.second, s.second, t));		
}

}
}
