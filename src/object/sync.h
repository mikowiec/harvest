

#include "net/sync.h"

#include "object/object_impl.h"

namespace reaper {
namespace object {
namespace sync {

class GenericSync;

enum UpdateType { Incr, Abs };

class GenericState : public net::State
{
	GenericSync* sync;
	Matrix mtx;
	Vector vel, rot_vel;
	bool is_dead;
public:
	GenericState(GenericSync*, const Matrix&, const Vector&, const Vector&, bool);
	void merge(const std::string&);
	std::string serialize();
};

class GenericSync : public net::SyncIF
{
	BasicDyn* acc;
	friend class GenericState;
	bool dead;
public:
	GenericSync(BasicDyn* a);
	net::State* get_current_state();
	void kill();
	bool is_dead();
};


}
}
}


