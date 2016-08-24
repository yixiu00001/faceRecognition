#include "Unified.h"
class CLbpCompute
{
	public:
		int  radius ;
		int  maxTransitions;
		bool rotationInvariant;

		CLbpCompute(int radius = 1,int maxTransitions = 8,bool rotationInvariant = false);
		~CLbpCompute();
		void init();
		void Hist(cv::Mat &srcmat,cv::Mat &dstmat,float _max = 256,float _min = 0,int _dims = -1);

		void Quantize(cv::Mat &srcmat,cv::Mat &dstmat,float a = 5093.03f,float b = 128.00f);
		void LbpFeature(cv::Mat &srcmat,cv::Mat &dstmat) ;


	private:

		unsigned char lut[256];
		unsigned char null;
};
