
/*
 * $Author: peter $
 * $Date: 2000/10/30 00:35:35 $
 * $Log: sequence.cpp,v $
 * Revision 1.1  2000/10/30 00:35:35  peter
 * res.. exempel...
 *
 */

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#include "sequence.h"

using namespace reaper::misc;

int main() {
	vector<string> l;
	l.push_back("hello");
	l.push_back("world");
	Seq<vector<string>::iterator> sq = seq(l);
	cout << sq << endl;
	cout << sq.size() << " " << sq->size() << endl;
	try {
		cout << *(++(++sq)) << endl;
	} catch (...) {
		cout << "Caught exception!" << endl;
	}
	return 0;
}


