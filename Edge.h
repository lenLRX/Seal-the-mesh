#ifndef __EDGE_H__
#define __EDGE_H__

#include "MyUtility.h"

class Edge
{
public:
	Edge(){
		next = 0;
		prev = 0;
		used = false;
		valid = true;
	}

	PointInTriangle v1;
	PointInTriangle v2;

	int idx1;
	int idx2;

	int next;
	int prev;
	bool used;
	bool valid;
};


#endif