#ifndef REAPER_GFX_FPS_METER_H
#define REAPER_GFX_FPS_METER_H

#include "os/abstime.h"
#include <vector>


namespace reaper
{
namespace gfx
{
	/// Draws an Framerate counter over the area (0,0)-(1,1)
	class FPSMeter
	{
		int array_index;
		float frames;
		std::vector<float> v;
		float total_time;
		os::time::TimeSpan prev_time;
	public:
		FPSMeter(int average_frames = 30)
		 : array_index(0), frames(0), v(average_frames, 1/50.0f), total_time(0),
		   prev_time(os::time::get_time())
		{}

		void render(bool blend = false, bool vertical = false);
	};

	/** As FPSMeter, but also sets up the viewport.
	 *  (quite crude, draws at the lower 60 pixels of the 
	 *  max viewport size.
	 */
	class FPSMeterVP
	{
		FPSMeter fps;
		int vp[4];
	public:
		FPSMeterVP();
		void reinit();
		void render();
//		void render(bool blend);
	};
}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/06/04 16:29:57 $
 * $Log: fps_meter.h,v $
 * Revision 1.13  2003/06/04 16:29:57  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.12  2003/01/06 12:42:01  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.11  2002/12/29 17:04:32  pstrand
 * hw/ moved to os/
 *
 * Revision 1.10  2002/04/08 20:07:08  pstrand
 * valarray -> vector<float>  (just for compat with egcs, with is the only place valarray is used...)
 *
 * Revision 1.9  2002/03/11 10:50:48  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.8  2001/12/18 14:14:16  macke
 * cancel last commit
 *
 * Revision 1.6  2001/12/05 21:58:38  peter
 * moved fps-meter code, debug_window is no more
 *
 * Revision 1.5  2001/08/06 12:16:11  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.4.4.1  2001/08/03 13:43:51  peter
 * pragma once obsolete...
 *
 * Revision 1.4  2001/07/11 10:54:50  peter
 * time.h uppdateringar...
 *
 * Revision 1.3  2001/07/06 01:47:10  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.2  2001/04/19 01:48:08  macke
 * ...
 *
 * Revision 1.1  2001/04/18 05:07:24  macke
 * Tokstädning
 *
 */

