
#ifndef REAPER_OBJECT_OBJECTREF_H
#define REAPER_OBJECT_OBJECTREF_H

#include "misc/smartptr.h"
#include "object/id.h"

namespace reaper {
namespace object {
	class SillyBase;
	class StaticBase;
	class DynamicBase;
	class PlayerBase;
	class ShotBase;

	typedef misc::SmartPtr<object::PlayerBase> PlayerPtr;
	typedef misc::WeakPtr<object::PlayerBase> PlayerWPtr;
	typedef misc::SmartPtr<object::SillyBase> SillyPtr;
	typedef misc::WeakPtr<object::SillyBase> SillyWPtr;
	typedef misc::SmartPtr<object::StaticBase> StaticPtr;
	typedef misc::WeakPtr<object::StaticBase> StaticWPtr;
	typedef misc::SmartPtr<object::DynamicBase> DynamicPtr;
	typedef misc::WeakPtr<object::DynamicBase> DynamicWPtr;
	typedef misc::SmartPtr<object::ShotBase> ShotPtr;
	typedef misc::WeakPtr<object::ShotBase> ShotWPtr;

}

}


#endif
