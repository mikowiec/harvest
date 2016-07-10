
#include "os/compat.h"

#include "net/sync.h"
#include "misc/test_main.h"
#include "main/types.h"

#include <iostream>



using namespace reaper;


class Ship
{
	Point pos;
	class ShipSync : public net::SyncIF
	{
		Ship* ship;
	public:
		ShipSync(Ship* s) : ship(s) { }

		bool get_change(net::change_spec&) {
			return false;
		}
		void update(const net::change_spec&) {
		}
	};
public:
	Ship()
	{
		net::Sync().inst().add(1, new ShipSync(this));

	}


};

int test_main()
{
	net::Synchronizer syncmgr;

	Ship* ship = new Ship();
	

//	syncmgr.update();
	

	return 0;
}

