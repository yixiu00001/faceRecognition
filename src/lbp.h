#ifndef lbp_h_ 
#define lbp_h_
#include "Unified.h"
#include "imgproc.h"
class  CLbp
{
	private :
		void PreProcImage(cv::Mat &srcmat,cv::Mat &dstmat); 
	public:
		CLbp(){}
		~CLbp(){}
		bool Project(cv::Mat &src,vector<cv::Mat> &dst);
};

#endif
