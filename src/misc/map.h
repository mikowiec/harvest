#ifndef REAPER_MISC_MAP_H
#define REAPER_MISC_MAP_H

#include "os/debug.h"
#include <string>
#include <map>
#include <typeinfo>
#include "main/exceptions.h"
#include "misc/sequence.h"
#include "misc/free.h"

namespace reaper
{
namespace misc
{	
#define TEMPLATE_MISC_MAP template<class ID,class T>

/// Manages resource collections, resource objects will be constructed with identifier as constructor argument
TEMPLATE_MISC_MAP
class Map
{		
	typedef std::map<ID,T*> C;		///< container type
	C resources;				///< container	

	std::string type;	///< typename of stored objects
	debug::DebugOutput dout;

	template<class It>
	class iterator_base : public std::iterator<std::forward_iterator_tag,T>
	{
	protected:
		It iter;
	public:
		iterator_base() {}
		iterator_base(const It &i) : iter(i) {}
		iterator_base(const iterator_base &i) : iter(i.iter) {}

		iterator_base& operator++() { ++iter; return *this; }

		iterator_base operator++(int) {
			iterator i(*this);
			++(*this);
			return i;
		}

		bool operator!=(const It& i) const {
			return iter != i.iter;
		}
		bool operator==(const It& i) const {
			return iter == i.iter;
		}
	};
public:
	typedef iterator_base<typename C::const_iterator> iter_base;

	class const_iterator : public iter_base
	{		
	public:
		const_iterator() {}
		const_iterator(const typename C::const_iterator &i) : iter_base(i) {}
		const_iterator(const const_iterator &i) : iter_base(i.iter) {}

		const T& operator*() const { return *iter->second; }
		const T* operator->() const { return iter->second; }

		bool operator!=(const const_iterator& i) const {
			return iter != i.iter;
		}
		bool operator==(const const_iterator& i) const {
			return iter == i.iter;
		}
	};

	class iterator : public iter_base
	{
	public:
		iterator() {}
		iterator(const typename C::iterator &i) : iter_base(i) {}
		iterator(const iterator &i) : iter_base(i.iter) {}

		T& operator*() const { return *iter->second; }
		T* operator->() const { return iter->second; }

		bool operator!=(const iterator& i) const {
			return iter != i.iter;
		}
		bool operator==(const iterator& i) const {
			return iter == i.iter;
		}
	};

	class key_iterator : public iter_base
	{
	public:
		key_iterator() {}
		key_iterator(const typename C::iterator &i) : iter_base(i) {}
		key_iterator(const key_iterator &i) : iter_base(i.iter) {}

		const ID & operator*() { return iter->first; }
		const ID * operator->() { return iter->first; }

		bool operator!=(const key_iterator& i) const {
			return iter != i.iter;
		}
		bool operator==(const key_iterator& i) const {
			return iter == i.iter;
		}
	};

	Map(const std::string &type);
	~Map();

	/// Returns reference to resource, constructs if not available
	T& get(const ID& id);

	/// Returns reference, will probably die if not available!!!
	T& operator[](const ID& id);
	
	/// Returns pointer to resource or NULL if not loaded
	T* get_if(const ID &id);

	/// Inserts an object (overwrites existing object with same id)
	T& insert(T* t, const ID &id);

	/// Removes resource from collection, returns false if resource did not exist
	bool remove(const ID &id);

	/// Returns true if resource is constructed
	bool is_loaded(const ID &id) const;

	/// Returns true if controlled sequence is empty
	bool empty() const;

	/// Iterates over all objects in manager
	///{
	iterator begin()             { return iterator(resources.begin()); }
	iterator end()               { return iterator(resources.end());   }
	const_iterator begin() const { return const_iterator(resources.begin()); }
	const_iterator end() const   { return const_iterator(resources.end());   }
	///}

	key_iterator begin_key()     { return key_iterator(resources.begin()); }
	key_iterator end_key()       { return key_iterator(resources.end());   }

	/// Returns the number of loaded objects
	int size() const { return resources.size(); };

