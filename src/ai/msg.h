#ifndef REAPER_AI_MSG_H
#define REAPER_AI_MSG_H

#include <queue>
#include <vector>

namespace reaper{
namespace ai{

	enum MsgName
	{
		MSG_EMPTY = 0,
		MSG_DAMAGE,
		MSG_DEATH,
		MSG_HELP,
		MSG_SELF_DESTRUCT,
		MSG_DEST_REACHED
	};

	struct Message
	{
		MsgName name;        /// name of message
		unsigned sender;     /// id of sender
		float amount;        /// for example amount of damage
		float delivery_time; /// delivery time, if delayed message
		
		Message(void) {};
		Message(MsgName n, unsigned s, float a=0.0, float d=0.0) 
		{
			name = n;
			sender = s;
			amount = a; 
			delivery_time = d;
		};

		bool operator<(const Message& m) const {return delivery_time > m.delivery_time;}
	};

	typedef std::priority_queue<Message> MsgQueue;

}
}
#endif

/*
 * $Author: peter $
 * $Date: 2001/08/06 12:16:02 $
 * $Log: msg.h,v $
 * Revision 1.14  2001/08/06 12:16:02  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.13.4.1  2001/08/03 13:43:45  peter
 * pragma once obsolete...
 *
 * Revision 1.13  2001/07/06 01:47:05  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.12  2001/05/13 17:19:05  niklas
 * fix
 *
 * Revision 1.11  2001/05/06 00:15:30  niklas
 * Ändrade meddelandestrukturen + städ
 *
 * Revision 1.10  2001/04/08 23:34:40  macke
 * Ljusinläsning och allmänt städ..
 *
 *
 */

