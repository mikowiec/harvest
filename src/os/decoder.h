/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:12 $
 * $Revision: 1.2 $
 */


#ifndef REAPER_HW_DECODER_H
#define REAPER_HW_DECODER_H

#include "res/res.h"

namespace reaper {
namespace os {


template<class Source>
class Decoder
{
public:
	virtual bool init(res::res_stream*) = 0;
	virtual Source* get() = 0;
	virtual ~Decoder() { }
};


}
}


#endif

