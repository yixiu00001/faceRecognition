/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*
*include "lbp.h"
*
* Last modified: 2015-12-19 08:35
*
* Filename: lbp.cpp
*
* Description:compute the lbp feature 
*
=============================================================================*/
#include "lbp.h"
#include "lbpCompute.h"


void CLbp::PreProcImage(cv::Mat &srcmat, cv::Mat &dstmat)
{

	cv::Mat m1,m2,m3,m4;

	//image pre processing,must is gray
	cv::GaussianBlur(srcmat, m1, cv::Size(0,0), 1.1f);
	//imshow("gaussian", m1);
	//waitKey(0);
	CFilterAlgorithm::Gamma(m1,m2);
	//imshow("gamma", m2);
	//waitKey(0);
	CFilterAlgorithm::Dog(m2,m3);
	//imshow("dog", m3);
	//waitKey(0);
	CFilterAlgorithm::ContrastEq(m3,m4);
	//imshow("eq", m4);
	//waitKey(0);

	dstmat = m4;
	
}

bool CLbp::Project(cv::Mat &src, vector<cv::Mat> &dst)
{
	Mat &srcmat = src;
	Mat premat,lbpmat;
	CLbpCompute lbpCom(1, 2);

	//do preprocess
	PreProcImage(srcmat,premat );

	//get lbp feature
	lbpCom.LbpFeature(premat, lbpmat );
	//cout<<"lbp feature="<<lbpmat<<endl;

	//to 8*8 image
	vector<Mat> rcftmat;
	CRegionsAlgorithm::RectRegions(lbpmat,rcftmat);

	//calc hist
	for(int i = 0;i < (int)rcftmat.size();i++)
	{
		cv::Mat histmat;

		CNumberAlgorithm::Hist(rcftmat[i],histmat,59.0f);
		dst.push_back(histmat);
	}


	
}
