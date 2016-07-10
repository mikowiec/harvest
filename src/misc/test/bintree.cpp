
#include <iostream>
#include <iterator>
#include <algorithm>
#include <stack>
#include <cstdlib>

#include "os/compat.h"
#include "misc/sequence.h"


template<class T>
class NodeIter 
;

template<class T>
class Node
{
	T val;
	Node<T>* left;
	Node<T>* right;
public:
	typedef int value_type;

	Node(const T& v, Node<T>* l = 0, Node<T>* r = 0) : val(v), left(l), right(r) { }

	bool is_leaf() const { return left == 0 && right == 0; }
	Node<T>* go_left() { return left; }
	Node<T>* go_right() { return right; }
	T value() const { return val; }
	NodeIter<T> push_back(const T& t) {
		if (t == val)
			return NodeIter<T>(this);
		else if (t < val) {
			if (left)
				return left->push_back(t);
			else {
				left = new Node<T>(t);
				return NodeIter<T>(left);
			}
		} else {
			if (right)
				return right->push_back(t);
			else {
				right = new Node<T>(t);
				return NodeIter<T>(right);
			}
		}
	}
	NodeIter<T> begin() {
		return NodeIter<T>(this);
	}
	NodeIter<T> end() {
		return NodeIter<T>();
	}
};


// Iterator for inorder traversal (depth-first-start-at-bottom ;)
template<class T>
class NodeIter
{
	std::stack<Node<T>*> up;
	Node<T>* p;
	void walk_left() {
		while (p->go_left()) {
//			std::cout << "walk left: " << p->value() << '\n';
			up.push(p);
			p = p->go_left();
		}
	}
public:
	typedef NodeIter<T> iterator;
	typedef NodeIter<const T> const_iterator;
	typedef T value_type;
	typedef T* pointer;
	typedef T& reference;
	typedef int size_type;
	typedef int difference_type;
	typedef std::input_iterator_tag iterator_category;

	NodeIter() : p(0) { }
	NodeIter(Node<T>* n) : p(n) { walk_left(); }

	bool operator==(const NodeIter<T>& n) { return p == n.p; }
	bool operator!=(const NodeIter<T>& n) { return p != n.p; }

	NodeIter<T>& operator++() {
		if (p->go_right()) {
//			std::cout << "walk right: " << p->value() << '\n';
			p = p->go_right();
			walk_left();
		} else {
			if (up.empty()) {
//				std::cout << "walk no more: " << p->value() << '\n';
				p = 0;
				return *this;
			} else {				
//				std::cout << "walk up: " << p->value() << '\n';
				p = up.top();
				up.pop();
			}
		}
		return *this;
	}
			
	T operator*() { return p->value(); }
	NodeIter<T>& operator=(const NodeIter<T>& t) { p = t.p; return *this; }
};

template<class T>
std::ostream& operator<<(std::ostream& os, const Node<T>& t)
{
	return os << t->value();
}

class f
{
	int a;
public:
	f() : a(1) { }
	int operator()(int i) { return i + ++a; }
};

class g
{
public:
	int operator()() { return std::rand() % 20; }
};
		

int main()
{
	Node<int> tree1(4);
	std::generate_n(std::back_inserter(tree1), 10, g());
	std::cout << "--------\n";
	std::copy(tree1.begin(), tree1.end(), std::ostream_iterator<int>(std::cout, "\n"));

	Node<int> tree2(10);
	std::transform(tree1.begin(), tree1.end(), std::back_inserter(tree2), f());
	std::cout << "--------\n";
	std::copy(tree2.begin(), tree2.end(), std::ostream_iterator<int>(std::cout, "\n"));

}


