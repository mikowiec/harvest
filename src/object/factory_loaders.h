#ifndef REAPER_OBJECT_FACTORY_LOADERS_H
#define REAPER_OBJECT_FACTORY_LOADERS_H


#include "misc/parse.h"
#include "main/types_io.h"

#include <iosfwd>

namespace reaper {
namespace object {


namespace factory {

template<class T>
bool read_spec_line(std::istream& is, const std::string& label, T& t)
{
	std::string var, val;
	misc::until(is, var, ":").get();
	misc::skip_spaces(is);

	if (misc::strequal(var, label)) {
		is >> t;
		misc::skip_crlf(is);
		return true;
	} else {
		misc::skipline(is);
		return false;
	}
}

/** Example loader.
    Reads only position
 */
template<class T>
class pos_loader
{
	typedef T (*CREATE_FUNC)(const Matrix &m, CompanyID c);
	CREATE_FUNC create;
public:
	pos_loader(CREATE_FUNC f) : create(f) {}

	T operator()(std::istream& is) {
		Point p;
		read_spec_line(is, "position", p);
		return create(Matrix(p), None);
	}
};

/** StaticBase loader.
    Reads position and company id
 */
template<class T>
class static_loader
{
	typedef T (*CREATE_FUNC)(const Matrix &m, CompanyID c);
	CREATE_FUNC create;
public:
	static_loader(int i) {}
	static_loader(CREATE_FUNC f) : create(f) {}

	T operator()(std::istream& is) {
		
		CompanyID id;
		Matrix m;

		read_spec_line(is, "company", id);
		read_spec_line(is, "matrix", m);

		T t = create(m, id);
//		printf("new: %x\n", t);
		return t;
	}
};


/** DynamicBase loader.
    Same as static_loader but this one also reads waypoints 
 */
template<class T>
class dynamic_loader
{
	typedef T (*CREATE_FUNC)(const Matrix &m, CompanyID c);
	CREATE_FUNC create;
public:
	dynamic_loader(CREATE_FUNC f) : create(f) {}

	T operator()(std::istream& is) {
		
		CompanyID id;
		Matrix m;
		read_spec_line(is, "company", id);
		read_spec_line(is, "matrix", m);

		T o = create(m, id);
//		printf("new: %x\n", o);
/*		
		Point p;
		while (read_spec_line(is, "waypoint", p)) {
			o->add_waypoint(p);
		}
*/

		return o; 
	}
};

}
}
}

#endif
