#include "coll_hash.h"
#include "exceptions.h"
using reaper::phys::phys_error;

#include <iostream>
using std::cout;
using std::endl;

coll_hash::coll_hash():
current_size(0),
elements(primes[0],buckList()),
num_elems(0)
{
}

coll_hash::~coll_hash()
{

}

//////////////////////////

const int coll_hash::primes[largest_prime] = {3,5,11,47,97,197,397,797,1597,3203,6421,12853,26021,52051,104999,203221};

//A very simple hash function
int coll_hash::hash_func(const bucket_key& k) const
{
        return (k.x*k.x + k.y*k.y + k.z*k.z) % primes[current_size];
}

void coll_hash::insert(const objId id,const bucket_key &k)
{
//	cout << "Coll_hash: inserting" << endl;

        bool insObj = false;
        bool insBuck = false;  //Keeps track of new insertions
        
        //First calculate the hash number
        int hashNum = hash_func(k);

        //Then see if we can find a bucket already present
        buckList& curList = elements[hashNum];
        buckList::iterator p = std::find_if(curList.begin(),curList.end(),buck_eq(k));
        if(p != curList.end()){
               //if so, then insert the ID there, and see if we succeeded 
                int oldSize = (**p).objects.size();
                (**p).objects.insert(id);
                //insObj = (**p).objects.size() != oldSize;
                
                //Keep track if the insertion was successful, and if we had to 
                //allocate a new bucket
        }
        else // a new bucket is needed.
        {
                buckPtr t = new bucket(k);
                t->objects.insert(id);
                num_elems++;
                curList.push_back(t);

        }
        //If the number of bucket becomes large, then rehash
        if( num_elems > primes[current_size] ) {
                ++current_size;

                if(current_size > largest_prime) 
                  throw phys_error("collHash: No larger prime available!");
                
                
                vector<buckList> new_elems(primes[current_size],buckList());
                
                for(int i = 0; i < elements.size(); ++i){
                        buckList::iterator p = elements[i].begin();
                        for( ; p != elements[i].end(); ++p ){
                                int hashNum = hash_func((**p).get_key());
                                new_elems[hashNum].push_back(*p);
                        }
                }

                elements = new_elems;
        }
}

void coll_hash::remove(const objId id,const bucket_key& k)
{
         //First calculate the hash number
        int hashNum = hash_func(k);

        //Then see if we can find a bucket
        buckList &curList = elements[hashNum];
        buckList::iterator p = std::find_if(curList.begin(),curList.end(),buck_eq(k));
        if(p != curList.end()){
               //if so, then insert the ID there, and see if we succeeded 
                int oldSize = (**p).objects.size();
                (**p).objects.erase(id);
                if( oldSize != (**p).objects.size() ) { //we actualy erased an element
                        if( (**p).objects.empty() ){
                                curList.erase(p);
                                --num_elems;
                        }
                        
                }
                else {
                        //No element found.
                }
        }
        else //No bucket found!
        {
                //Throw exception or let be;
        }
}

const set<objId>& coll_hash::get_ids(const bucket_key& k) const
{
        int hashNum = hash_func(k);

	//Then see if we can find a bucket
        const buckList &curList = elements[hashNum];
        buckList::const_iterator p = std::find_if(curList.begin(),curList.end(),buck_eq(k));
        if(p != curList.end()){
		
		return (**p).objects;
		
        }
        else //No bucket found!
        {
		throw phys_error("No such bucket");
	}

}


//temporary
vector<bucket_key> coll_hash::get_keys()
{
        vector<bucket_key> keys;
        for(int i = 0; i < elements.size(); ++i ){
                buckList tList = elements[i];
                for(buckList::const_iterator p = tList.begin(); p != tList.end(); ++p){
                        keys.push_back( (**p).get_key() );
                }
        }

        return keys;
}

//Temporary
void coll_hash::print()
{
        for(int i = 0; i < elements.size(); ++i ){
                cout << i << " : ";
                buckList tList = elements[i];
                for(buckList::const_iterator p = tList.begin(); p != tList.end(); ++p){
                        cout << "NK: ";
                        for(std::set<objId>::const_iterator o = (**p).objects.begin(); 
                        o !=(**p).objects.end(); o++){
                                cout << " " << *o;
                        }
                }
                cout << endl;
        }
}

grid::grid(float x, float y, float z, float d) :
mX(x),mY(y),mZ(z),distance(d)
{}

//Caculates all keys for all the grids that the bounding box p1-p2 spans
void grid::calc_key(const Point& p1, const Point& p2,vector<bucket_key>& keys) const
{
        if(p1.x > p2.x || p1.y > p2.y || p1.z > p2.z)  //check p1 < p2
                throw phys_error("Grid: No correct bounding box. Reversed coords?");
        
        vector<int> x,y,z;
        
        double c = p1.x - mX;

	x.reserve( (p2.x -p1.x)/distance  + 1) ;
	y.reserve( (p2.y -p1.y)/distance  + 1) ;
	z.reserve( (p2.z -p1.z)/distance  + 1) ;

        while(c <= p2.x - mX){
                int t = floor( c/distance);
                x.push_back( t );
                c = (t+1) * distance;
        }

        c = p1.y - mY;

        while(c <= p2.y - mY){
                int t = floor( c/distance);
                y.push_back( t );
                c = (t+1) * distance;
        }

        c = p1.z - mZ;

        while(c <= p2.z - mZ){
                int t = floor( c/distance);
                z.push_back( t );
                c = (t+1) * distance;
        }      

        keys.clear();
        keys.reserve(x.size() * y.size() * z.size() );

        for(int i = 0;i < x.size(); ++i)
                for(int j = 0; j < y.size(); ++j)
                        for(int k = 0; k < z.size(); ++k)
                                keys.push_back(bucket_key(x[i],y[j],z[k]));
	
                
}

void grid::get_points(const bucket_key& k,Point& min,Point& max) const
{
        min.x = k.x * distance + mX;
        min.y = k.y * distance + mY;
        min.z = k.z * distance + mZ;

        max.x = (k.x+1) * distance + mX;
        max.y = (k.y+1) * distance + mY;
        max.z = (k.z+1) * distance + mZ;
}
