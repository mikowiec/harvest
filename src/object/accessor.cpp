

#include "os/compat.h"

#include "object/accessor.h"
#include "object/base_data.h"

namespace reaper {

namespace object {

GetInfo::~GetInfo() { }
GetDyn::~GetDyn() { }
SetDyn::~SetDyn() { }
GetStat::~GetStat() { }
SetStat::~SetStat() { }

Point GetStat::pos() const { return mtx().pos(); }


/* The dynamic part */
class DynAccImpl : public BasicDyn
{
	Vector vel, rot_vel;
public:
	DynAccImpl() : vel(0,0,0), rot_vel(0,0,0) { }
	Vector velocity() const     { return vel; }
	Vector rot_velocity() const { return rot_vel; }
	void set_velocity(const Vector& v)      { vel = v; }
	void set_rot_velocity(const Vector& rv) { rot_vel = rv; }
};


/* With external matrix */
class ExtBasicAccessor : public DynAccImpl
{
	ID oid;
	CompanyID cid;
	Matrix& m;
	float rad;
public:
	ExtBasicAccessor(ID oi, CompanyID ci, Matrix& nm, float r)
	 : oid(oi), cid(ci), m(nm), rad(r)
	{ }
	ID id() const               { return oid; }
	CompanyID company() const   { return cid; }
	Matrix mtx() const          { return m; }
	float radius() const        { return rad; }
	void set_mtx(const Matrix& nm)          { m = nm; }
};


/* With SillyData-chunk */
class BasicSillyAccessor : public DynAccImpl
{
	SillyData& data;
	Vector vel, rot_vel;
public:
	BasicSillyAccessor(SillyData& d)
	 : data(d)
	{ }
	ID id() const               { return data.get_id(); }
	CompanyID company() const   { return data.get_company(); }
	Matrix mtx() const          { return data.get_mtx(); }
	float radius() const        { return data.get_radius(); }
	void set_mtx(const Matrix& m)           { data.set_mtx(m); }
};


/* With internal matrix */
struct Mtx {
	Matrix mx;
	Mtx(const Matrix& mi) : mx(mi) { }
};
class BasicAccessor : private Mtx, public ExtBasicAccessor
{
public:
	BasicAccessor(ID id, CompanyID cid, const Matrix& mi, float r)
	 : Mtx(mi), ExtBasicAccessor(id, cid, mx, r)
	{ }
};




BasicDyn* basic_accessor_ext_mtx(ID id, CompanyID cid, Matrix& m, float rad)
{
	return new ExtBasicAccessor(id, cid, m, rad);
};

BasicDyn* basic_accessor(ID id, CompanyID cid, const Matrix& m, float rad)
{
	return new BasicAccessor(id, cid, m, rad);
};

BasicDyn* basic_accessor(SillyData& d)
{
	return new BasicSillyAccessor(d);
};

}
}