	/// Frees all loaded objects
	void purge();
};

TEMPLATE_MISC_MAP
Map<ID,T>::Map(const std::string &t) : type(t), dout(t + " map")
{
	//dout << "created\n";
}

TEMPLATE_MISC_MAP
Map<ID,T>::~Map()
{
	//dout << "destroyed\n";
	purge();
}

TEMPLATE_MISC_MAP
T& Map<ID,T>::get(const ID& id) 
{
	typename C::iterator i = resources.find(id);

	if(i == resources.end()) {
		T* t = new T(id);
//		dout << "Constructing: " << id << " count: " << size()+1 << " ptr: " << t << '\n';
		return *(resources[id] = t); // new T(id));
	} else {
		return *i->second;
	}
}

template<class T>
inline std::string to_str(T) { return "<unknown>"; }
inline std::string to_str(std::string s) { return s; }

TEMPLATE_MISC_MAP
T& Map<ID,T>::operator[](const ID& id)
{
	typename C::iterator i = resources.find(id);
	if (i == resources.end())
		throw reaper_error(std::string("Map<") + typeid(ID).name() + ", " + typeid(T).name() + "> not found: " + to_str(id));
	return *i->second; ///@todo FIXME !!! ARGH!!
}

TEMPLATE_MISC_MAP
T* Map<ID,T>::get_if(const ID &id)
{
	typename C::iterator i = resources.find(id);

	if(i == resources.end()) {
		return 0;
	} else {
		return i->second;
	}
}


TEMPLATE_MISC_MAP
T& Map<ID,T>::insert(T* t, const ID &id)
{
//	dout << "Inserting: " << id << " count: " << size()+1 << '\n';
	return *(resources[id] = t);
}


TEMPLATE_MISC_MAP
bool Map<ID,T>::remove(const ID &id)
{
	typename C::iterator i = resources.find(id);

	if(i == resources.end()) {
		return false;
	} else {
//		dout << "Removing:  " << id << " count: " << size()-1 << '\n';
		delete i->second;
		resources.erase(i);
		return true;
	}
}

TEMPLATE_MISC_MAP
bool Map<ID,T>::is_loaded(const ID &id) const
{
	return resources.find(id) != resources.end();
}

TEMPLATE_MISC_MAP
bool Map<ID,T>::empty() const
{
	return resources.empty();
}

TEMPLATE_MISC_MAP
void Map<ID,T>::purge()
{
//	dout << "Purging " << size() << '\n';

//	for(typename C::iterator i = resources.begin(); i != resources.end(); ++i) {
//		dout << "delete: " << i->second << '\n';
//		delete i->second;
//	}
	misc::for_each(seq(resources), misc::delete_it);

	resources.clear();
}
}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:39 $
 * $Log: map.h,v $
 * Revision 1.22  2002/12/29 17:04:39  pstrand
 * hw/ moved to os/
 *
 * Revision 1.21  2002/01/09 13:57:38  peter
 * Yet Another Vector Implementation, this time SimpleYetFlexible(tm)  ;)
 *
 * Revision 1.20  2002/01/07 14:00:30  peter
 * resurs och ljudmeck
 *
 * Revision 1.19  2002/01/01 23:25:49  peter
 * felhantering..
 *
 * Revision 1.18  2001/12/24 14:40:29  peter
 * 0.93
 *
 * Revision 1.17  2001/12/17 21:11:45  peter
 * *** empty log message ***
 *
 * Revision 1.16  2001/12/13 17:03:29  peter
 * småfixar...
 *
 * Revision 1.15  2001/08/06 12:16:30  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.14.4.1  2001/08/03 13:44:06  peter
 * pragma once obsolete...
 *
 * Revision 1.14  2001/07/06 01:47:26  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.13  2001/05/08 00:14:57  macke
 * dynamiska ljus.. buggar lite dock, så de är bortkommenterade..
 *
 * Revision 1.12  2001/04/24 12:11:32  peter
 * hmm...
 *
 * Revision 1.11  2001/04/24 10:07:11  peter
 * gcc blev förvirrad, vet inte varför (delvis)...
 *
 */

