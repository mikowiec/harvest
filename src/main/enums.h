#ifndef REAPER_MAIN_ENUMS_H
#define REAPER_MAIN_ENUMS_H

namespace reaper {

// if companyid is changed, you must update:
// str2company()             in misc/parse.cpp 
// operator<<(os, companyid) in main/types_io.cpp

/// Company identifier
enum CompanyID
{
	Nature = 0,
	Village,
	Projectile,
        Slick,
        Horny,
        Shell,
        Punkrocker,
	Number_Companies
};

enum MissileType
{
	Light_Missile = 0,
	Target_Missile,
	Heavy_Missile,
	Number_MissileTypes
};

}

#endif
/*
 * $Author: macke $
 * $Date: 2002/01/28 00:49:24 $
 * $Log: enums.h,v $
 * Revision 1.7  2002/01/28 00:49:24  macke
 * more company ids
 *
 * Revision 1.6  2001/08/06 12:16:27  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.5.2.1  2001/08/03 13:44:02  peter
 * pragma once obsolete...
 *
 * Revision 1.5  2001/07/30 23:43:23  macke
 * Häpp, då var det kört.
 *
 * Revision 1.4  2001/07/06 01:47:24  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.3  2001/01/14 05:45:51  macke
 * Megaförändringar
 *
 * Revision 1.2  2000/11/21 16:00:09  macke
 * Lade till LevelID
 *
 * Revision 1.1  2000/11/16 18:15:49  macke
 * Alla enums bör nog ligga i separat fil
 *
 *
 */
