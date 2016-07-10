// coll_obj.cpp: implementation of the coll_obj class.
//
//////////////////////////////////////////////////////////////////////

#include "coll_obj.h"
#include <GL/glut.h>


int coll_obj::current_id = 0;

inline int coll_obj::get_id() const {return id;}

void coll_obj::render() const
{
        glPushMatrix();
        glTranslatef(position.x,position.y,position.z);
        glutSolidSphere(radius,8,8);
        glPopMatrix();
}

void coll_obj::simulate(const float dt)
{
        Vector acc(0,-9.8,0);

        velocity+=dt*acc;
        position+=dt*velocity;

        if(position.y < 0){
                position.y = -(position.y/2);
                velocity.y *= -1;
        }
}

