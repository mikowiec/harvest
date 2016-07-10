#ifndef REAPER_PHYS_POLYGON_H
#define REAPER_PHYS_POLYGON_H

#include <vector>

#include "main/types.h"
#include "point.h"

namespace reaper
{
namespace phys
{

class Edge;

class Polygon
{
public:
	typedef std::vector<Edge*>::iterator  EdgeIterator;
	typedef std::vector<Point*>::iterator PointIterator;

	typedef std::vector<Edge*>::const_iterator  ConstEdgeIterator;
	typedef std::vector<Point*>::const_iterator ConstPointIterator;

	typedef std::back_insert_iterator<std::vector<Edge*> > EdgeInserter;
	typedef std::back_insert_iterator<std::vector<Point*> > PointInserter;

	Polygon() {}

	const Vector &normal() const { return norm; }
	Vector &normal() { return norm; }

//	double distance_to(const Point &p) const;
//	bool overlap(const Polygon &p) const;

	PointIterator begin_points() { return points.begin(); }
	PointIterator end_points() { return points.end(); }
	
	EdgeIterator begin_edges() { return edges.begin(); }
	EdgeIterator end_edges() { return edges.end(); }

	ConstPointIterator begin_points() const { return points.begin(); }
	ConstPointIterator end_points()   const { return points.end(); }
	
	ConstEdgeIterator begin_edges() const { return edges.begin(); }
	ConstEdgeIterator end_edges()   const { return edges.end(); }

	PointInserter point_inserter() { return std::back_inserter(points); }

/*  	bool is_left_to(const Edge &e) const; */
/*  	bool is_right_to(const Edge &e) const; */
 protected:
	Vector norm;

	std::vector<Point*> points;  
	std::vector<Edge*> edges;    
};

}
}
#endif;
