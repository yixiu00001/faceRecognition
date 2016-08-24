#ifndef compare_h_
#define compare_h_

#include "Unified.h"

class CFaceCompare
{
	public:
		bool Compare( cv::Mat &targetmat,cv::Mat &querymat,float &dist );
		bool Train(vector<vector<cv::Mat> >&matlist, const vector<int> &classlist);
		bool Load(const char* filePath);
};

#endif
