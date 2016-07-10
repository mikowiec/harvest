
#include "os/compat.h"

#include "os/debug.h"
#include "world/world.h"
#include "object/base.h"
#include "object/object_impl.h"
#include "main/types_io.h"

#include "misc/test_main.h"

namespace reaper {
namespace gfx {
	class RenderInfo;
}
}

using namespace reaper;
using namespace reaper::world;
using namespace reaper::object;

class TestObj
 : public SillyBase
{
	SillyData data;
public:

	TestObj(const Matrix& mtx)
	 : SillyBase(data), data(mtx, Nature, "foo", 1.0f)
	{
		derr << "id " << get_id() << '\n';
	}
	virtual const gfx::RenderInfo* render(bool effects) const
	{
		return 0;
	}
	virtual void collide(const CollisionInfo&) 
	{
	}
};

int test_main()
{
	WorldRef w = World::create();
	w->load("empty_level");

	for (int i = 0; i < 10; ++i) {
		w->add_object(new Triangle(Point(i,i,i), Point(i,i+1,i), Point(i,i+2,i+4)));
		w->add_object(SillyPtr(new TestObj( Matrix(Vector(i*5,0,0)))));
	}

	si_iterator c, e = w->end_si();

	int i = 0;
	float sum = 0;
	for (c = w->begin_si(); c != e; ++c, ++i) {
		derr << c->get_pos() << '\n';
		sum += c->get_pos().x;
	}

	derr << "---------\n";
	if (i != 10 || sum != 225) {
		derr << "i " << i << " sum " << sum << '\n';
		return 1;
	}

	Cylinder cyl(Point2D(40, 0), 15);
	i = 0, sum = 0;
	for (c = w->find_si(cyl); c != e; ++c, ++i)	{
		derr << c->get_pos() << '\n';
		sum += c->get_pos().x;
	}
	derr << "i " << i << " sum " << sum << '\n';
	return 0;
}

