#include "sift.h"
#include "imgproc.h"

void Grid(cv::Mat &srcmat,vector<cv::Point2f> &landmarks,int rows = 1,int columns = 1)
{
	const float row_step = 1.0f * srcmat.rows / rows;
	const float column_step = 1.0f * srcmat.cols / columns;
	for (float y=row_step/2; y<srcmat.rows; y+=row_step)
		for (float x=column_step/2; x<srcmat.cols; x+=column_step)
			landmarks.push_back(cv::Point2f(x,y));
}
bool CSift::Project( cv::Mat &src,vector<cv::Mat> &dst )
{
	cv::Mat srcmat = src;
	cv::SIFT sift;
	vector<cv::Point2f> landmarks;
	vector<cv::Mat> matlist;

	Grid(srcmat,landmarks,10,10);

	std::vector<cv::KeyPoint> keyPoints;
	for(int i = 0;i < (int)landmarks.size();i++)
	{
		keyPoints.push_back(cv::KeyPoint(landmarks[i].x, landmarks[i].y, 12));
	}

	cv::Mat m;
	sift(srcmat, cv::Mat(), keyPoints, m, true);
	m.setTo(0, m<0); // SIFT returns large negative values when it goes off the edge of the image.
	
	CRegionsAlgorithm::ByRow(m,matlist);
	dst.insert(dst.end(),matlist.begin(),matlist.end());
	/*
	cout<<"sift====="<<endl;
	for(int i=0;i<matlist.size();i++)
	{
		cout<<matlist[i]<<endl;
	}
	cout<<"================"<<endl;
	*/
	return true;
}
