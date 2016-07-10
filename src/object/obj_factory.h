/*
 * $Author: peter $
 * $Date: 2001/08/27 12:55:28 $
 */


#ifndef REAPER_OBJECT_OBJFACTORY_H
#define REAPER_OBJECT_OBJFACTORY_H

namespace reaper {

namespace object {

class SillyBase;
class MkInfo;

namespace factory {



SillyBase* gen_create(const MkInfo&);

}
}
}

#endif

