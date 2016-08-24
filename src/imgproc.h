/*
image proc Algorithm
*/
#ifndef imgproc_h__
#define imgproc_h__

#include "Unified.h"

class CFilterAlgorithm
{
public:

	static void Dog(cv::Mat &srcmat,cv::Mat &dstmat,float a = 1.0f,float b = 2.0f);

	static void Gamma( cv::Mat &srcmat,cv::Mat &dstmat );

	static void ContrastEq( cv::Mat &srcmat,cv::Mat &dstmat,float a = 0.1f,float t = 10.0f );
};

class CNumberAlgorithm
{
public:

	enum NormType { Inf = cv::NORM_INF,
		L1 = cv::NORM_L1,
		L2 = cv::NORM_L2 };

public:
	
	static void Normalize(cv::Mat &srcmat,cv::Mat &dstmat,NormType type);

	static void Hist(cv::Mat &srcmat,cv::Mat &dstmat,float _max = 256,float _min = 0,int _dims = -1);

	static void Quantize(cv::Mat &srcmat,cv::Mat &dstmat,float a = 5093.03f,float b = 128.00f);
};

class CRegionsAlgorithm
{
public:

	static void RectRegions(cv::Mat &srcmat,vector<cv::Mat> &dst,int width = 8,
		int height = 8,int _widthStep = 6,int _heightStep = 6);

	static void ByRow(cv::Mat &srcmat,vector<cv::Mat> &dst);

	static void Cat(vector<cv::Mat> &src,vector<cv::Mat> &dst,int partitions = 1);
};

#endif // imgproc_h__