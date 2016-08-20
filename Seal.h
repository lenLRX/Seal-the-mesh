#ifndef __SEAL_H__
#define __SEAL_H__
#include "MyUtility.h"
#include "Area.h"

static std::vector<int> GenerateFaces(std::vector<VxVector> pts,
	std::vector<int> idxs,VxVector Norm,CKContext* context){
	Chain chain;

	for(size_t i = 1;i < pts.size();i++){
		Edge E;
		PointInTriangle p1,p2;
		p1.point = pts[i - 1];
		p1.No = idxs[i - 1];
		p2.point = pts[i];
		p2.No = idxs[i];
		E.v1 = p1;
		E.v2 = p2;

		chain.Edges.push_back(E);
	}

	Area area;
	area.Boundary = chain;
	area.ParentNorm = Norm;
	

	return area.Triangulation(context);
}


#endif//__SEAL_H__