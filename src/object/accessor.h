#ifndef REAPER_OBJECT_ACCESSOR_H
#define REAPER_OBJECT_ACCESSOR_H

#include "main/types.h"
#include "main/enums.h"

#include "object/id.h"

namespace reaper {
namespace object {

struct GetInfo
{
	virtual ~GetInfo();

	/* Identifier use for message passing,
	 * and possibly mapping against subparts.
	 */

	virtual CompanyID company() const = 0;
	virtual ID id() const = 0;
};

struct GetStat : public GetInfo
{
	virtual ~GetStat();

	virtual Matrix mtx() const = 0;
	virtual Point pos() const; /* Redundant or convenient? */
	virtual float radius() const = 0;

};

struct GetDyn : public GetStat
{
	virtual ~GetDyn();

	virtual Vector velocity() const = 0;
	virtual Vector rot_velocity() const = 0;
};

struct SetStat
{
	virtual ~SetStat();

	virtual void set_mtx(const Matrix&) = 0;
};

struct SetDyn : public SetStat
{
	virtual ~SetDyn();

	virtual void set_velocity(const Vector&) = 0;
	virtual void set_rot_velocity(const Vector&) = 0;
};

/*
 * Gah!  set_ prefixes ought to be unnecessary, but without
 *       them the calls are ambiguous (if both get&set are inherited)
 */

struct BasicDyn : public GetDyn, public SetDyn { };

BasicDyn* basic_accessor(ID id, CompanyID cid, const Matrix& m, float rad);
BasicDyn* basic_accessor_ext_mtx(ID id, CompanyID cid, Matrix& m, float rad);

class SillyData;
BasicDyn* basic_accessor(SillyData&);


}
}

#endif

