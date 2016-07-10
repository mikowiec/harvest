#ifndef REAPER_MAIN_TYPES_PARAM_H
#define REAPER_MAIN_TYPES_PARAM_H

#include <vector>
#include "main/types.h"

//FIXME: Decide on proper location for these declarations
namespace reaper {

class MtxParams : public std::vector<Matrix> {
public:
	MtxParams(int size) : std::vector<Matrix>(size, Matrix::id()) {};

};

}
#endif

