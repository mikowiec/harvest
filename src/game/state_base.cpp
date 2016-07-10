/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:32 $
 * $Log: state_base.cpp,v $
 * Revision 1.16  2002/12/29 17:04:32  pstrand
 * hw/ moved to os/
 *
 * Revision 1.15  2002/04/18 01:18:28  pstrand
 * scenario in data files now
 *
 * Revision 1.14  2002/04/11 01:19:08  pstrand
 * res_stream exceptions
 *
 * Revision 1.13  2002/03/11 10:50:48  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.12  2002/01/11 23:11:08  peter
 * upprensning, mm
 *
 * Revision 1.11  2001/11/26 10:46:02  peter
 * kompilatorgnäll... ;)
 *
 * Revision 1.10  2001/11/21 00:54:53  peter
 * döda, fixa, och annat...
 *
 * Revision 1.9  2001/08/20 16:57:21  peter
 * no delete anymore...
 *
 * Revision 1.8  2001/06/07 05:14:15  macke
 * Reaper v0.9
 *
 * Revision 1.7  2001/05/15 05:02:06  niklas
 * no message
 *
 * Revision 1.6  2001/05/14 21:12:13  peter
 * *** empty log message ***
 *
 * Revision 1.5  2001/05/14 12:38:45  niklas
 * textutskrift
 *
 * Revision 1.4  2001/05/14 11:38:27  niklas
 * inläsning m.h.a. res
 *
 * Revision 1.3  2001/05/13 17:25:42  niklas
 * info utskrift
 *
 * Revision 1.2  2001/05/10 11:40:09  macke
 * häpp
 *
 * Revision 1.1  2001/05/06 16:14:30  niklas
 * no message
 *
*/

#include "os/compat.h"
#include "game/scenario_states.h"
#include "game/scenario_mgr.h"
#include "os/reltime.h"
#include "os/debug.h"
#include "misc/font.h"
#include "misc/parse.h"
#include "res/res.h"

namespace reaper{
namespace game{
namespace scenario{




StateBase::StateBase(ObjectMgr& o, MsgQueue& m)
 : om(o), mq(m)
{
}

StateBase::~StateBase()
{ }


}
}
}
