#ifndef REAPER_GFX_CAMERA_H
#define REAPER_GFX_CAMERA_H

#include "world/query_obj.h"

namespace reaper {
namespace gfx {

class OldCamera
{
public:		
	Point eye;    ///< position
	Point center; ///< normalized forward vector
	Vector up;    ///< normalized up vector

	float horiz_fov;
	float vert_fov;
	
	/**@name Constructors
		 All given vectors must be normalized 
	*/
	///{
	OldCamera(const Point &e, const Point  &lookat, const Vector &u, float hfov, float vfov);
	///}

};
/// Camera properties
class Camera
{
public:		
	Point pos;    ///< position
	Vector front; ///< normalized forward vector
	Vector up;    ///< normalized up vector
	mutable world::Frustum frustum;

	float horiz_fov;
	float vert_fov;
	
	/**@name Constructors
		 All given vectors must be normalized 
	*/
	///{
	Camera() {}
	Camera(const OldCamera &e);
	Camera(const Point &e, const Point  &lookat, const Vector &u, float hfov, float vfov);
	Camera(const Point &e, const Vector &f,      const Vector &u, float hfov, float vfov);
	///}

	void set_lookat(const Point &p);
	const world::Frustum& setup_view() const; // sets projection/view-matrix and updates frustum
};

}
}
#endif

/*
 * $Author: fizzgig $
 * $Date: 2002/05/16 23:58:59 $
 * $Log: camera.h,v $
 * Revision 1.8  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.7  2001/08/06 12:16:11  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.6.4.1  2001/08/03 13:43:50  peter
 * pragma once obsolete...
 *
 * Revision 1.6  2001/07/06 01:47:09  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.5  2001/06/09 01:58:46  macke
 * Grafikmotor reorg
 *
 * Revision 1.4  2001/05/10 11:40:10  macke
 * häpp
 *
 */

