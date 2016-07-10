#ifndef REAPER_MISC_HASH_H
#define REAPER_MISC_HASH_H

#include <vector>
#include <string>
#include <map>

#ifdef HAVE_HASH_MAP
#include <hash_map>
#endif

#ifdef HAVE_EXT_HASH_MAP
#include <ext/hash_map>
#endif


namespace reaper {

#ifdef HASH_NAMESPACE_GNU_CXX
namespace hash = ::__gnu_cxx;
#endif

#ifdef HASH_NAMESPACE_STD
namespace hash = ::std;
#endif

#ifdef HASH_NAMESPACE_NONE
#define hash 
#endif

#ifdef USE_MAP_AS_HASH_MAP

namespace hash {

template<class K, class T>
class hash_map
{
	typedef std::map<K,T> Map;
	Map map;

public:
	typedef Map::iterator iterator;
public:
	T& operator[](const K& k) {
		return map[k];
	}
	void insert(const std::pair<K,T>& p) {
		(*this)[p.first] = p.second;
	}
	void insert(const K& k, const T& t) {
		(*this)[k] = t;
	}
	iterator find(const K& k) {
		return map.find(k);
	}
	void erase(const K& k) {
		map.erase(k);
	}
	void erase(const iterator& i) {
		map.erase(i);
	}
	size_t size() const {
		return map.size();
	}
	bool elem(const K& k) const {
		return map.find(k) != map.end();
	}
	void clear() {
		map.clear();
	}
	iterator begin() {
		return map.begin();
	}
	iterator end() {
		return map.end();
	}
};

}


#endif

#ifdef USE_OWN_HASH_MAP

/* A very naïve and simplistic hashtable, improve! */
/* WARNING, buggy, but I don't have a
 *          simple testcase   -peter
 */

/* Default hasher, just xor the representation,
 * if you know your type, you can and should do better..
 */

namespace hash {


template<class T>
struct Hash
{
	size_t operator()(const T& t) const {
		const char* p = reinterpret_cast<const char*>(&t);
		size_t r = 0;
		for (size_t i = 0; i < sizeof(T); ++i)
			r = (r<<1)^*p++;
		return r;
	}
};



template<>
struct Hash<std::string>
{
	size_t operator()(const std::string& s) const {
		size_t n = 0;
		for (size_t i = 0; i < s.size(); ++i) 
			n = (n<<1) ^ s[i];
		return n;
	}
};

template<>
struct Hash<int>
{
	size_t operator()(int i) const {
		return i;
	}
};

/* Note that the element type needs a default constructor,
 * it can be avoided by a slight redesign, if necessary..
 * Really big elements should not be stored, since they
 * are copied upon resize.
 *
 */

template<class K, class T, class H = Hash<K>, class EQ = std::equal_to<K> >
class hash_map
{
private:
	struct Node
	{
		Node* next;
		std::pair<K,T> data;
		bool used;

		Node() : next(0), used(false) { }
		Node(const std::pair<K,T>& p, Node* n = 0)
		 : next(n), data(p), used(true)
		{ }
		Node(const Node& n) : next(n.next), data(n.data), used(n.used) { }
		Node& operator=(const Node& n) {
			next = n.next;
			data.first = n.data.first;
			data.second = n.data.second;
			used = n.used;
			return *this;
		}
	};
	typedef std::vector<Node> Tbl;
	typedef Node* Iter;
	typedef const Node* CIter;
	Tbl tbl;
	H hash;
	EQ eq;
	size_t count;

public:
	class iterator : public std::iterator<std::forward_iterator_tag,
					      std::pair<K,T> >
	{
		Tbl* tbl;
		size_t idx;
		Iter iter;
		
		void find_next() {
			do {
				iter = iter->next;
				if (!iter) {
					++idx;
					if (idx == tbl->size()) {
						tbl = 0;
						idx = 0;
						iter = 0;
						return;
					}
					iter = &(*tbl)[idx];
				}
			} while (!iter->used);
		}
	public:
		typedef std::pair<K,T> value_type;

		iterator(Tbl* t) : tbl(t), idx(0), iter(&(*tbl)[0]) {
			if (!iter->used)
				find_next();
		}
		iterator(Tbl* t, int ix, Iter i) : tbl(t), idx(ix), iter(i) {
			if (!iter || (iter && !iter->used)) {
				tbl = 0;
				iter = 0;
				idx = 0;
			}
		}
		iterator() : tbl(0), idx(0), iter(0) { }

		iterator(const iterator& i)
		 : tbl(i.tbl), idx(i.idx), iter(i.iter)
		{ }

		iterator& operator=(const iterator& i) {
			tbl = i.tbl;
			idx = i.idx;
			iter = i.iter;
			return *this;
		}

		iterator& operator++() {
			find_next();
			return *this;
		}

		iterator operator++(int) {
			iterator i(*this);
			++(*this);
			return i;
		}
		
		value_type& operator*() { return iter->data; }
		const value_type& operator*() const { return iter->data; }

		value_type* operator->() { return &iter->data; }
		const value_type* operator->() const { return &iter->data; }
		
		bool operator==(const iterator& i) const {
			return tbl == i.tbl && idx == i.idx && iter == i.iter;
		}

		bool operator!=(const iterator& i) const {
			return !(*this == i);
		}
	};
private:

