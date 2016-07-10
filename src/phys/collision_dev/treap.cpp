#include "os/compat.h"
#include "treap.h"
#include "exceptions.h"
#include "os/debug.h"

namespace reaper{
namespace phys_dev{

using reaper::phys::phys_error;

template<class T,class CmpPrio, class dif,class CmpData ,class Eq>
bool Treap< T,CmpPrio,dif, CmpData,Eq >::gt(const T &pd1,const T &pd2) const
{
	return !( CmpPrio()(pd1,pd2) || Eq()(pd1,pd2) );	
}

template<class T,class CmpPrio, class dif, class CmpData ,class Eq>
bool Treap< T,CmpPrio, dif,CmpData,Eq >::insert(const Pairdat &pd)
{
	++the_size;

	Treap_node<T>* current = treap;
	Treap_node<T>* neue;

	// Is this the first insert and thus the root node?
	if(treap == NULL){
		treap = new Treap_node<T>(pd, NULL);
		return false;
	}

	
	bool position_not_found = true;

	// Verifying BinaryTree property

	while(position_not_found){
		reaper::debug::DebugOutput dout;


		if(Eq()(pd, current->dat)){
			//dout << "EXCEPTION -> Treap -> Element Already Stored" << endl;
			--the_size;
			return true;
		}

		if(CmpData()(pd, current->dat)){
			if(current->left != NULL){
				current = current->left;
			} else {
				current->left = new Treap_node<T>(pd, current);
				current->left->parent = current;
				position_not_found = false;
				neue = current->left;
			}
		} else {
			if(current->right != NULL){
				current = current->right;
			} else {
				current->right = new Treap_node<T>(pd, current);
				current->right->parent = current;
				position_not_found = false;
				neue = current->right;
			}
		}
	}


	// Verifying Heap Property

	while((neue->parent != NULL) && CmpPrio()(neue->dat, neue->parent->dat) ){
		//cout << neue->dat << endl;
		//cout << neue->parent << endl;
		rot_up(neue);
		//rot_up(neue);
	}

	return false;
}




template<class T,class CmpPrio, class dif, class CmpData ,class Eq  >
bool Treap< T,CmpPrio,dif, CmpData,Eq >::rot_left(Treap_node<T> *node)
{
	Treap_node<T>* tmp;

	// 3:
	if(node->parent != NULL){
		if(node->parent->left == node)
			node->parent->left = node->right;
		else if(node->parent->right == node)
			node->parent->right = node->right;
		else {
			throw phys_error( "Treap -> rot_left -> fatal error!");
		
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

template<class T,class CmpPrio, class dif, class CmpData ,class Eq  >
bool Treap< T,CmpPrio, dif,CmpData,Eq >::rot_right(Treap_node<T> *node)
{
	Treap_node<T>* tmp;

	// 3:
	if(node->parent != NULL){
		if(node->parent->left == node)
			node->parent->left = node->left;
		else if(node->parent->right == node)
			node->parent->right = node->left;
		else {
			throw phys_error("Treap -> rot_right -> fatal error!" );
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

template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
bool Treap< T,CmpPrio,dif, CmpData,Eq >::rot_up(Treap_node<T>* node)
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

template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
Treap_node<T>* Treap< T,CmpPrio, dif,CmpData,Eq >::find(const T pd)
{
	Treap_node<T>* current = treap;

	while ((current != NULL) && (!Eq()(pd, current->dat))){

		if(CmpData()(current->dat, pd)){
			current = current->right;
		} else {
			current = current->left;
		}

	}

	return current;

}


template<class T,class CmpPrio,class dif, class CmpData ,class Eq  >
bool Treap< T,CmpPrio, dif,CmpData,Eq >::remove(const Pairdat &pd)
{
//	static bool left;

	Treap_node<T>* current = find(pd);

	if(current == NULL)return true;

	for(;;){

		if( (current->left == NULL) && (current->right == NULL)){
			if(current->parent != NULL)
				if(current->parent->left == current)
					current->parent->left = NULL;
				else if(current->parent->right == current)
					current->parent->right = NULL;
				else {
					throw phys_error("Treap -> remove -> disintegrity warning!");
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
		else if (gt(current->right->dat,current->left->dat))
			rot_right(current);
		else
			rot_left(current);

	}
}
	
template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
bool Treap< T,CmpPrio, dif,CmpData,Eq >::rec_print(Treap_node<T>* node)
{
	if(node == NULL)return true;
	rec_print(node->left);
	cout << node->dat << endl;
	rec_print(node->right);
	return false;
}

template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
bool Treap< T,CmpPrio, dif,CmpData,Eq >::print(void)
{
	rec_print(treap);
	return false;
}

template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
T Treap< T,CmpPrio, dif,CmpData,Eq >::pop(void)
{
	if(treap == NULL) throw phys_error("Treap -> Cannot pop empty heap" );

	Pairdat pd = treap->dat;
	remove(pd);

	return pd;
}

template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
T Treap< T,CmpPrio, dif,CmpData,Eq >::top(void)
{
	return treap->dat;
}


template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
bool Treap< T,CmpPrio, dif,CmpData,Eq >::empty(void)
{
	return (treap == NULL);
}

template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
bool Treap< T,CmpPrio,dif, CmpData,Eq >::update(const Pairdat &pd)
{

	Treap_node<T>* nod = find(pd);

	// Cant update what isn't there!
	if( nod == NULL) throw phys_error("Treap -> Update -> Nothing to Update!" );

	nod->dat = pd;

	// Rotate Upwards if the cost has been reduced
	while( (nod->parent != NULL) && CmpPrio()(nod->dat,nod->parent->dat) )
		rot_up(nod);

	// Rotate Downwards if the cost has increased
	bool not_in_position = true;
	while(not_in_position){

		if(nod->left == NULL)
			if(nod->right == NULL)
				not_in_position = false;
			else if(gt(nod->dat, nod->right->dat))
				rot_left(nod);
			else
				not_in_position = false;
		else 
			if(nod->right == NULL)
				if(gt( nod->dat,nod->left->dat) )
					rot_right(nod);
				else
					not_in_position = false;
			else if( (gt(nod->dat,nod->left->dat)) || (gt( nod->dat, nod->right->dat)) )
				if(gt(nod->right->dat, nod->left->dat))
					rot_right(nod);
				else
					rot_left(nod);
			else
				not_in_position = false;

	}


	return false;
}

template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
bool Treap< T,CmpPrio,dif, CmpData,Eq >::find_nodes(Treap_node<T>* s_node,dif tst,std::vector<T>& objs)
{
	Treap_node<T>* current = s_node;

	if(current != NULL){

		if( tst(current->dat) ){
			objs.push_back(current->dat);
			find_nodes(current->right,tst,objs);
		} 
		else {
			find_nodes(current->right,tst,objs);
		}
		
		if(tst(current->dat) ){
			objs.push_back(current->dat);
			find_nodes(current->left,tst,objs);
		} 
		else {
			find_nodes(current->left,tst,objs);
		}

	}

	return false;
}

template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
bool Treap< T,CmpPrio,dif, CmpData,Eq >::delete_if(dif tst_f)
{
	std::vector<T> objs_to_delete;
	objs_to_delete.reserve(the_size);

	find_nodes(treap,tst_f,objs_to_delete);

	for(int i = 0;i < objs_to_delete.size(); ++i)
		remove(objs_to_delete[i]);

	return false;
}

template<class T,class CmpPrio, class dif,class CmpData ,class Eq  >
bool Treap< T,CmpPrio,dif, CmpData,Eq >::update_if(dif tst_f)
{
	std::vector<T> objs_to_modify;
	objs_to_modify.reserve(the_size);

	find_nodes(treap,tst_f,objs_to_modify);

	for(int i = 0;i < objs_to_modify.size(); ++i){
		objs_to_modify[i]->calc_lower_bound();
		update(objs_to_modify[i]);
	}

	return false;
}

}//phys_dev
}//reaper
