/*
sift Algorithm
*/
#ifndef sift_h__
#define sift_h__
#include "Unified.h"

//#include "Algorithm.h"

class CSift
{
public:
	CSift()
	{}
	~CSift()
	{}

	//Extraction feature
	bool Project(cv::Mat &src,vector<cv::Mat> &dst);
};

#endif // sift_h__
