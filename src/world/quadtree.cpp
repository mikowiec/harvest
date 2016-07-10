



#include "os/compat.h"

#include <limits>
#include <iostream>
#include <cassert>

#include "gfx/abstracts.h"
#include "gfx/light.h"
#include "main/types.h"
#include "main/types_io.h"
#include "main/types_ops.h"
#include "misc/free.h"
#include "misc/sequence.h"
#include "misc/smartptr.h"
#include "object/accessor.h"
#include "object/base.h"
#include "os/debug.h"
#include "world/geometry.h"
#include "world/geometry.h"
#include "world/quadtree.h"

namespace reaper {
namespace quadtree {

debug::DebugOutput derr("qt", 5);


/*
 conventions used in this file:
  (Point, Point) denotes a 3d-box
  (Point2D, Point2D) denotes a rectangle in the xz-plane
  (Point, float) denotes a sphere
  (Point2D, float) denotes a circle in the xz-plane (or an infinite cylinder)

 this will be made type-safe at some point...
*/



const int Out = 0, LL = 1, LR = 2, UL = 4, UR = 8, All = 15;

/** A square.
 *  With the lower-left corner located at (xoff, yoff),
 *  and a side length of 2 * half_size.
 */

class Square
{
public:
	float xoff, yoff, half_size;

	Square() : xoff(0), yoff(0), half_size(0) { }

	Square(float x, float y, float sz)
	 : xoff(x), yoff(y), half_size(sz)
	{ }

	/** Return a shrinked square, towards the given quadrant. */
	Square shrinked(Quadrant q) const;
	/** Return a shrinked square, towards the given quadrant. */
	Square shrinked_loose(Quadrant q) const;
	/** Returns a (8x) grown square */
	Square grown() const;
};


float margin(float sz)
{
	return sz/4;
}

Square Square::shrinked(Quadrant q) const
{
	Square sq(xoff, yoff, half_size);
	switch (q) {
	case LL:  break;
	case LR:  sq.xoff += half_size; break;
	case UR:  sq.xoff += half_size;
	case UL:  sq.yoff += half_size; break;
	}
	sq.half_size /= 2;
	return sq;
}

Square Square::shrinked_loose(Quadrant q) const
{
	float sz = half_size;
	float mg = margin(sz);
	float new_hs = (sz+mg)/2;
	switch (q) {
	case LL:  return Square(xoff, yoff, new_hs);
	case LR:  return Square(xoff+sz-mg, yoff, new_hs);
	case UL:  return Square(xoff, yoff+sz-mg, new_hs);
	case UR:  return Square(xoff+sz-mg, yoff+sz-mg, new_hs);
	}
	return *this; // dummy
}


Square Square::grown() const
{
	Square sq(xoff, yoff, half_size);
	sq.half_size *= 2;
	sq.xoff -= sq.half_size;
	sq.yoff -= sq.half_size;
	sq.half_size *= 2;
	return sq;
}

/** Calculate which subsquare a given rectangle occupies.
 *  \param ll lower-left corner
 *  \param ur upper-right corner
 *  \param sq square
 *  \return The occupied quadrant, or Ext if any boundary is crossed.
 */

Quadrant get_quadrant(const Point2D& ll, const Point2D& ur, const Square& sq);

Quadrant get_quadrant(const Point& p1, const Point& p2, const Square& sq);
Quadrant get_quadrant(Point2D c, float r, const Square& sq);
Quadrant get_quadrant(Point c, float r, const Square& sq);


Quadrant get_quadrant(const Point2D& ll, const Point2D& ur, const Square& sq)
{
	float sz = sq.half_size;
	float ll_x = ll.x - sq.xoff;
	float ur_x = ur.x - sq.xoff;
	float ll_y = ll.y - sq.yoff;
	float ur_y = ur.y - sq.yoff;

	float mg = margin(sz);

	Quadrant q = Out;

	if (ll_x < sz+mg && ll_y < sz+mg)
		q |= LL;

	if (ur_y >= sz-mg && ll_x < sz+mg)
		q |= UL;

	if (ur_x >= sz-mg && ll_y < sz+mg)
		q |= LR;

	if (ur_x >= sz-mg && ur_y >= sz-mg)
		q |= UR;

	return q;
}


bool inside(Point2D p1, Point2D p2, Square sq)
{
	float sz = sq.half_size*2;
	return (p1.x > sq.xoff && p1.y > sq.yoff &&
	        p2.x < sq.xoff+sz && p2.y < sq.yoff+sz);

}

bool overlap(Quadrant q)
{
	switch (q) {
	case LL: return false;
	case UL: return false;
	case LR: return false;
	case UR: return false;
	default: return true;
	}
}

Quadrant get_quadrant(Point2D c, float r, const Square& sq)
{
	return get_quadrant(Point2D(c.x - r, c.y - r),
			    Point2D(c.x + r, c.y + r), sq);
}

Quadrant get_quadrant(Point c, float r, const Square& sq)
{
	return get_quadrant(Point2D(c.x - r, c.z - r),
			    Point2D(c.x + r, c.z + r), sq);

}


Quadrant get_quadrant_strict(Point2D p1, Point2D p2, const Square& sq)
{
	if (inside(p1, p2, sq.shrinked_loose(LL)))
		return LL;
	if (inside(p1, p2, sq.shrinked_loose(LR)))
		return LR;
	if (inside(p1, p2, sq.shrinked_loose(UL)))
		return UL;
	if (inside(p1, p2, sq.shrinked_loose(UR)))
		return UR;
	return All;
}



/* A box for elements, buffering common computations.  */
template<class Elem>
struct Box
{
	world::Sphere sp;
	Elem elem;
	bool deleted;
private:
public:
	Box(const Box& b);
	Box(float x, float y) {
		sp.p.x = x;
		sp.p.y = 0;
		sp.p.z = y;
		sp.r = 0;
	}

