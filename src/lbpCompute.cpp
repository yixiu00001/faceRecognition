/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*
*include "lbpCompute.h"
*
* Last modified: 2015-12-19 08:46
*
* Filename: lbpCompute.cpp
*
* Description: 
*
=============================================================================*/
#include "lbpCompute.h"

	/* Returns the number of 0->1 or 1->0 transitions in i */
	static int numTransitions(int i)
	{
		int transitions = 0;
		int curParity = i%2;
		for (int j=1; j<=8; j++) {
			int parity = (i>>(j%8)) % 2;
			if (parity != curParity) transitions++;
			curParity = parity;
		}
		return transitions;
	}
	static int rotationInvariantEquivalent(int i)
	{
		int min = std::numeric_limits<int>::max();
		for (int j=0; j<8; j++) {
			bool parity = (i % 2) != 0;
			i = i >> 1;
			if (parity) i+=128;
			min = std::min(min, i);
		}
		return min;
	}

	CLbpCompute::CLbpCompute(int radius  ,int maxTransitions , bool rotationInvariant )
	{
		this->radius = radius;
		this->maxTransitions = maxTransitions;
		this->rotationInvariant = rotationInvariant;
		init();
	}
	CLbpCompute::~CLbpCompute( )
	{}

	void CLbpCompute::init()
	{
		bool set[256];
		uchar uid = 0;
		for (int i=0; i<256; i++) {
			if (numTransitions(i) <= maxTransitions) {
				int id;
				if (rotationInvariant) {
					int rie = rotationInvariantEquivalent(i);
					if (i == rie) id = uid++;
					else          id = lut[rie];
				} else            id = uid++;
				lut[i] = id;
				set[i] = true;
			} else {
				set[i] = false;
			}
		}

		null = uid;
		for (int i=0; i<256; i++)
			if (!set[i])
				lut[i] = null; // Set to null id
	}

	void CLbpCompute::Hist( cv::Mat &srcmat,cv::Mat &dstmat,float _max /*= 256*/,float _min /*= 0*/,int _dims /*= -1*/ )
	{
		const int dims = _dims == -1 ? (int)(_max - _min) : _dims;

		vector<cv::Mat> mv;
		cv::split(srcmat, mv);
		cv::Mat m(mv.size(), dims, CV_32FC1);

		for (size_t i=0; i<mv.size(); i++) {
			int channels[] = {0};
			int histSize[] = {dims};
			float range[] = {_min, _max};
			const float* ranges[] = {range};
			cv::Mat hist, chan = mv[i];
			// calcHist requires F or U, might as well convert just in case
			if (mv[i].depth() != CV_8U && mv[i].depth() != CV_32F)
				mv[i].convertTo(chan, CV_32F);
			cv::calcHist(&chan, 1, channels, cv::Mat(), hist, 1, histSize, ranges);
			memcpy(m.ptr(i), hist.ptr(), dims * sizeof(float));
		}

		dstmat = m;
	}


	void CLbpCompute::LbpFeature(cv::Mat &srcmat,cv::Mat &dstmat) 
	{
		cv::Mat m; 
		srcmat.convertTo(m, CV_32F);

		cv::Mat n(m.rows, m.cols, CV_8UC1);
		n = null; // Initialize to NULL LBP pattern

		//cout<<"srcmat="<<srcmat<<endl;
		const float *p = (const float*)m.ptr();
		//cout<<"=====LbpFeature====="<<radius<<" rows="<<m.rows<<" cols="<<m.cols<<endl;
		for (int r=radius; r<m.rows-radius; r++) {
			for (int c=radius; c<m.cols-radius; c++) {
				
				const float cval  =     (p[(r+0*radius)*m.cols+c+0*radius]);
				
				
					n.at<uchar>(r, c) = lut[(p[(r-1*radius)*m.cols+c-1*radius] >= cval ? 128 : 0) |
					(p[(r-1*radius)*m.cols+c+0*radius] >= cval ? 64  : 0) |
					(p[(r-1*radius)*m.cols+c+1*radius] >= cval ? 32  : 0) |
					(p[(r+0*radius)*m.cols+c+1*radius] >= cval ? 16  : 0) |
					(p[(r+1*radius)*m.cols+c+1*radius] >= cval ? 8   : 0) |
					(p[(r+1*radius)*m.cols+c+0*radius] >= cval ? 4   : 0) |
					(p[(r+1*radius)*m.cols+c-1*radius] >= cval ? 2   : 0) |
					(p[(r+0*radius)*m.cols+c-1*radius] >= cval ? 1   : 0)];
				//cout<<cval<<"["<<r<<","<<c<<"]="<<n.at<uchar>(r,c)<<endl;

			}
		}

		dstmat = n;
	}

