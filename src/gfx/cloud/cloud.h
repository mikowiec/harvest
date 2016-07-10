
#ifndef REAPER_GFX_CLOUD_CLOUD_H
#define REAPER_GFX_CLOUD_CLOUD_H

#include <deque>

namespace reaper {
namespace gfx {

class Camera;

namespace cloud {

class CloudEffect;

typedef std::deque<CloudEffect*> Clouds;


class CloudSystem
{
	Clouds clouds;
public:
	void init();
	void simul();
	void update(const Camera& cam, const Matrix& m);
};


}
}
}

#endif