	Box(const world::Sphere& s)
	 : sp(s), deleted(true)
	{ }

	Box(const world::Sphere& s, Elem e)
	 : sp(s), elem(e), deleted(false)
	{ }
	~Box() { }

	void update(const world::Sphere& s)
	{
		sp = s;
	}

	void clear()
	{
		deleted = true;
		elem = 0;
	}

	const Elem& unbox() const { return elem; }
	Elem& unbox() { return elem; }
	bool operator<(const Box& o) const {
		return (sp.p.x+sp.p.z) < (o.sp.p.x+o.sp.p.z);
	}
	const Point& pos() const { return sp.p; }
	float radius() const { return sp.r; }

	float get_height() const { return sp.p.y+sp.r; }

	bool operator==(const Elem& e) const { return elem == e; }
	bool operator!=(const Elem& e) const { return elem != e; }

};

template<class T>
struct BoxComp {

	int operator()(const Box<T>* b1, const Box<T>* b2) const {
		return *b1 < *b2;
	}
	
};

template<class T>
struct VectorOps {
	typedef std::vector< Box<T>* > t;
	typedef typename t::iterator iterator;

	static iterator lower_bound(t& s, const Box<T>& v) {
		return s.begin();
	}
	static iterator upper_bound(t& s, const Box<T>& v) {
		return s.end();
	}
	static void insert(t& s, Box<T>* c) {
		s.push_back(c);
	}

	static void clear(t& s) {
		misc::for_each(misc::seq(s), misc::delete_it);
	}

	static iterator begin(t& s) { return s.begin(); }
	static iterator end(t& s)   { return s.end(); }

	static void sort(t& s) {
	}
};

template<class T>
struct SortedVectorOps {
	typedef std::vector< Box<T>* > t;
	typedef typename t::iterator iterator;

	static iterator lower_bound(t& s, const Box<T>& v) {
		return std::lower_bound(s.begin(), s.end(), &v, BoxComp<T>());
	}
	static iterator upper_bound(t& s, const Box<T>& v) {
		return std::upper_bound(s.begin(), s.end(), &v, BoxComp<T>());
	}
	static void insert(t& s, Box<T>* c) {
		s.push_back(c);
	}

	static void clear(t& s) {
		misc::for_each(misc::seq(s), misc::delete_it);
	}

	static iterator begin(t& s) { return s.begin(); }
	static iterator end(t& s)   { return s.end(); }

