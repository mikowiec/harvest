#include "os/compat.h"
#include "misc/treap.h"
#include <iostream>

using namespace reaper::misc;
using namespace std;

int main()
{
	try {
		Treap<int> tr;

		tr.push(3);
		tr.push(2);
		tr.push(5);
		tr.push(7);

		tr.print();

		tr.remove(2);

		cout << "\n";
		tr.print();	

		return 0;
	}
        catch(const reaper::error_base &e) {
                cout << e.what() << endl;
                return 1;
        }

}
	
