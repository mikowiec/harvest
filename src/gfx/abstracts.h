#ifndef REAPER_GFX_ABSTRACTS_H
#define REAPER_GFX_ABSTRACTS_H

#include <string>
#include "misc/unique.h"

namespace reaper
{
class Matrix;
class MtxParams;
class Point;

namespace world { class Sphere; }
namespace gfx {

typedef reaper::misc::Unique RenderID;

/// Identifier to determine texture and type of effect (for state sorting)
class IDable
{
	const RenderID id;
	const RenderID texture;
public:
	IDable(const RenderID &rid, const RenderID &tex) : id(rid), texture(tex) {}

	const RenderID& get_id() const { return id; }
	const RenderID& get_texture() const { return texture; }
	virtual ~IDable() {}
};

/// Effects are transparent graphics that are drawn after solid objects.
class Effect : public IDable
{
public:
	Effect(const RenderID &id, const RenderID &tex) : IDable(id,tex) {}
	virtual const world::Sphere& get_sphere() = 0;

	virtual void setup() const = 0;   // pre render 
	virtual void render() = 0;        // rendering
	virtual void restore() const = 0; // post render
};

/// Effect that needs to be simulated for each frame
class SimEffect : public Effect
{
public:
	SimEffect(const RenderID &id, const RenderID &tex) : Effect(id,tex) {}

	virtual const Point& get_pos() const = 0;
	virtual float get_radius() const = 0;

	/// 'death' simulation (effect has been 'orphaned' and is expected to wither and die eventually.) 
	/**
	    This simulation is also used for object that never has a parent object, such as smoke stacks,
	    explosions, etc. Returns true when alive, false when expired. */
	virtual bool simulate_death(float dt) = 0;

	/// 'normal' simulation (effect is still 'owned' by an object and receives new data from it.)  
	/**  This is not used by particle systems/light flashes etc that are 'instant' created, upon
	     objects deaths, etc etc, so we define it to throw an exception to avoid putting bogus
	     implementations in each derived class. */
	virtual void simulate(float dt);
};

typedef reaper::misc::Unique RenderID;

// RenderInfo class used for describing meshes that are to be rendered
class RenderInfo {
	//FIXME: Linked list written by myself.. 
	RenderInfo *next_ri;
	RenderInfo* next() const { return next_ri; }

public:
	const Matrix *mtx;
	const MtxParams *param; // null if mesh is not parametric
	bool blend;

	float radius;
	RenderID mesh;
	RenderID texture;

	RenderInfo(const std::string &mesh_file, const Matrix &m, bool blend);
	RenderInfo(const std::string &mesh_file, const Matrix &m, MtxParams &param, bool blend);

	RenderInfo& operator=(const RenderInfo &);

	// just a link, no memory alloc/dealloc
	void link(RenderInfo &ri);		

	// const iterator
	class iterator : public std::iterator<std::forward_iterator_tag, RenderInfo>
	{
		const RenderInfo *ptr;
	public:
		explicit iterator(const RenderInfo *ri) : ptr(ri) {}

		const RenderInfo& operator*() { return *ptr; }
		const RenderInfo* operator->() { return ptr; }

		iterator& operator++() { ptr = ptr->next(); return *this; }
		iterator operator++(int) {
			iterator i = *this;
			++(*this);
			return i;
		}

		bool operator==(const iterator &i) const { return ptr == i.ptr; }
		bool operator!=(const iterator &i) const { return ptr != i.ptr; }
	};

	iterator begin() const { return iterator(this); }
	iterator end() const { return iterator(0); }

	friend class iterator;
};



class Hud
{
public:
	virtual void render() = 0;
};



/// This class is used to intialize/destroy static data in gfx-classes
/** Create a static object derived by this class, provide implementations
    for the functions and add it's adress to the initializer-list in gfx::Renderer
*/
class Initializer
{
public:
	virtual void init() = 0; // called upon construction of renderer
	virtual void exit() = 0;  // - " -       destruction - " - 
	virtual ~Initializer() { }
};

}
}
#endif
/*
 * $Author: pstrand $
 * $Date: 2003/06/04 16:29:57 $
 * $Log: abstracts.h,v $
 * Revision 1.24  2003/06/04 16:29:57  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.23  2002/09/13 07:45:55  pstrand
 * gcc-varningar
 *
 * Revision 1.22  2002/04/29 20:33:26  fizzgig
 * Added quadtree for particle-systems, plus shot->silly fix
 *
 * Revision 1.21  2002/04/09 03:36:21  pstrand
 * eff.ptr koll, shot-parent-id, quadtree-update-update
 *
 * Revision 1.20  2002/02/27 10:03:35  fizzgig
 * uhm..
 *
 * Revision 1.19  2002/02/26 12:44:01  fizzgig
 * Shadows on all objects
 *
 * Revision 1.18  2002/02/11 11:12:29  macke
 * shadows für alle .. dynamics.. :)
 *
 * Revision 1.17  2002/02/08 11:27:48  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.16  2001/11/20 21:44:09  picon
 * firing shots after restart now works..
 *
 * Revision 1.15  2001/08/06 12:16:10  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.14.2.1  2001/08/03 13:43:50  peter
 * pragma once obsolete...
 *
 * Revision 1.14  2001/07/30 23:43:14  macke
 * Häpp, då var det kört.
 *
 * Revision 1.13  2001/07/23 23:48:10  macke
 * Slimmad grafikhantering samt lite namnbyten
 *
 * Revision 1.12  2001/07/06 01:47:08  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.11  2001/06/09 01:58:45  macke
 * Grafikmotor reorg
 *
 * Revision 1.10  2001/06/07 05:14:15  macke
 * Reaper v0.9
 *
 * Revision 1.9  2001/05/10 11:40:10  macke
 * häpp
 *
 */