	static void sort(t& s) {
		qt_sort(s.begin(), s.end(), BoxComp<T>());
	}
};


template<class T>
struct NodeContOps : public SortedVectorOps<T>
{ };


template<class T>
Point get_pos(misc::SmartPtr<T> p) {
	return p->stat().pos();
}

Point get_pos(gfx::light::Light*) {
	return Point();
}

Point get_pos(world::Triangle* t) {
	return world::midpoint(world::mintri(t->a, t->b, t->c),
			       world::maxtri(t->a, t->b, t->c));
}


Point get_pos(gfx::SimEffect* e)
{
	return e->get_sphere().p;
}

template<class T>
float radius(misc::SmartPtr<T> p) {
	return p->stat().radius();
}

float radius(gfx::light::Light* t) {
	return 0;
}

float radius(world::Triangle* t) {
	return length(world::mintri(t->a, t->b, t->c) -
		      world::maxtri(t->a, t->b, t->c)) / 2.0;
}

float radius(gfx::SimEffect* e)
{
	return e->get_sphere().r;
}

/** Quadrant-calculator for a SillyObject
 *  (or any such object) with a bounding sphere.
 */

template<class Elem>
struct Geom
{
	static float get_height(Elem e)
	{
		return e->get_pos().y + e->get_radius();
	}

	static Quadrant quadrant_strict(Elem e, const Square& sq)
	{
		float r = radius(e);
		Point pos = get_pos(e);
		Point2D p1(pos.x-r, pos.z-r);
		Point2D p2(pos.x+r, pos.z+r);
		return get_quadrant_strict(p1, p2, sq);
	}

	static Quadrant quadrant(Elem e, const Square& sq)
	{
		float r = radius(e);
		Point pos = get_pos(e);
		return get_quadrant(pos, r, sq);
	}

	static bool intersect(const Box<Elem>& b, const Point& center, float radius)
	{
		return world::intersect(b.pos(), b.radius(), center, radius);
	}


	static bool intersect(const Box<Elem>& b, const Point2D& center, float radius)
	{
		Point2D pos(b.pos().x, b.pos().z);
		return world::intersect(pos, b.radius(), center, radius);
	}

	static bool intersect(const Box<Elem>& b, const Point2D& ll, const Point2D& ur)
	{
		Point p = b.pos();
		float r = b.radius();
		return world::intersect(ll, ur, Point2D(p.x - r, p.z - r), Point2D(p.x + r, p.z + r));
	}

	static bool intersect(const Box<Elem>& b, const world::Line& l)
	{
		Point p = b.pos();
		float r = b.radius();
		return world::intersect(Point(p.x - r, p.y - r, p.z - r),
				        Point(p.x + r, p.y + r, p.z + r), l);
	}

	static bool intersect(const Box<Elem>& b, const world::Frustum& f)
	{
		return world::intersect(b.pos(), b.radius(), f);
	}

	static world::Sphere calc_bsphere(Elem e)
	{
		return world::Sphere(get_pos(e), radius(e));
	}

};

using world::mintri;
using world::maxtri;
using world::mintri2;
using world::maxtri2;

/** Quad-helper for triangles */

struct Geom<world::Triangle*>
{
	typedef world::Triangle Tri;
	static float get_height(Tri* t)
	{
		return world::max(t->a.y, t->b.y, t->c.y);
	}
	

	static Quadrant quadrant_strict(Tri* t, const Square& sq)
	{
		Point2D p1 = mintri2(t->a, t->b, t->c);
		Point2D p2 = maxtri2(t->a, t->b, t->c);
		return get_quadrant_strict(p1, p2, sq);
	}

	static Quadrant quadrant(Tri* t, const Square& sq)
	{
		Quadrant q = quadtree::get_quadrant(mintri(t->a, t->b, t->c),
					      maxtri(t->a, t->b, t->c), sq);
		return quadtree::get_quadrant(mintri(t->a, t->b, t->c),
					      maxtri(t->a, t->b, t->c), sq);
	}

	static bool intersect(const Box<Tri*>& b, const Point& center, float radius)
	{
		return world::intersect(b.pos(), b.radius(), center, radius);
	}

	static bool intersect(const Box<Tri*>& b, const Point2D& center, float radius)
	{
		return world::intersect(Point2D(b.pos().x, b.pos().z), b.radius(), center, radius);
	}

	static bool intersect(const Box<Tri*>& b, const Point2D& ll, const Point2D& ur)
	{
		return world::intersect(ll, ur, Point2D(b.pos().x, b.pos().z), b.radius());
	}

	static bool intersect(const Box<Tri*>& b, const world::Frustum& f)
	{
		return world::intersect(*b.unbox(), f);
	}

	static bool intersect(const Box<Tri*>& b, const world::Line& l)
	{
		return world::intersect(b.pos(), b.radius(), l);
	}

