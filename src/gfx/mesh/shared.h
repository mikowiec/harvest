#ifndef REAPER_MESH_SHARED_H
#define REAPER_MESH_SHARED_H

#include "os/compat.h"
#include "gfx/vertex_array.h"

#include <algorithm>
#include <vector>
#include <set>
#include <list>
#include <time.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#define MESH_HASH_SIZE 1000

namespace reaper 
{
namespace gfx
{
namespace mesh
{


class SetTooSmall{};
class MalFormedInput{};
class ContractionOutOfDate{};


class HeightField {

private:
	
	std::vector<float> data;
	int width, height;

public:

	HeightField(int w, int h):
		data(w*h, 0.0),
		width(w),
		height(h)
	{
	}

	int get_width(){return width;};
	int get_height(){return height;};

	void set(int w, int h, float d)
	{
		data.at(h*width + w) = d;
	}

	float get(int w, int h)
	{
		return data.at(h*width + w);
	}

	void add(int w, int h, float param)
	{
		data.at(h*width + w) += param;
	}


	void sub(int w, int h, float param)
	{
		data.at(h*width + w) -= param;
	}

	HeightField& operator=(const HeightField& rhs)
	{
		width = rhs.width;
		height = rhs.height;
		data = rhs.data;

		return *this;
	}

};


class MeshException
{
public:
	char* error;

	MeshException(char* e):error(e){}
};

enum ContStatus { OK, SUBDUE, INVALID };

class Timer {

private:

	float times[100];
	int start_time, cnt;
	float ellapsed;
	int maxcnt;

public:

	Timer():ellapsed(0.0), cnt(0), maxcnt(10){};

	void reset(){ellapsed = 0.0; cnt = 0;};
	void start(){start_time = clock();};
	void stop(){
		ellapsed =  ((float) ( clock() - start_time )) / (float) CLOCKS_PER_SEC;
		times[cnt] =  ellapsed;
		cnt = (cnt>maxcnt?0:cnt+1); 
	};

	float avg(){
		double sum = 0.0;
		for(int i = 0; i < maxcnt; i++)sum += times[i];
		return sum / (float) maxcnt;
	};

	float time(){return ellapsed;};
};

/**********************************************
				CLASS: Pair
**********************************************/

class Pair {
public:
	int a, b;

	Pair(int x, int y)
	{
		if(x>y)std::swap(x,y);

		a = x;
		b = y;
	}

	bool operator<(const Pair& p0) const
	{
		return ( (a < p0.a) || ( (a == p0.a) && (b < p0.b) ) );
	}


	bool operator==(const Pair p0) const
	{
		return (a == p0.a) && (b == p0.b);
	}

};


class HashTab {

private:
	int tab[MESH_HASH_SIZE][2];

public:
	void clear(){for(int i = 0; i < MESH_HASH_SIZE; i++)tab[i][0] = tab[i][1] = -1;};

	void insert(int ei, int val){
		tab[ei % MESH_HASH_SIZE][0] = ei;
		tab[ei % MESH_HASH_SIZE][1] = val;
	}

	int lookup(int ei){
		if(tab[ei % MESH_HASH_SIZE][0] == ei)return tab[ei % MESH_HASH_SIZE][1];
		else return -1;
	}
};

typedef std::vector<int> vint;
typedef std::list<int> lint;

template<class T>
T pop(std::set<T>& s)
{
	std::set<T>::reverse_iterator ci = s.rbegin();
	T t = *ci;
	s.erase(t);
	return t;
}
	
template<class T>
T pop(std::vector<T>& s)
{
	T t = s.back();
	s.pop_back();
	return t;
}


}
}
}


#endif

