#include "os/compat.h"

#include "gfx/shadow/utility.h"
#include "gfx/abstracts.h"
#include "gfx/managers.h"
#include "object/accessor.h"

namespace reaper {
namespace gfx {
namespace shadow {

void render_shadow(const SillyBase &o)
{
	const float inv_radius = 0.99f / o.stat().radius(); // some margin needed

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();	
	glRotatef(-90, 1, 0, 0);
	Matrix mtx = o.stat().mtx();
	mtx.pos(Point(0, 0, 0));
	glMultMatrix(mtx);
	glScalef(inv_radius, inv_radius, inv_radius);

	MeshRef mr(MeshMgr::get_ref());
	TextureMgr::get_ref()->use("");

	const RenderInfo *ri = o.render(false);
	for(RenderInfo::iterator i = ri->begin(); i != ri->end(); ++i) {
		mr->render(*i, 0);
	}
}

}
}
}
