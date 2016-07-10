
#ifndef REAPER_MISC_COMMAND_H
#define REAPER_MISC_COMMAND_H

#include <deque>
#include "misc/sequence.h"
#include "misc/stlhelper.h"
#include "misc/smartptr.h"

namespace reaper {
namespace misc {


#ifdef MSVC

struct Null { };

template<class Arg = Null>
class uck
{
public:
	template<int arg>
	class CommandImpl {
	public:
	};

	template<>
	class CommandImpl<0> {
	public:
		virtual void operator()() = 0;
		virtual ~CommandImpl<0>() { }
	};
	template<>
	class CommandImpl<1>
	{
	public:
		virtual void operator()(Arg) = 0;
		virtual ~CommandImpl<1>() { }
	};
};



template<int arg = 0, class Arg = Null>
class Command
{
	misc::SmartPtr< uck<Arg>::CommandImpl<arg> > impl;
public:
	Command() : impl(0) { }
	Command(uck<Arg>::CommandImpl<arg>* i) : impl(i) { }
	Command(const Command& cmd) : impl(cmd.impl) { }
	void operator()() { if (impl.valid()) (*impl)(); }
	void operator()(Arg a) { if (impl.valid()) (*impl)(a); }
	Command& operator=(const Command& cmd)
	{
		impl = cmd.impl;
		return *this;
	}

	void disable() { impl.force_free(); }
	misc::SmartPtr< uck<Arg>::CommandImpl<arg> > get() { return impl; }
};

template<class O>
class ObjCaller : public uck<>::CommandImpl<0>
{
	O o;
public:
	ObjCaller(O oo) : o(oo) { }
	ObjCaller(const ObjCaller& oc) : o(oc.o) { }
	ObjCaller& operator=(const ObjCaller& oc)
	{
		o = oc.o;
		return *this;
	}

	void operator()() { o(); }
};

template<class O>
Command<> mk_cmd(O o)
{
	return Command<>(new ObjCaller<O>(o));
}



template<class S, class T>
class MemVoidArgCaller : public uck<>::CommandImpl<0>
{
	T* o;
	void (T::*mem)();
public:
	MemVoidArgCaller(S* t, void (T::*m)()) : o(t), mem(m) { }
	void operator()() { (o->*mem)(); }
};

template<class S, class T>
Command<0> mk_cmd(S* t, void (T::*m)())
{
	return Command<0>(new MemVoidArgCaller<S, T>(t, m));
}


template<class S, class T, class A>
class MemArgCaller : public uck<>::CommandImpl<0>
{
	T* o;
	void (T::*mem)(A);
	A arg;
public:
	MemArgCaller(S* t, void (T::*m)(A), A a) : o(t), mem(m), arg(a) { }
	void operator()() { (o->*mem)(arg); }
};

template<class S, class T, class A>
Command<0> mk_cmd(S* t, void (T::*m)( A),  A a)
{
	return Command<0>(new MemArgCaller<S, T, A>(t, m, a));
}

template<class S, class T, class A>
Command<0> mk_cmd(S* t, void (T::*m)(const A&), A a)
{
	return Command<0>(new MemArgCaller<S, T, const A&>(t, m, a));
}



template<class S, class T, class A>
class MemCaller : public uck<A>::CommandImpl<1> {
	T* o;
	void (T::*mem)(A);
public:
	MemCaller(S* t, void (T::*m)(A)) : o(t), mem(m) { }
	void operator()(A arg) { (o->*mem)(arg); }
};

template<class S, class T, class A>
Command<1, A> mk_cmd(S* t, void (T::*m)(A a))
{
	return Command<1, A>(new MemCaller<S, T, A>(t, m));
}


typedef Command<> Ping;

#else


struct Null { };

template<int arg, class Arg = Null>
class CommandImpl
{
public:
	virtual ~CommandImpl() { }
};

class CommandImpl<0>
{
public:
	virtual void operator()() = 0;
	virtual ~CommandImpl() { }
};

template<class Arg>
class CommandImpl<1, Arg>
{
public:
	virtual void operator()(Arg) = 0;
	virtual ~CommandImpl() { }
};

template<int arg = 0, class Arg = Null>
class Command
{
	misc::SmartPtr<CommandImpl<arg, Arg> > impl;
public:
	Command() : impl(0) { }
	Command(CommandImpl<arg, Arg>* i) : impl(i) { }
	Command(const Command& cmd) : impl(cmd.impl) { }
	void operator()() { if (impl.valid()) (*impl)(); }
	void operator()(Arg a) { if (impl.valid()) (*impl)(a); }
	Command& operator=(const Command& cmd)
	{
		impl = cmd.impl;
		return *this;
	}

	void disable() { impl.force_free(); }
	misc::SmartPtr< CommandImpl<arg,Arg> > get() { return impl; }
};

template<class O>
class ObjCaller : public CommandImpl<0>
{
	O o;
public:
	ObjCaller(O oo) : o(oo) { }
	ObjCaller(const ObjCaller& oc) : o(oc.o) { }
	ObjCaller& operator=(const ObjCaller& oc)
	{
		o = oc.o;
		return *this;
	}
	void operator()() { o(); }
};

template<class O>
Command<0> mk_cmd(const O& o)
{
	return Command<0>(new ObjCaller<O>(o));
}


template<class S, class T>
class MemVoidArgCaller : public CommandImpl<0>
{
	T* o;
	void (T::*mem)();
public:
	MemVoidArgCaller(S* t, void (T::*m)()) : o(t), mem(m) { }
	void operator()() { (o->*mem)(); }
};

template<class S, class T>
Command<0> mk_cmd(S* t, void (T::*m)())
{
	return Command<0>(new MemVoidArgCaller<S, T>(t, m));
}


template<class S, class T, class A>
class MemArgCaller : public CommandImpl<0>
{
	T* o;
	void (T::*mem)(A);
	A arg;
public:
	MemArgCaller(S* t, void (T::*m)(A), A a) : o(t), mem(m), arg(a) { }
	void operator()() { (o->*mem)(arg); }
};

template<class S, class T, class A>
Command<0> mk_cmd(S* t, void (T::*m)(A), A a)
{
	return Command<0>(new MemArgCaller<S, T, A>(t, m, a));
}

template<class S, class T, class A>
Command<0> mk_cmd(S* t, void (T::*m)(const A&), A a)
{
	return Command<0>(new MemArgCaller<S, T, const A&>(t, m, a));
}

template<class S, class T, class A>
class MemCaller : public CommandImpl<1, A>
{
	T* o;
	void (T::*mem)(A);
public:
	MemCaller(S* t, void (T::*m)(A)) : o(t), mem(m) { }
	void operator()(A arg) { (o->*mem)(arg); }
};

template<class S, class T, class A>
Command<1, A> mk_cmd(S* t, void (T::*m)(A a))
{
	return Command<1, A>(new MemCaller<S, T, A>(t, m));
}

typedef misc::Command<> Ping;

#endif



}
}

#endif


