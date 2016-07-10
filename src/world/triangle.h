#ifndef WORLD_TRIANGLE_H
#define WORLD_TRIANGLE_H

#include "main/types.h"

namespace reaper
{
namespace world
{

class Triangle
{
public:

	Triangle(const Point &, const Point &, const Point &);

	const Point &a;
	const Point &b;
	const Point &c;

	Vector normal;

	//Vectorer för edges. edges[0] är vektorn mellan p1 och p2, 
	//edge 2 vektorn mellan p2 och p3, edges[2] mellan p3 och p1
	Vector edges[3];


	//Normaler för planen som går genom normalen för trianglarna och
	//respektive edge. Normalerna går utåt.
	Vector norms[3];
};

}
}

#endif


/*
 * $Author: pstrand $
 * $Date: 2002/04/08 21:27:46 $
 * $Log: triangle.h,v $
 * Revision 1.7  2002/04/08 21:27:46  pstrand
 * *** empty log message ***
 *
 * Revision 1.6  2001/08/06 12:16:47  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.5.2.1  2001/08/03 13:44:17  peter
 * pragma once obsolete...
 *
 * Revision 1.5  2001/07/30 23:43:35  macke
 * Häpp, då var det kört.
 *
 * Revision 1.4  2001/07/06 01:47:39  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 */