	Iter find_node(const K& k) {
		return &tbl[hash(k) % tbl.size()];
	}
	CIter find_node(const K& k) const {
		return &tbl[hash(k) % tbl.size()];
	}

	void rehash() {
		Tbl old;
		old.swap(tbl);
		count = 0;
		tbl.resize(static_cast<int>(3.14*old.size()));
		for (size_t i = 0; i < old.size(); ++i) {
			Iter n = &old[i];
			if (n->used)
				insert(n->data);
			n = n->next;
			while (n) {
				if (n->used)
					insert(n->data);
				Iter p = n;
				n = n->next;
				delete p;
			}
		}
	}
	void insert_new(const K& k, const T& t) {
		insert_new(std::make_pair(k,t));
	}

	void insert_new(const std::pair<K,T>& p) {
		if (count > tbl.size()) {
			rehash();
		}
		Iter n = find_node(p.first);
		if (n->used)
			n->next = new Node(p, n->next);
		else
			*n = Node(p, n->next);
		++count;
	}
public:
	hash_map(int init_size = 13)
	 : tbl(init_size), count(0)
	{ }
	~hash_map() {
		clear();
	}
	T& operator[](const K& k) {
		Iter p = find_node(k);
		while (p && !eq(p->data.first, k))
			p = p->next;
		if (p && p->used) {
			return p->data.second;
		} else {
			insert_new(k, T());
			return (*this)[k];
		}
	}
	void insert(const std::pair<K,T>& p) {
		(*this)[p.first] = p.second;
	}
	void insert(const K& k, const T& t) {
		(*this)[k] = t;
	}
	iterator find(const K& k) {
		Iter n = find_node(k);
		Iter p = n;
		while (p && !eq(p->data.first, k))
			p = p->next;
		return iterator(&tbl, n - &tbl[0], p);
	}
	void erase(const K& k) {
		--count;
		Iter n = find_node(k);
		Iter p = 0;
		if (eq(n->data.first, k)) {
			if (n->next)
				*n = *n->next;
			else {
				n->data.second = T();
				n->used = false;
			}
			return;
		}
		while (n->next) {
			p = n;
			n = n->next;
			if (eq(n->data.first, k)) {
				p->next = n->next;
				delete n;
				return;
			}
		}
		++count;
	}
	void erase(const iterator& i) {
		erase(i->first); // FIXME
	}
	size_t size() const {
		return count;
	}
	bool elem(const K& k) const {
		CIter n = find_node(k);
		while (n) {
			if (eq(n->data.first, k) && n->used)
				return true;
			n = n->next;
		}
		return false;
	}
	void clear() {
		size_t init_size = tbl.size();
		Tbl empty;
		tbl.swap(empty);
		tbl.resize(init_size);
		
		for (size_t i = 0; i < empty.size(); ++i) {
			Iter n = empty[i].next;
			while (n) {
				Iter t = n;
				n = n->next;
				delete t;
			}
		}

	}

	iterator begin() {
		return iterator(&tbl);
	}
	iterator end() {
		return iterator();
	}
};

}

#endif

}

#endif