	static world::Sphere calc_bsphere(Tri* t)
	{
		const Point t1(mintri(t->a, t->b, t->c));
		const Point t2(maxtri(t->a, t->b, t->c));
		const Point rad = (t2 - t1) / 2.0;
		return world::Sphere(t1 + rad, length(rad));
	}

};

template<class E>
Acceptable<E>& Acceptable<E>::operator=(const Acceptable<E>& ac)
{
	what = ac.what;
	bsp = ac.bsp;
	p1 = ac.p1;
	p2 = ac.p2;
	frustum = ac.frustum;
	return *this;
}

template<class E>
Quadrant Acceptable<E>::quadrant(const Square& sq) const {
	switch (what) {
	case All:     return LL+LR+UL+UR;
	case Rect:    return get_quadrant(Point2D(p1.x, p1.z), Point2D(p2.x, p2.z), sq);
	case Cyl:
	case Frustum:
	case Sphere:  return get_quadrant(bsp.p, bsp.r, sq);
	case Line:    return get_quadrant(world::minpt2(p1, p2), world::maxpt2(p1, p2), sq);
	}
	return LL+LR+UL+UR;
}


template<class E>
bool Acceptable<E>::acceptable(const Box<E>& b) const {
	switch (what) {
	case All:     return true;
	case Rect:    return Geom<E>::intersect(b, Point2D(p1.x, p1.z), Point2D(p2.x, p2.z));
	case Cyl:     return Geom<E>::intersect(b, Point2D(bsp.p.x, bsp.p.z), bsp.r);
	case Sphere:  return Geom<E>::intersect(b, bsp.p, bsp.r);
	case Line:    return Geom<E>::intersect(b, world::Line(p1, p2));
	case Frustum: return Geom<E>::intersect(b, frustum);
	}
	return false;
}

template<class E>
typename Acceptable<E>::NS::iterator Acceptable<E>::find_begin(typename NS::t& set, float max_size)
{
	if (what != All) {
		float d = 2*bsp.r + 2*max_size;
		Box<E> box(bsp.p.x - d, bsp.p.z - d);
		return NodeContOps<E>::lower_bound(set, box);
	} else {
		return set.begin();
	}
}

template<class E>
typename Acceptable<E>::NS::iterator Acceptable<E>::find_end(typename NS::t& set, float max_size)
{
	if (what != All) {
		float d = 2*bsp.r + 2*max_size;
		Box<E> box(bsp.p.x + d, bsp.p.z + d);
		return NodeContOps<E>::upper_bound(set, box);
	} else {
		return set.end();
	}
}

template<class E>
float Acceptable<E>::min_height() const {
	switch (what) {
	case Frustum:
	case Sphere:
		return bsp.p.y - bsp.r;
	case Line:
		return min(p1.y, p2.y);
	default:
		return -1e20; // FIXME
	}
}

template<class E>
Acceptable<E>::Acceptable(const world::Line& l)
 : what(Line), p1(l.p1), p2(l.p2)
{
	Point rad((p2-p1)/2);
	bsp.p = p1 + rad;
	bsp.r = length(rad);
}

template<class Iter, class T, class F>
inline void isort1(Iter first, Iter last, T, const F& f)
{
	for (Iter i = first + 1; i != last; ++i) {

		T val = *i;
		if (f(val, *first)) {
			copy_backward(first, i, i + 1);
			*first = val;
		}
		else
		{
			Iter lst = i;
			Iter next = i;
			--next;
			while (f(val, *next)) {
				*lst = *next;
				lst = next;
				--next;
			}
			*lst = val;
		}
	}
}

template<class Iter, class T, class F>
inline void isort2(Iter first, Iter last, T, F f)
{
	for (Iter i = first + 1; i != last; ++i) {
		T x = *i;
		Iter j = i-1;
		while (j >= first && f(x, *j)) {
			*(j+1) = *j;
			--j;
		}
		*(j+1) = x;
	}
}

template <class Iter, class F>
void qt_sort(Iter first, Iter last, F f)
{
	if (first == last) return; 
//	isort1(first, last, *first, f);
//	isort2(first, last, *first, f);
	std::sort(first, last, f);
}



template<class E>
class Node
{
	friend class QuadTree<E>;
	friend class QuadTree_dfs_iterator<E>;

	enum NodeType { IntNode, Leaf };
	NodeType state;
	Square geom;

