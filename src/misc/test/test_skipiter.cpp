#include "misc/sequence.h"
#include "misc/skipiter.h"
#include <deque>
#include <iterator>
#include <iostream>

using namespace std;
using namespace reaper::misc;

struct IsEven
{
	bool operator()(int i) {
		return (i % 2) == 0;
	}
};

int main()
{
	ostream_iterator<int> os(cout, " ");

	deque<int> v;
	for(int i = 0; i < 10; i++) 
		v.push_back(i);	

	cout << "seq:\n";
	copy(seq(v), os); cout << "\n";
	cout << "skipseq 1:\n";
	copy(skip_seq_eq(v.begin(), v.end(), 5), os); cout << "\n";
	copy(skip_seq_eq(seq(v), 5), os); cout << "\n";
	cout << "skipseq 2:\n";
	copy(skip_seq(v.begin(), v.end(), IsEven()), os); cout << "\n";
	copy(skip_seq(seq(v), IsEven()), os); cout << "\n";
	
	return 0;
}
