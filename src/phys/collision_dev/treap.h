
#ifndef REAPER_TREAP_H
#define REAPER_TREAP_H

#include <functional>
#include <vector>
using std::equal_to;

namespace reaper{
namespace phys_dev{

template< class T > struct Treap_node;

template< class T >
struct Treap_node{
	
	typedef T Pairdat;

	Pairdat dat;
	Treap_node<T>* left;
	Treap_node<T>* right;
	Treap_node<T>* parent;

	Treap_node(void):left(NULL), right(NULL), parent(NULL){}
	Treap_node(const Pairdat& pd, Treap_node<T>* p):dat(pd), left(NULL), right(NULL), parent(p){}
};

template<class T> 
class falseC
{
	bool operator()(const T& ob) {return false;}
};


template< class T, class CmpPrio, class dif = falseC<T>, class CmpData = std::less<T> , class Eq = std::equal_to<T> >
class Treap
{
	typedef T Pairdat;

private:
	int the_size;

	Treap_node<T>* treap;

	bool rot_left(Treap_node<T> *node);
	bool rot_right(Treap_node<T> *node);
	bool rot_up(Treap_node<T>* node);

	bool rec_print(Treap_node<T>* node);

	bool gt(const T&,const T&) const;
	bool find_nodes(Treap_node<T>*,dif f,std::vector<T>&);

public:
	Treap_node<T>* find(const Pairdat pd);
	bool delete_if(dif p);
	bool update_if(dif p);

	Treap(void):the_size(0), treap(NULL) {}

	bool insert(const Pairdat &pd);
	bool update(const Pairdat &pd);
	bool remove(const Pairdat &pd);
	bool empty(void);

	bool print(void);

	Pairdat pop(void);

	Pairdat top(void);

	int size(void){return the_size;}

};

}
}


#endif
