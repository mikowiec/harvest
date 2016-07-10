
/*
 * $Author: peter $
 * $Date: 2000/10/30 00:35:35 $
 * $Log: string.cpp,v $
 * Revision 1.1  2000/10/30 00:35:35  peter
 * res.. exempel...
 *
 */

#include <string>
#include <list>
#include <algorithm>

#include "stringhelper.h"
#include "sequence.h"

int main() {
	string t = "hello world test string";
	strlst l2;
	split(t, l2, 2);
	copy(cseq(l2), ostream_iterator<string>(cout, "|"));
	cout << ltos(stol("47")) << endl;
	return 0;
}
