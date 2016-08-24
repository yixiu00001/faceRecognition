/*
image
face recongize 
pre processing Algorithm
*/
#ifndef facepreproc_h__
#define facepreproc_h__

#include "Unified.h"

class CFacePreProc
{
public:

	//use mask image in src img
	void FaceMask(cv::Mat &srcmat,cv::Mat &dstmat);

	//correct Orientation
	void OrientationCorrect(cv::Mat &srcmat,cv::Point2f ptview[3],cv::Mat &dstmat,int Affine_WIDTH = 88,int Affine_HEIGHT = 88,float X1 = 0.25f,float Y1 = 0.35);

	void ProProcFace(cv::Mat &srcmat,cv::Point2f ptfeature[3],cv::Mat &dstmatf,bool mask,int Affine_WIDTH = 88,int Affine_HEIGHT = 88,float X1 = 0.25f,float Y1 = 0.35f);
};

#endif // facepreproc_h__
