#ifndef REAPER_NET_SYNC_H
#define REAPER_NET_SYNC_H


#include "os/compat.h"

#include "misc/singleton.h"
#include "net/net.h"
#include "os/reltime.h"

#include <string>

namespace reaper {
namespace net {

typedef os::time::RelTime Time;
typedef int ID;
typedef int SyncID;
typedef std::string change_spec;

struct State
{
	virtual ~State();
	virtual void merge(const std::string&) = 0;
	virtual std::string serialize() = 0;
};

struct SyncIF
{
	virtual ~SyncIF();
	virtual State* get_current_state() = 0;	
};

struct Synchronizer_impl;

class Synchronizer : public misc::singleton::Base
{
	Synchronizer_impl* impl;
public:
	Synchronizer();
	~Synchronizer();
	void add(ID id, SyncIF*);
	void spec_local_id(ID);
	void update(StateReps&);
};


typedef misc::singleton::Impl<Synchronizer> SyncImpl;
typedef misc::singleton::Accessor<Synchronizer> Sync;


}
}


#endif
