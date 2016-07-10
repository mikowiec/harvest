/*
 * $Author: peter $
 * $Date: 2001/08/06 12:16:18 $
 * $Log: treap.h,v $
 * Revision 1.4  2001/08/06 12:16:18  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.3.4.1  2001/08/03 13:43:55  peter
 * pragma once obsolete...
 *
 * Revision 1.3  2001/07/06 01:47:16  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.2  2001/03/15 00:13:44  peter
 * lite småfix bara
 *
 * Revision 1.1  2001/02/19 02:53:54  macke
 * PM för terrängen, riktig dimma, multitexture fix och lite mer
 *
 */

#ifndef REAPER_GFX_TREAP_H
#define REAPER_GFX_TREAP_H

#include "pm.h"

namespace reaper 
{
namespace gfx
{
namespace pm
{

struct Treap_node;

typedef Treap_node* Treap_node_ptr;

struct Treap_node{
	Pairdat dat;
	Treap_node_ptr left, right, parent;

	Treap_node(void):left(NULL), right(NULL), parent(NULL){}
	Treap_node(const Pairdat& pd, Treap_node_ptr p):dat(pd), left(NULL), right(NULL), parent(p){}
};



class Treap
{
private:
	int the_size;

	Treap_node_ptr treap;

	bool rot_left(Treap_node *node);
	bool rot_right(Treap_node *node);
	bool rot_up(Treap_node_ptr node);

	bool pair_id(const Pairdat& pd1, const Pairdat& pd2);
	bool pair_lt(const Pairdat& pd1, const Pairdat& pd2);

	bool rec_print(Treap_node_ptr node);



public:
	Treap_node_ptr find(const Pairdat pd);

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
}
#endif
