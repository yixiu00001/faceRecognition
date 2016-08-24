/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*
*include "FacePreProcess.h"
*
* Last modified: 2015-11-18 9:44
*
* Filename: FacePreProcess.cpp
*
* Description: 
*
=============================================================================*/
#include "Unified.h" 
#include "FacePreProcess.h"
#include "iostream"

void CFacePreProc::FaceMask( cv::Mat &srcmat,cv::Mat &dstmat )
{
	Mat mask(srcmat.size(), CV_8UC1);
	mask.setTo(1);

	float SCALE = 1.1f;
	cv::ellipse(mask, cv::RotatedRect(cv::Point2f((float)(srcmat.cols/2),(float)(srcmat.rows/2)), cv::Size2f(SCALE*srcmat.cols, SCALE*srcmat.rows), 0), 0, -1);
	dstmat = srcmat.clone();
	dstmat.setTo(0, mask);

}

cv::Point2f getThirdAffinePoint(const cv::Point2f &a, const cv::Point2f &b)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	return cv::Point2f(a.x - dy, a.y + dx);
}

void CFacePreProc::OrientationCorrect( cv::Mat &srcmat,cv::Point2f ptview[3],cv::Mat &dstmat,int Affine_WIDTH,int Affine_HEIGHT,float X1,float Y1 )
{
	cv::Point2f dstPoints[3];
	dstPoints[0] = cv::Point2f(X1*Affine_WIDTH, Y1*Affine_HEIGHT);
	dstPoints[1] = cv::Point2f((1 - X1)*Affine_WIDTH, (Y1)*Affine_HEIGHT);
	dstPoints[2] = getThirdAffinePoint(dstPoints[0], dstPoints[1]);

	ptview[2] = getThirdAffinePoint(ptview[0], ptview[1]);
	//cout<<"source="<<dstPoints[0].x<<" "<<dstPoints[0].y<<" "<<dstPoints[1].x<<" "<<dstPoints[1].y<<" "<<dstPoints[2].x<<" "<<dstPoints[2].y<<endl;
	//cout<<"dst="<<ptview[0].x<<" "<<ptview[0].y<<" "<<ptview[1].x<<" "<<ptview[1].y<<" "<<ptview[2].x<<" "<<ptview[2].y<<endl;

	cv::Mat affineTransform = cv::getAffineTransform(ptview, dstPoints);
	cv::warpAffine(srcmat, dstmat, affineTransform, cv::Size(Affine_WIDTH, Affine_HEIGHT), cv::INTER_LINEAR);
}

void CFacePreProc::ProProcFace( cv::Mat &srcmat,cv::Point2f ptfeature[3],cv::Mat &dstmat,bool mask,int Affine_WIDTH,int Affine_HEIGHT,float X1,float Y1 )
{
	cv::Mat graymat;
	cv::Mat correctmat;
	cv::Mat maskmat;

	cv::cvtColor(srcmat,graymat,cv::COLOR_BGR2GRAY);
	//cout<<"source w*h="<<graymat.row<<" " <<graymat.cols<<endl;
	OrientationCorrect(graymat,ptfeature,correctmat,Affine_WIDTH,Affine_HEIGHT,X1,Y1);
	//cout<<"dst w*h="<<graymat.row<<" " <<graymat.cols<<endl;

	if(mask)
	{
		FaceMask(correctmat,maskmat);
	}
	else
	{
		maskmat = correctmat;
	}

	dstmat = maskmat;
	//imwrite("dst.bmp",dstmat);
}
