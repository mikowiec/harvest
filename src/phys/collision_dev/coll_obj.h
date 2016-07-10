// coll_obj.h: interface for the coll_obj class.
//
//////////////////////////////////////////////////////////////////////

#ifndef COLLOBJ_H
#define COLLOBJ_H

#include "os/compat.h"
#include "main/types_ops.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using reaper::Point;
using reaper::Vector;

class coll_obj  
{
public:
        coll_obj() : radius(1),position(0,5,0),velocity(0,0,0) {id = current_id++;};
        coll_obj(float r) : radius(r),position(0,5+r,0),velocity(0,0,0) {id = current_id++;};
        coll_obj(float r,float x, float y, float z) :
        radius(r),position(x,y,z),velocity(0,0,0) {id = current_id++;};
        virtual ~coll_obj() {};
        void render() const;
        void simulate(float dt);
        int get_id() const;
        float get_radius() const {return radius;}
        Point get_pos() const {return position;}
private:
        float radius;
        Point position;
        Vector velocity;
        static int current_id;
        int id;
};

#endif // !COLLOBJ_H
