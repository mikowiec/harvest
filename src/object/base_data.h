
#ifndef REAPER_OBJECT_BASE_DATA_H
#define REAPER_OBJECT_BASE_DATA_H

#include "object/base.h"
#include <string>

namespace reaper {
namespace object {

class SillyData {
	ID id;
	std::string name;

	friend class kill_handler;
	bool dead;

	const float radius;
	CompanyID company;
	Matrix m;

	friend class SillyBase;
public:
	event::Events* events;
	// fetches radius from mesh
	SillyData(const Matrix& mtx, CompanyID c, const std::string& n, ID i);
	// override radius
	SillyData(const Matrix& mtx, CompanyID c, const std::string& n, float radius, ID i);
	~SillyData();

	ID get_id() const; 
	CompanyID get_company() const;

	bool is_dead() const; 

	Point get_pos() const; 
	float get_radius() const; 

	const Matrix& get_mtx() const;
	void set_mtx(const Matrix& mtx);

	const std::string& get_name() const;
};

}
}

#endif

