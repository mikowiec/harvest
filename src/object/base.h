#ifndef REAPER_OBJECT_BASE_H
#define REAPER_OBJECT_BASE_H

#include "main/types.h"
#include "main/enums.h"
#include "object/baseptr.h"

#include <memory>
#include <iosfwd>

namespace reaper {

namespace gfx { class RenderInfo; }
namespace sound { class Interface; }
namespace ai    { struct Message; }

namespace object {

namespace event { class Events; }



class SillyData;

class CollisionInfo;

class Base {
	/* Used to debug object destruction */
//	friend void obj_count_incr(Base*);
//	friend int get_obj_count();

public:
	Base();
	virtual ~Base();
};

struct GetStat;

/// Immovable, inactive simple objects (trees, ammoboxes)
class SillyBase : public Base {
public:
	/** Callbacks for various events occuring, other systems may hang on
	 * eventhandlers here, see OnDead in game/object_mgr.cpp for example
	 * usage.  Exerimental. Will be replaced by something better.
	 */
	virtual event::Events& events() = 0;

	virtual const gfx::RenderInfo* render(bool effects) const = 0;

	virtual const GetStat& stat() const = 0;

	virtual void initialize() = 0;
};

/// Immovable, active objects (gun turrets)
class StaticBase : public SillyBase {
public:
};


struct GetDyn;

/// Movable, active objects (ships, missiles)
class DynamicBase : public StaticBase {
public:
	virtual const GetDyn& dyn() const = 0;
};


/*
 * We could have one get() method instead of both stat() and dyn(),
 * since GetDyn < GetStat (using covariant return types),
 * but then we are forced to include object/accessor.h here..
 */

/*
 * The base classes are rather minimized at the moment, 
 * with the old methods removed or moved to object/accessor.h,
 * if motivated, some may be moved back here...
 */


/// Player controllable objects
class PlayerBase : public DynamicBase {
public:

}; 

/// Movable, inactive objects (lazer shots, cannon balls etc)
class ShotBase : public SillyBase {
public:


};


}
}

#endif

