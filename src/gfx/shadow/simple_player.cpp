#include "os/compat.h"

#include <iterator>

#include "gfx/shadow/main.h"
#include "gfx/shadow/utility.h"

namespace reaper {
namespace gfx {
namespace shadow {

SimpleShadow::SimpleShadow() :
	tr(TextureMgr::get_ref())
{}

bool SimpleShadow::add_shadow(const SillyBase &o, const Frustum &f, float r)
{
	return gen_shadow_triangles_if(o, f, r, 
		std::back_inserter(points), 
		std::back_inserter(texcoords),
		std::back_inserter(colors));
}

int SimpleShadow::render(const Frustum &frustum) 
{ 
	object::DynamicPtr player = wr->get_local_player();

	if (player.valid() && add_shadow(*player, frustum, 0.75f)) {
		tr->use("smoke");
		render_triangles();
		return 1;
	} else {
		return 0;
	}		
}

void SimpleShadow::render_triangles()
{
	if(points.empty()) {
		return;
	}
		
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
	ClientStateKeeper s1(GL_COLOR_ARRAY, true);
	ClientStateKeeper s2(GL_TEXTURE_COORD_ARRAY, true);

	glVertexPointer(3, GL_FLOAT, sizeof(Point), &points[0]);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ByteColor), &colors[0]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(TexCoord), &texcoords[0]);
	
	glDrawArrays(GL_TRIANGLES,0,points.size());

	// don't reallocate, only 'destruct' elements (it's all PODs)
	points.clear();
	texcoords.clear();
	colors.clear();
}

}
}
}

