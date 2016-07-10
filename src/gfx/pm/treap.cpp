/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:37 $
 * $Log: treap.cpp,v $
 * Revision 1.11  2002/12/29 17:04:37  pstrand
 * hw/ moved to os/
 *
 * Revision 1.10  2002/09/13 07:45:55  pstrand
 * gcc-varningar
 *
 * Revision 1.9  2001/11/26 10:46:04  peter
 * kompilatorgnäll... ;)
 *
 * Revision 1.8  2001/04/28 19:24:02  truls
 * Completely New Makeprog Function. Treap improved. 35 - 80 % Faster treap.update(). Correct Quadric Calculation. Penalties Correct. Etc.
 *
 * Revision 1.7  2001/04/20 07:27:09  macke
 * Headerfiler...
 *
 * Revision 1.6  2001/04/10 21:55:49  truls
 * slowheap, file-split etc
 *
 * Revision 1.5  2001/04/08 23:29:06  truls
 * don't use the pm, it's respawing!
 *
 * Revision 1.4  2001/04/08 17:48:53  truls
 * blah
 *
 * Revision 1.3  2001/04/07 23:11:45  truls
 * camera dependency etc
 *
 * Revision 1.2  2001/02/23 07:32:40  macke
 * hw/compat.h samt ett cvs-logg huvud
 *
 */

#include "os/compat.h"
#include "pm.h"
#include "treap.h"
#include "os/debug.h"

