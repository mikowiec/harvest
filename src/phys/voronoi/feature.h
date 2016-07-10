#ifndef REAPER_PHYS_FEATURE_H
#define REAPER_PHYS_FEATURE_H

#include <vector>
#include "main/types.h"


namespace reaper
{
namespace phys
{

class Cell;

/// Base of polyhedron feature object  
class Feature
{
public:
	Feature() {}; 

	typedef std::vector<Cell*>::iterator CellIterator;
	typedef std::vector<Cell*>::const_iterator ConstCellIterator;
	typedef std::back_insert_iterator<std::vector<Cell*> > CellInserter;

	CellIterator begin_cells() { return cells.begin(); }
	CellIterator end_cells()   { return cells.end(); }

	ConstCellIterator begin_cells() const { return cells.begin(); }
	ConstCellIterator end_cells()   const { return cells.end(); }

	CellInserter cell_inserter() { return std::back_inserter(cells); }

	virtual ~Feature() {}
protected:
	std::vector<Cell*> cells; ///< Voronoi region of feature
private:
	Feature(const Feature&);
	
};

}
}
#endif