	Node* quad[4];
	typename NodeCont<E>::t elems;

	float max_size;
	float max_height;
	int depth;

public:
	typedef NodeCont<E> NS;
	typedef NodeContOps<E> NSOps;
	typedef typename NodeCont<E>::iterator iterator;

	Node(const Square& sq, int depth);
	~Node();
	void insert(Box<E>*);

	bool empty() const;

	void update(Node<E>*);
//	void remove(const Quadtree_dfs_iterator<E>& i);

};

template<class E>
bool QuadTree_dfs_iterator<E>::test() {
	Box<E>* b = *current;
	if (b->elem == 0) {
		assert(b->deleted);
		return false;
	}
	assert(!b->deleted);
	if (ok_quads.acceptable(*b)) {
		return true;
	}
	return false;
}

template<class E>
void QuadTree_dfs_iterator<E>::find_next_node() {
	/* 
	 * Think of this code as a state machine, rather
	 * than as a function.
	 * It has two valid states, either with "current"
	 * pointing to a intersected node, or at the end
	 * of the container (at_end == true).
	 */

	while (!nodes.empty()) {
		Node_t* p = nodes.top();
		last = p;

		if (ok_quads.min_height() > p->max_height) {
			nodes.pop();
			continue;
		}

		/* First visit the internal nodelist... */
		bool sub_at_end = false;
		while (!sub_at_end && !p->elems.empty()) {
			if (! sub_iter) {
				current = ok_quads.find_begin(p->elems, p->max_size);
				cur_end = ok_quads.find_end(p->elems, p->max_size);
				if (current == cur_end) {
					sub_at_end = true;
				} else {
					sub_iter = true;
					if (test())
						return;
				}
			} else {
				++current;
				if (current != cur_end) {
					if (test())
						return;
				} else {
					sub_at_end = true;
				}
			}
		}
		sub_iter = false;
	
		/* ...then children */
		nodes.pop();
		Quadrant acpt = ok_quads.quadrant(p->geom);
		assert(acpt);
		if (acpt & LL && p->quad[0])
			nodes.push(p->quad[0]);
		if (acpt & LR && p->quad[1])
			nodes.push(p->quad[1]);
		if (acpt & UL && p->quad[2])
			nodes.push(p->quad[2]);
		if (acpt & UR && p->quad[3])
			nodes.push(p->quad[3]);
	}
	at_end = true;
}

template<class E>
void QuadTree_dfs_iterator<E>::remove()
{
	if (current == last->elems.end()) {
		derr << "remove: warning, end-iterator\n";
		return;
	}
	Box<E>* b = *current;
	b->clear();
}

template<class E>
void QuadTree_dfs_iterator<E>::init_iter(QuadTree<E>* t) {
	if (t->root->empty()) {
		tree = 0;
		at_end = true;
	} else {
		nodes.push(t->root);
		find_next_node();
	}
}

template<class E>
typename QuadTree_dfs_iterator<E>::reference
    QuadTree_dfs_iterator<E>::operator*() const {
	return (**current).unbox();
}

template<class E>
typename QuadTree_dfs_iterator<E>::reference
    QuadTree_dfs_iterator<E>::operator*() {
	return (**current).unbox();
}

template<class E>
const typename QuadTree_dfs_iterator<E>::pointer
    QuadTree_dfs_iterator<E>::operator->() const {
	return &(**current).unbox();
}

template<class E>
typename QuadTree_dfs_iterator<E>::pointer
    QuadTree_dfs_iterator<E>::operator->() {
	return &(**current).unbox();
}

template<class E>
float QuadTree_dfs_iterator<E>::get_height() const {
	return (**current).get_height();
}

template<class E>
bool Node<E>::empty() const
{
	return (elems.empty() &&
			(!quad[0] || quad[0]->empty()) &&
			(!quad[1] || quad[1]->empty()) &&
			(!quad[2] || quad[2]->empty()) &&
			(!quad[3] || quad[3]->empty()));
}


template<class E>
void Node<E>::update(Node<E>* root)
{
	for (int i = 0; i < 4; ++i)
		if (quad[i])
			quad[i]->update(root);

	Point2D ll(geom.xoff, geom.yoff);
	Point2D ur(ll + Point2D(2*geom.half_size, 2*geom.half_size));

	typename NodeCont<E>::t tmp;
	std::vector< Box<E>* > reinsert;
	std::swap(elems, tmp);
	iterator c, e = NSOps::end(tmp);
	for (c = NSOps::begin(tmp); c != e; ++c) {
		Box<E>* b = *c;
		if (b->deleted) {
			delete b;
		} else {
			b->update(Geom<E>::calc_bsphere(b->unbox()));
			if (Geom<E>::intersect(*b, ll, ur))
				NSOps::insert(elems, b);
			else
				reinsert.push_back(b);
		}
	}
	NSOps::sort(elems);
	tmp.clear();
	misc::for_each(misc::seq(reinsert),
		std::bind1st(std::mem_fun(&Node<E>::insert), root));
}

void indent(int n) {
	for (int i = 0; i < n; ++i)
		putchar(' ');
}


template<class T> struct QuadTreeInfo { };

using std::max;
using std::min;

template<class E>
Node<E>::Node(const Square& sq, int d)
 : geom(sq), max_size(0), max_height(-1), depth(d)
{
	state = (depth < QuadTreeInfo<E>::max_depth) ? IntNode : Leaf;
	quad[0] = quad[1] = quad[2] = quad[3] = 0;
}

template<class E>
Node<E>::~Node()
{
	NSOps::clear(elems);
	for (int i = 0; i < 4; ++i) {
		delete quad[i];
	}
}

template<class T>
struct deref_eq
{
	T t;
	deref_eq(T tt) : t(tt) { }
	deref_eq(const deref_eq& d) : t(d.t) { }
	bool operator()(T t2) const { return t->elem == t2->elem; }
};

template<class NS, class T, class B>
void insert_to_leaf(T& elems, B b)
{
	NS::insert(elems, b);
}

int quad2idx(Quadrant q)
{
	switch (q) {
	case LL: return 0;
	case LR: return 1;
	case UL: return 2;
	case UR: return 3;
	}
	assert(false);
	return -1;
}

template<class E>
void Node<E>::insert(Box<E>* b) {
	max_size = max(b->radius(), max_size);
	max_height = max(b->get_height(), max_height);
	switch (state) {
	case Leaf:
		insert_to_leaf<NSOps>(elems, b);
		break;
	case IntNode:
		Quadrant q = Geom<E>::quadrant_strict(b->unbox(), geom);
		if (overlap(q)) {
			insert_to_leaf<NSOps>(elems, b);
		} else {
			assert(q);
			int i = quad2idx(q);
			if (!quad[i])
				quad[i] = new Node(geom.shrinked_loose(q), depth+1);
			quad[i]->insert(b);
		}
		assert(!overlap(q) || elems.size() > 0);
		break;
	}
}

template<class E>
void QuadTree<E>::insert(E e)
{
	Box<E>* b = new Box<E>(Geom<E>::calc_bsphere(e), e);
	root->insert(b);
}

template<class E>
QuadTree<E>::QuadTree(float xoff, float yoff, float sz)
{
	root = new Node<E>(Square(xoff,yoff,sz/2), 0);
	float d_max = std::numeric_limits<float>::max();
	float d_min = -d_max;
}

template<class E>
QuadTree<E>::~QuadTree()
{
	delete root;
}


template<class E>
typename QuadTree<E>::iterator QuadTree<E>::begin()
{
	return iterator(this);
}

template<class E>
typename QuadTree<E>::iterator QuadTree<E>::end() const
{
	return iterator();
}


template<class E>
void QuadTree<E>::erase(iterator i)
{
	if (i != iterator())
		i.remove();
	else
		derr << "Warning, erase on end iterator\n";
}

template<class E>
void QuadTree<E>::update()
{
	root->update(root);
}


#define INST_QUADTREE(T, N, M) \
	template class QuadTree<T>; \
	template class Acceptable<T>; \
	template class Node<T>; \
	template class QuadTree_dfs_iterator<T>; \
	struct QuadTreeInfo<T> { \
		enum { max_depth = N }; \
		enum { max_tmp_elems = M }; \
	};

INST_QUADTREE(world::Triangle*, 6, 1);
INST_QUADTREE(object::SillyPtr, 5, 4);
INST_QUADTREE(object::StaticPtr, 4, 4);
INST_QUADTREE(object::DynamicPtr, 4, 1);
INST_QUADTREE(object::ShotPtr, 4, 1);
INST_QUADTREE(gfx::light::Light*, 3, 8);
INST_QUADTREE(gfx::SimEffect*, 3, 8);


}
}


