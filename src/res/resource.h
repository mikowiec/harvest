
#ifndef REAPER_RES_RESOURCE_H
#define REAPER_RES_RESOURCE_H

#include <string>
#include <map>
#include <set>
#include <typeinfo>
#include <memory>
#include "main/exceptions.h"
#include "misc/smartptr.h"
#include "misc/base.h"
#include "misc/free.h"
#include "misc/sequence.h"
#include "misc/singleton.h"
#include "misc/prim_iter_impl.h"
#include "res/res.h"

namespace reaper {
namespace res {

typedef std::string Ident;
typedef const Ident& IdentRef;

typedef misc::prim_iter_if<Ident>* EntIter;

class Managed
{
public:

	Managed(IdentRef);
	virtual ~Managed();

	virtual int flush() = 0;

	virtual EntIter list() = 0;
};

class Mgr
{
	std::map<Ident, Managed*> nodes;
public:
	void add(IdentRef, Managed*);
	void del(Managed*);

	misc::prim_iter_if< std::pair<const Ident,Managed*> >* roots();

	void flush(); // call flush on all roots
	void shutdown(); // flush everything (shutdown)
};

Mgr& mgr();

define_reaper_exception(res_error);

template<class T> class NodeConfig;

template<class T>
struct tp {
	typedef misc::SmartPtr<T> ptr;
	typedef NodeConfig<T>* cfg;
	typedef T& ref;
};



template<class T>
class NodeConfig
{
public:
	typedef typename tp<T>::ptr Ptr;

	virtual Ptr create(IdentRef) = 0;
	virtual void save(IdentRef, const T&) { }
};



template<class T>
void create_error(Ident id)
{
	throw res_error(std::string("Don't know how to create ") +
	                            id + " :: " + typeid(T).name());
}

template<class T>
class ResNode : misc::NoCopy, public Managed
{
	typename tp<T>::cfg config;
	typedef std::map<Ident, typename tp<T>::ptr> Ents;
	typedef typename Ents::iterator Ents_i;
	Ents ents;

	typename tp<T>::ptr get(IdentRef id) {
		typename tp<T>::ptr p = config->create(id);
		if (p == 0)
			create_error<T>(id + " (create failed)");
		return p;
	}

public:
	ResNode() : Managed(typeid(T).name()) { }

	~ResNode() {
		flush();
		delete config;
	}

	void set_config(typename tp<T>::cfg cfg) { config = cfg; }

	typename tp<T>::ptr lookup(IdentRef id) {
		Ents_i i = ents.find(id);
		return (i != ents.end()) ? i->second : 0;
	}

	void save(IdentRef id) {
		typename tp<T>::ptr p = lookup(id);
		if (p != 0 && config != 0)
			config->save(id, *p);
	}

	typename tp<T>::ptr find_ptr(IdentRef id) {
		typename tp<T>::ptr p = lookup(id);
		if (p != 0)
			return p;
		p = get(id);
		ents[id] = p;
		return p;
	}

	int flush() {
		ents.clear();
		return 42;
	}


	virtual EntIter list() {
		std::set<Ident>* s = new std::set<Ident>();
		Ents_i c, e = ents.end();
		for (c = ents.begin(); c != e; ++c)
			s->insert(c->first);
		return misc::mk_owned_iter(s);
	}
};


template<class T>
struct G {
	typedef misc::singleton::Wrap< ResNode<T> > W;
	typedef misc::singleton::Accessor< W > Acc;
	typedef misc::singleton::Impl< W > Impl;
};


template<class T>
ResNode<T>& inst()
{
	return G<T>::Acc().inst();
}


template<class T>
class ScopeNode
{
	ResNode<T> n;
public:
	ScopeNode(typename tp<T>::cfg cfg) : n(cfg) { }
	~ScopeNode() { }
	ResNode<T>& node() { return n; }
};




/* below is just accessors to ResNode<T> members, no logic */

template<class T>
typename tp<T>::ref resource(ScopeNode<T>& s, const Ident& a)
{
	return *s.node().find_ptr(a);
}

template<class T>
typename tp<T>::ptr resource_ptr(ScopeNode<T>& s, const Ident& a)
{
	return s.node().find_ptr(a);
}

template<class T>
typename tp<T>::ref resource(ResNode<T>& r, const Ident& a)
{
	return *r.find_ptr(a);
}

template<class T>
typename tp<T>::ptr resource_ptr(ResNode<T>& r, const Ident& a)
{
	return r.find_ptr(a);
}

template<class T>
typename tp<T>::ref resource(const Ident& a)
{
	return *inst<T>().find_ptr(a);
}

template<class T>
typename tp<T>::ptr resource_ptr(const Ident& a)
{
	return inst<T>().find_ptr(a);
}

template<class T>
void define_node(NodeConfig<T>* cfg)
{
	/* prepare for new creaton */
	G<T>::Impl::prepare();
	/* create new node and configure it */
	G<T>::Acc()->set_config(cfg);
}

template<class T>
void remove_node()
{
	/* invalidate */
	G<T>::Impl::shutdown();
}


template<class T>
void save(const Ident& path)
{
	inst<T>().save(path);
}


}
}

#endif

