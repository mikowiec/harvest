#ifndef REAPER_MAIN_TYPES_IO_H
#define REAPER_MAIN_TYPES_IO_H

#include <iostream>
#include <typeinfo>
#include <sstream>
#include "main/exceptions.h"
#include "main/enums.h"
#include "main/types.h"

#include <stdio.h>

namespace reaper
{

define_reaper_exception(io_error);


// input/output for our fundamental types

template<class T>
std::istream& operator>>(std::istream&, Vec<T>&);
template<class T>
std::ostream& operator<<(std::ostream&, const Vec<T>&);

std::ostream& operator<<(std::ostream &os, const Matrix &m);
std::istream& operator>>(std::istream &is, Matrix &m);

inline std::istream& operator>>(std::istream &, Quaternion &);
inline std::ostream& operator<<(std::ostream &, const Quaternion &);

template<class T>
std::istream& operator>>(std::istream &, triangle_base<T> &);
template<class T>
std::ostream& operator<<(std::ostream &, const triangle_base<T> &);

template<class T> T read(const std::string &s);
template<class T> std::string write(const T &t);

std::ostream& operator<<(std::ostream &os, CompanyID);
std::istream& operator>>(std::istream &is, CompanyID &id);



/////////////////////////////////////////////////////////////////////////////
// implementation below
/////////////////////////////////////////////////////////////////////////////


namespace io_hlp
{

template<typename value_type>
std::istream& read(std::istream& is, value_type* p, int N)
{
	for (int i = 0; i < N; ++i) {
		is.get();
		is >> p[i];
	}
	char c;
	while (is >> c && c != ']')
		;
	return is;
}

template<typename value_type>
std::istream& read(std::istream& is, value_type* p, int N, value_type def)
{
	int i = 0;
	while (i < N && is.peek() != ']') {
		is.get();
		is >> p[i++];
	}
	while (i < N)
		p[i++] = def;
	char c;
	while (is >> c && c != ']')
		;
	return is;
}

template<int n, typename value_type>
std::ostream& write(std::ostream& os, const value_type* p)
{
	os << "[ ";
	for (int i = 0; i < n-1; ++i) {
		os << p[i] << ", ";
	}
	os << p[n-1] << "]";
	return os;
}

template<int n, typename value_type>
std::ostream& write(std::ostream& os, const value_type* p, value_type def)
{
	os << "[ ";
	int i = 0;
	while (i < n-2) {
		os << p[i++] << ", ";
	}
	os << p[i];
	if (p[n-1] == def)
		os << ']';
	else
		os << ", " << p[i+1] << ']';
	return os;
}

}

template<class T>
std::istream& operator>>(std::istream& is, Vec<T>& vec)
{
	return io_hlp::read(is, vec.get(), T::N);
}

template<class T>
std::ostream& operator<<(std::ostream& os, const Vec<T>& vec) 
{
	return io_hlp::write<T::N>(os, vec.get());
}


inline std::istream& operator>>(std::istream& is, Quaternion& q)
{
	return io_hlp::read(is, q.get(), 4);
}

inline std::ostream& operator<<(std::ostream& os, const Quaternion& q)
{
	return io_hlp::write<4>(os, q.get());
}

template<class T>
std::istream& operator>>(std::istream& is, triangle_base<T>& t)
{
	// Assumes no padding...
	return io_hlp::read(is, &t.v1, 3);
}

template<class T>
inline std::ostream& operator<<(std::ostream& os, const triangle_base<T>& t)
{
	// Assumes no padding...
	return io_hlp::write<3>(os, &t.v1);
}        

template<class T>
inline T read(const std::string &s)
{
	std::istringstream ss(s);
	T t;
	ss >> t;
	return t;
}

template<class T>
inline std::string write(const T &t)
{
	std::ostringstream ss;
	ss << t;
	return ss.str();
}


}

#endif

/*
 * $Author: pstrand $
 * $Date: 2002/06/19 17:20:06 $
 * $Log: types_io.h,v $
 * Revision 1.20  2002/06/19 17:20:06  pstrand
 * ''
 *
 * Revision 1.19  2002/06/18 12:19:25  pstrand
 * upprensning
 *
 * Revision 1.18  2002/05/04 11:10:00  pstrand
 * menu only shows available ships...
 *
 * Revision 1.17  2002/02/27 18:08:41  pstrand
 * release-0.96
 *
 * Revision 1.16  2002/01/28 00:49:44  macke
 * matrix <-> matrix4 conversion fix
 *
 * Revision 1.15  2002/01/27 04:00:36  peter
 * *** empty log message ***
 *
 * Revision 1.14  2001/11/26 10:46:04  peter
 * kompilatorgnäll... ;)
 *
 * Revision 1.13  2001/08/27 12:55:26  peter
 * objektmeck...
 *
 * Revision 1.12  2001/08/06 12:16:29  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.11.2.2  2001/08/05 14:01:27  peter
 * objektmeck...
 *
 * Revision 1.11.2.1  2001/08/03 13:44:05  peter
 * pragma once obsolete...
 *
 * Revision 1.11  2001/07/30 23:43:23  macke
 * Häpp, då var det kört.
 *
 * Revision 1.10  2001/07/24 23:52:48  macke
 * Jo, explicit ska det vara (fel på annat ställe)..  rättade till lite array-fel också..
 *
 * Revision 1.9  2001/07/16 15:20:49  peter
 * *** empty log message ***
 *
 * Revision 1.8  2001/07/16 14:57:11  peter
 * mindre optimering..
 *
 * Revision 1.7  2001/07/06 01:47:25  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.6  2001/05/07 05:35:23  macke
 * level editor ... prototyp funkar,..,
 *
 * Revision 1.5  2001/05/06 01:32:41  macke
 * bök på leveleditorn..
 *
 */



