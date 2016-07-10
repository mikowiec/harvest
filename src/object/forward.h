
#ifndef REAPER_OBJECT_FORWARD_H
#define REAPER_OBJECT_FORWARD_H

namespace reaper {
namespace gfx {
	class RenderInfo;
}
namespace ai {
	struct Message;
}
namespace phys {
	class PrimAccessor;
	class SimAccessor;
	class DynAccessor;
	class ShotAccessor;
}
namespace object {

struct GetDyn;

class SillyData;
class SillyBase;
class StaticBase;
class DynamicBase;
class PlayerBase;

namespace weapon {
	class Weapon;
	class Missiles;

}

namespace controls {

	class Ship;
	class Turret;
	class GroundShip;

}

namespace current_data {
	class Engine;
	class Turret;
	class Ship;
	class GroundShip;
}

namespace data {
	class Engine;
}


namespace hull {
	class Standard;
	class Shielded;
}

namespace phys {
	class Turret;
	class Ship;
	class GroundShip;
	class Laser;
//	class ObjectAccessor;
}

namespace sound {
	class Base;
	class Static;
	class Projectile;
	class Ship;
	class Effect;
	class Accessor;
}

namespace renderer {
	class Shield;
	class Laser;
	class Ship;
	class Missile;
}
namespace sound {
	class Ship;
	class Turret;
	class Projectile;
	struct Laser;
	struct HeavyMissile;
	struct LightMissile;
	class Interface;
}

class ShotInfo;
class WeaponInfo;
class TurretInfo;
class ShipInfo;
class GroundShipInfo;

}
}

#endif

