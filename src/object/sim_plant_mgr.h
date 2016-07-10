#ifndef REAPER_OBJECT_SIM_PLANT_MGR_H
#define REAPER_OBJECT_SIM_PLANT_MGR_H

namespace reaper {
namespace object {
namespace plants {


struct SimPlant
{
	virtual void simulate() = 0;
	virtual void render() = 0;
};

SimPlant* mk_plant();


}
}
}

#endif

