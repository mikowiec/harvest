#include "coll_hash.h"
#include "os/compat.h"
#include <iostream>

using std::cout;
using std::endl;
using std::cin;

//Testing the hash table, seems to work
/*
int main(int argc, char **argv)	
{
        cout << "Starting 1" << endl;
        coll_hash tabell;
        

        for(;;){
                int id,x,y,z;
                char c;
                cout << "Id, x y z" << endl;
                cin >> c >> id >> x >> y >> z ;
                switch(c){
                case 'i': tabell.insert(id,bucket_key(x,y,z));
                        break;
                case 'd': tabell.remove(id,bucket_key(x,y,z));
                        break;
                default:
                        break;
                }
                tabell.print();
        }

        return 0;
}

*/

void print_key(const bucket_key& key)
{
        cout << "Key: " << key.x << " " << key.y << " " << key.z << " " << endl;
}
/*
//Testing the grid function
int main(int argc, char **argv)	
{
        cout << "Starting.. " << endl;
        coll_hash tabell;
        grid the_grid(-10, 0, -10 ,2);

        for(;;){
                float x,y,z, x2, y2, z2;
                char c;
                cout << "x y z1 x y z2" << endl;
                cin >> x >> y >> z ;
                cin >> x2 >> y2 >> z2 ;
                
                vector<bucket_key> keys;
                the_grid.calc_key(Point(x,y,z),Point(x2,y2,z2),keys);

                for(int i = 0; i< keys.size(); ++i)
                        print_key(keys[i]);
                
        }

        return 0;
}
*/
