
#include "os/compat.h"

#include <iostream>
#include <iomanip>

#include "misc/parse.h"
#include "main/types_io.h"


namespace reaper {

std::istream& operator>>(std::istream &is, CompanyID &id)
{
	std::string s;
	is >> s;
	id = misc::str2company(s);
	return is;
}


std::ostream& operator<<(std::ostream &os, CompanyID id)
{
	switch (id) {
	case Nature:     os << "Nature"; break;
	case Projectile: os << "Projectile"; break;
	case Slick:      os << "Slick"; break;
	case Horny:      os << "Horny"; break;
	case Shell:      os << "Shell"; break;
	case Punkrocker: os << "Punkrocker"; break;
	case Village:    os << "Village"; break;
	default: break;
	}
	return os;
}

std::ostream& operator<<(std::ostream &os, const Matrix &m)
{
	os << std::setprecision(12) << '[';
	for (int c = 0; c < 3; c++) {
		for (int r = 0; r < 3; r++)
			os << m[r][c] << ", ";
		os << "0,  ";
	}
	os << m.pos()[0] << ", " << m.pos()[1] << ", " << m.pos()[2] << ", 1]";
	return os;
}

std::istream& operator>>(std::istream &is, Matrix &m)
{
	char junk;
	float ign;
	
	for (int c = 0; c < 3; c++) {
		for (int r = 0; r < 3; r++)
			is >> junk >> m[r][c];
		is >> junk >> ign;
	}
	for (int r = 0; r < 3; r++)
		is >> junk >> m.pos()[r];
	misc::until(is, ']');
	is.get();
	return is;
}

}