namespace reaper 
{
namespace gfx
{
namespace pm
{


bool Treap::pair_id(const Pairdat& pd1, const Pairdat& pd2)
{
	return ( 
			 (  (pd1.v[0] == pd2.v[0]) && (pd1.v[1] == pd2.v[1]) ) ||
		     (  (pd1.v[0] == pd2.v[1]) && (pd1.v[1] == pd2.v[0]) ) 
		   );
}

bool Treap::pair_lt(const Pairdat& pd1, const Pairdat& pd2)
{
	if(pd1.v[0] == pd2.v[0])
		return (pd1.v[1] < pd2.v[1]);

	return (pd1.v[0] < pd2.v[0]);
}


bool Treap::insert(const Pairdat &pd)
{
	++the_size;

	Treap_node_ptr current = treap;
	Treap_node_ptr neue = 0;

	// Is this the first insert and thus the root node?
	if(treap == NULL){
		treap = new Treap_node(pd, NULL);
		return false;
	}

	
	bool position_not_found = true;

	// Verifying BinaryTree property

	while(position_not_found){
		debug::DebugOutput dout;


		if(pair_id(pd, current->dat)){
			//dout << "EXCEPTION -> Treap -> Element Already Stored" << endl;
			--the_size;
			return true;
		}

		if(pair_lt(pd, current->dat)){
			if(current->left != NULL){
				current = current->left;
			} else {
				current->left = new Treap_node(pd, current);
				current->left->parent = current;
				position_not_found = false;
				neue = current->left;
			}
		} else {
			if(current->right != NULL){
				current = current->right;
			} else {
				current->right = new Treap_node(pd, current);
				current->right->parent = current;
				position_not_found = false;
				neue = current->right;
			}
		}
	}


	// Verifying Heap Property

	while((neue->parent != NULL) && (neue->dat.cost < neue->parent->dat.cost)){
		//cout << neue->dat << endl;
		//cout << neue->parent << endl;
		rot_up(neue);
		//rot_up(neue);
	}

	return false;
}




bool Treap::rot_left(Treap_node *node)
{
	Treap_node_ptr tmp;

	// 3:
	if(node->parent != NULL){
		if(node->parent->left == node)
			node->parent->left = node->right;
		else if(node->parent->right == node)
			node->parent->right = node->right;
		else {
			cout << "EXCEPTION: Treap -> rot_left -> fatal error!" << endl;
			cin.get();
		}
	} else {
		treap = node->right;
		//node->right->parent = NULL;
	}

	// 5:
	if(node->right->left != NULL)
		node->right->left->parent = node;
	
	// 4:
	tmp = NULL;
	if(node->right != NULL){
		node->right->parent = node -> parent;
		tmp = node->right->left;
		node->right->left = node;
	}

	// 2:
	node->parent = node->right;
	node->right = tmp;

	return false;
}

bool Treap::rot_right(Treap_node *node)
{
	Treap_node_ptr tmp;

	// 3:
	if(node->parent != NULL){
		if(node->parent->left == node)
			node->parent->left = node->left;
		else if(node->parent->right == node)
			node->parent->right = node->left;
		else {
			cout << "EXCEPTION: Treap -> rot_right -> fatal error!" << endl;
			cin.get();
		}
	} else {
		treap = node->left;
	}

	// 5:
	if(node->left->right != NULL)
		node->left->right->parent = node;


	// 4:
	tmp = NULL;
	if(node->left != NULL){
		node->left->parent = node->parent;
		tmp = node->left->right;
		node->left->right = node;
	}

	// 2:
	node->parent = node->left;
	node->left = tmp;

	return false;
}

bool Treap::rot_up(Treap_node_ptr node)
{
	if(node->parent == NULL){
		cout << "Cannot rot_up top node" << endl;
		return true;
	}

	//cout << "rot_up: " << node << ", " << node->parent << " " << node->left << " " << node->right << endl;

	if(node->parent->left == node)
		rot_right(node->parent);
	else
		rot_left(node->parent);

	return false;
}

Treap_node_ptr Treap::find(const Pairdat pd)
{
	Treap_node_ptr current = treap;

	while ((current != NULL) && (!pair_id(pd, current->dat))){

		if(pair_lt(current->dat, pd)){
			current = current->right;
		} else {
			current = current->left;
		}

	}

	return current;

}

bool Treap::remove(const Pairdat &pd)
{
//	static bool left;

	Treap_node_ptr current = find(pd);

	if(current == NULL)return true;

	for(;;){

		if( (current->left == NULL) && (current->right == NULL)){
			if(current->parent != NULL)
				if(current->parent->left == current)
					current->parent->left = NULL;
				else if(current->parent->right == current)
					current->parent->right = NULL;
				else {
					cout << "EXCEPTION: Treap -> remove -> disintegrity warning!" << endl;
					cin.get();
				}
			else
				treap = NULL;

			delete(current);

			--the_size;

			return false;
		}

		if(current->left == NULL)
			rot_left(current);
		else if (current->right == NULL) 
			rot_right(current);
		else if (current->right->dat.cost > current->left->dat.cost)
			rot_right(current);
		else
			rot_left(current);

	}
}
	
bool Treap::rec_print(Treap_node_ptr node)
{
	if(node == NULL)return true;
	rec_print(node->left);
	cout << node->dat << endl;
	rec_print(node->right);
	return false;
}

bool Treap::print(void)
{
	rec_print(treap);
	return false;
}

Pairdat Treap::pop(void)
{
	if(treap == NULL) cout << "EXCEPTION -> Treap -> Cannot pop empty heap" << endl;

	Pairdat pd = treap->dat;
	
	remove(pd);

	return pd;
}

Pairdat Treap::top(void)
{
	return treap->dat;
}


bool Treap::empty(void)
{
	return (treap == NULL);
}

bool Treap::update(const Pairdat &pd)
{

	Treap_node_ptr nod = find(pd);

	// Cant update what isn't there!
	if( nod == NULL) cout << "EXCEPTION -> Treap -> Update -> Nothing to Update!" << endl;

	// Rotate Upwards if the cost has been reduced
	while( (nod->parent != NULL) && (nod->dat.cost < nod->parent->dat.cost) )
		rot_up(nod);

	// Rotate Downwards if the cost has increased
	bool not_in_position = true;
	while(not_in_position){

		if(nod->left == NULL)
			if(nod->right == NULL)
				not_in_position = false;
			else if(nod->dat.cost > nod->right->dat.cost)
				rot_left(nod);
			else
				not_in_position = false;
		else 
			if(nod->right == NULL)
				if(nod->dat.cost > nod->left->dat.cost)
					rot_right(nod);
				else
					not_in_position = false;
			else if( (nod->dat.cost > nod->left->dat.cost) || (nod->dat.cost > nod->right->dat.cost) )
				if(nod->right->dat.cost > nod->left->dat.cost)
					rot_right(nod);
				else
					rot_left(nod);
			else
				not_in_position = false;

	}


	return false;
}

}
}
}
