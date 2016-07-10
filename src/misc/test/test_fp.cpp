
/*
 * $Author: peter $
 * $Date: 2001/08/20 17:06:19 $
 * $Log: test_fp.cpp,v $
 * Revision 1.2  2001/08/20 17:06:19  peter
 * *** empty log message ***
 *
 * Revision 1.1  2001/03/21 10:39:02  peter
 * *** empty log message ***
 *
 * Revision 1.2  2001/01/01 15:42:28  peter
 * no message
 *
 * Revision 1.1  2000/10/30 00:35:35  peter
 * res.. exempel...
 *
 */

#include <string>
#include <list>
#include <algorithm>
#include <functional>
#include <iostream>

#include "misc/stlhelper.h"
#include "misc/sequence.h"
#include "misc/maybe.h"

using namespace reaper::misc;
using namespace std;

class f1 : public unary_function<int, int> {
public:
	int operator()(int i) {
		return i * 2;
	}
};

class f2 : public unary_function<int, int> {
	int n;
public:
	f2(int i) : n(i) { }
	int operator()(int i) {
		return n * i;
	}
	void print(int i) {
		cout << i << endl;
	}
};

class f3 : public unary_object<int, int> {
	int n;
public:
	f3() : n(1) { }
	int operator()(int i) {
		return i + n++;
	}
};

int main() {
	string t = "Hello cruel world";
	Maybe<string> m(t);
	Maybe<string> n;
	cout << string(m) << endl;
	if (m && !n) 
		cout << "just" << endl;
	n = string("näää");
	if (n)
		cout << "jo: " << string(n) << endl;
	n = false;
	cout << string(n) << endl;

	list<int> l(10);
	fill(seq(l), 2);
	transform(seq(l), l.begin(), f3());
	transform(seq(l), ostream_iterator<int>(cout, " "), compose(f1(), f2(5)));
	f2* tt = new f2(2);
	for_each(seq(l), apply_to(tt, &f2::print));
	cout << endl;
	return 0;
}



