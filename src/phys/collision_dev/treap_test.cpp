#include "os/compat.h"
#include "treap.cpp"
#include <iostream>

using std::endl;
using std::cout;
using std::cin;


class testObj
{
public:
	int id1,id2;
	float time;
	testObj(int i1,int i2,float t) : id1(i1),id2(i2),time(t) {};
	bool operator==(const testObj& rhs) const {return rhs.id1==id1 && rhs.id2 == id2;};
	bool operator<(const testObj&rhs) const 
	{
		if(id1 < rhs.id1)
			return true;
		else if(id1 == rhs.id1)
			return id2 < rhs.id2;
		else
			return false;
	} ;

	bool has_id(int i) {return id1==i || id2 == i;}

	friend std::ostream& operator<<(std::ostream& s, testObj& o);
	friend bool compare(const testObj& o1,const testObj& o2) ;
};

std::ostream& operator<<(std::ostream& s, testObj& o) 
{
	return s << "Id1: " << o.id1 << " Id2: " << o.id2 << " time: " << o.time << " ";
}

bool compare(const testObj& o1,const testObj& o2) 
{
	return o1.time < o2.time;
}

class compTime
{
public:
	bool operator()(const testObj& o1,const testObj& o2) const
	{
		return compare(o1,o2);
	};
};

class mindre
{
public:
	bool operator()(const testObj& o1,const testObj& o2) const
	{
		return o1 < o2;
	};
};

class testf
{
	int id;

public:
	explicit testf(int i): id(i) {};
	bool operator()(testObj j) const {return j.has_id(id);}
};

class Equal
{
public:
	bool operator()(const testObj& o1,const testObj& o2) const
	{
		return o1 == o2;
	};
};

int main(int argc, char **argv)	
{
	Treap< testObj,compTime,testf> treap;

        for(;;){
                int id1,id2;
		float time;
                char c;
		cout << "Operation Id1 Id2 time" << endl;
                cin >> c >> id1 >> id2 >> time;
                switch(c){
                case 'i': treap.insert(testObj(id1,id2,time));
                        break;
                case 'd': treap.remove(testObj(id1,id2,0.0));
                        break;
		case 'u': treap.update(testObj(id1,id2,time));
                        break;
		case 'p': treap.pop();
			break;
		case 't': treap.delete_if(testf(id1));
			break;
                default:
                        break;
                }

		if(!treap.empty())
		cout << "Top" << treap.top() << endl;
                
		cout << "Whole: " << endl;
		treap.print();
        }

        return 0;
}

