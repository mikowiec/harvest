
#ifndef REAPER_WORLD_QUADTREE_H
#define REAPER_WORLD_QUADTREE_H

#include "os/compat.h"
#include "os/debug.h"

#include <list>
#include <vector>
#include <map>
#include <stack>
#include <deque>
#include <iostream>
#include <algorithm>

#include "misc/free.h"
#include "world/query_obj.h"
#include "world/triangle.h"

namespace reaper
{
namespace quadtree
{

typedef int Quadrant;

class Square;


template<class T> struct Box;

template<class T>
struct Vector {
	typedef std::vector< Box<T>* > t;
	typedef typename t::iterator iterator;
};

template<class T>
struct NodeCont : public Vector<T>
{ };


template<class E>
class QuadTree;
template<class E>
class QuadTree_dfs_iterator;


/** Decision-helper for square-calculations.
 *  Calculates the subsquare for a given query-object
 *  and the current square.
 *  And decides if an element is acceptable for a given query.
 */

template<class E>
class Acceptable
{
	enum { All, Cyl, Sphere, Rect, Line, Frustum } what;
	world::Sphere bsp;
	Point p1;
	Point p2;
	world::Frustum frustum;
public:
	Acceptable()
	 : what(All)
	{ }

	Acceptable(const world::Rect& r)
	 : what(Rect), p1(r.ll.x, 0, r.ll.y), p2(r.ur.x, 0, r.ur.y)
	{
		Point ll = Point(r.ll.x, 0, r.ll.y);
		float w = (r.ur.x - r.ll.x) / 2;
		float h = (r.ur.y - r.ll.y) / 2;
		bsp.p = ll + Point(w, 0, h);
		bsp.r = sqrt(w*w+h*h);
	}

	Acceptable(const world::Cylinder& cyl)
	 : what(Cyl), bsp(Point(cyl.p.x, 0, cyl.p.y), cyl.r)
	{ }

	Acceptable(const world::Sphere& s)
	 : what(Sphere), bsp(s)
	{ }

	Acceptable(const world::Line& l);

	Acceptable(const world::Frustum& fr)
	 : what(Frustum), frustum(fr)
	{
		//FIXME
		bsp.p = fr.pos() + fr.dir();
		bsp.r = 2*length(fr.dir());
	}

	Acceptable(const Acceptable& ac)
	 : what(ac.what), bsp(ac.bsp), p1(ac.p1), p2(ac.p2), frustum(ac.frustum)
	{ }

	Acceptable& operator=(const Acceptable& ac);

	Quadrant quadrant(const Square& sq) const;

	bool acceptable(const Box<E>& b) const;

	typedef NodeCont<E> NS;

	typename NS::iterator find_begin(typename NS::t& set, float max_size);

	typename NS::iterator find_end(typename NS::t& set, float max_size);
	float min_height() const;
};

template<class E> class Node;

template<class E>
class QuadTree_dfs_iterator
{
	Acceptable<E> ok_quads;
	typedef Node<E> Node_t;

	typedef QuadTree_dfs_iterator<E> iterator;

	QuadTree<E>* tree;
	Node_t* last;
	std::stack<Node_t*> nodes;
	typedef NodeCont<E> NS;
	typedef typename NodeCont<E>::iterator Node_iter;
	Node_iter current;
	Node_iter cur_end;
	bool at_end, sub_iter;

	void find_next_node();
	bool test();
	void init_iter(QuadTree<E>* t);
public:

	typedef E value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef std::forward_iterator_tag iterator_category;
	typedef int difference_type;
	typedef int distance_type;

	QuadTree_dfs_iterator()
	 : tree(0), last(0), at_end(true), sub_iter(false)
	{ }

	QuadTree_dfs_iterator(const QuadTree_dfs_iterator& qi)
	 : ok_quads(qi.ok_quads), tree(qi.tree), last(qi.last), nodes(qi.nodes)
	 , current(qi.current), cur_end(qi.cur_end)
	 , at_end(qi.at_end), sub_iter(qi.sub_iter)
	{ }


	template<class T>
	QuadTree_dfs_iterator(QuadTree<E>* t, const T& ok)
	{
		ok_quads = Acceptable<E>(ok);
		tree = t;
		last = 0;
		at_end = sub_iter = false;
		init_iter(t);
	}

	QuadTree_dfs_iterator(QuadTree<E>* t)
	 : ok_quads(), tree(t), last(0), at_end(false), sub_iter(false)
	{
		init_iter(t);
	}

	~QuadTree_dfs_iterator()
	{ }

	QuadTree_dfs_iterator& operator=(const QuadTree_dfs_iterator& qi)
	{
		ok_quads = qi.ok_quads;
		tree = qi.tree;
		last = qi.last;
		nodes = qi.nodes;
		current = qi.current;
		cur_end = qi.cur_end;
		at_end = qi.at_end;
		sub_iter = qi.sub_iter;
		return *this;
	}

	float get_height() const;

	reference operator*() const;

	reference operator*();

	const pointer operator->() const;

	pointer operator->();

	QuadTree_dfs_iterator& operator++() {
		find_next_node();
		return *this;
	}

	QuadTree_dfs_iterator operator++(int)
	{
		QuadTree_dfs_iterator i(*this);
		++(*this);
		return i;
	}

	void erase() { remove(); }

	void remove();

	bool operator==(const QuadTree_dfs_iterator& q) const {
		return (at_end && q.at_end) || (!at_end && !q.at_end && current == q.current);
	}
	bool operator!=(const QuadTree_dfs_iterator& q) const {
		return ! (*this == q);
	}
};


template<class E>
class QuadTree
{
	friend class QuadTree_dfs_iterator<E>;
	Node<E>* root;
	QuadTree(const QuadTree&);
	QuadTree operator=(const QuadTree&);

public:
	QuadTree(float xoff, float yoff, float sz);

	~QuadTree();

	void insert(E e);

	typedef QuadTree_dfs_iterator<E> iterator;

	iterator begin();

	iterator end() const;

	template<class T>
	iterator find(const T& t)
	{
		return iterator(this, t);
	}

	void erase(iterator i);

	template<class T>
	float max_height(const T& t)
	{
		float height = 0;
		iterator c, e = end();
		for (c = find(t); c != e; ++c) {
			height = std::max(height, c.get_height());
		}
		return height;
	}

	void update();
};


}
}

#endif


