#ifndef base_h_
#define base_h_

#include "imgproc.h"

#include "Unified.h"

class CNormalize
{
	private:
		CNumberAlgorithm::NormType normtype;
	
	public:
		CNormalize(CNumberAlgorithm::NormType normtype)
		{
			this->normtype = normtype;
		}
		~CNormalize()
		{}
		
		bool Normalize(vector<cv::Mat> &src , vector<cv::Mat> &dst)
		{
			for(int i=0;i<(int)src.size();i++)
			{
				cv::Mat dstmat;
				CNumberAlgorithm::Normalize(src[i], dstmat, normtype);
				dst.push_back(dstmat);
			}
			return true;
		}

};

class CQuantize
{
protected:

	float a;
	float b;

public:
	CQuantize(float a = 5093.03f,float b = 128.00f)
	{
		this->a = a;
		this->b = b;
	}
	~CQuantize()
	{}

	virtual bool Project(vector<cv::Mat> &src,vector<cv::Mat> &dst)
	{
		for(int i = 0;i < (int)src.size();i++)
		{
			cv::Mat dstmat;

			CNumberAlgorithm::Quantize(src[i],dstmat,a,b);
			dst.push_back(dstmat);
		}
		
		return true;
	}
};

class CDistanceTest
{
	public :

	double dotCompute(const Mat &v1, const Mat &v2,unsigned int size)
	{
		double ret = 0, ret1=0,ret2=0;
		int rows,cols;
		rows = v1.rows;
		cols = v1.cols;
		for( int i = 0; i<rows; i++ )
		{
			const float* inData1=v1.ptr<float>(i);
			const float* inData2=v2.ptr<float>(i);
			for(int j=0; j<cols;j++)
			{
				//ret +=(inData1[j]*inData2[j]);
				ret1 += inData1[j];
				ret2 += inData2[j];
			}
		}
		double mean1 = ret1/(rows*cols);
		double mean2 = ret1/(rows*cols);
		for( int i = 0; i<rows; i++ )
		{
			const float* inData1=v1.ptr<float>(i);
			const float* inData2=v2.ptr<float>(i);
			for(int j=0; j<cols;j++)
			{
				ret +=((inData1[j]-mean1)*(inData2[j]-mean2));
			}
		}
		return ret;
	}
	double module(const Mat &v1, unsigned int size)
	{
		double ret = 0;
		int rows,cols;
		rows = v1.rows;
		cols = v1.cols;
		for( int i = 0; i<rows; i++ )
		{
			const float* inData1=v1.ptr<float>(i);
			for(int j=0; j<cols;j++)
			{
				ret +=(inData1[j]*inData1[j]);
			}
		}

		//return ret;
		return sqrt(ret);
	}
	double cosionDis(const Mat &v1, const Mat &v2, int size)
	{
		return (dotCompute(v1,v2, size)/((module(v1, size)*module(v2, size))));
	}
	double dotCompute(const unsigned char* v1, const unsigned char *v2,unsigned int size)
	{
		double ret = 0;
		for(unsigned int i=0;i<size; i++)
		{
			ret += (v1[i]*v2[i]);
		}
		return ret;
	}
	double module(const unsigned char* v1, unsigned int size)
	{
		double ret = 0;
		for(unsigned int i=0;i<size; i++)
		{
			ret += (v1[i]*v1[i]);
		}
		return sqrt(ret);
		
	}
	double cosionDis(const unsigned char* v1, const unsigned char *v2,unsigned int size)
	{
		return (dotCompute(v1,v2, size)/((module(v1, size)*module(v2, size))));
	}
		double ManhaDis(const Mat&v1, const Mat &v2, int size)
		{
			double ret = 0;
			int rows,cols;
			rows = v1.rows;
			cols = v1.cols;
			for( int i = 0; i<rows; i++ )
			{
				const float* inData1=v1.ptr<float>(i);
				const float* inData2=v2.ptr<float>(i);
				for(int j=0; j<cols;j++)
				{
					ret +=abs(inData1[j]-inData2[j]);
				}
			}
			return ret;
		}
	double cvDis(const Mat &v1, const Mat &v2, int size)
	{
		//CV_COMP_CHISQR,CV_COMP_INTERSECT,CV_COMP_BHATTACHARYYA  
		double correl;
		//double chisqr, intersect, bhattacharyya ;
		correl = cv::compareHist(v1, v2, CV_COMP_CORREL);
		//chisqr = cv::compareHist(v1, v2, CV_COMP_CHISQR);
		//intersect = cv::compareHist(v1, v2, CV_COMP_INTERSECT);
		//bhattacharyya = cv::compareHist(v1, v2, CV_COMP_BHATTACHARYYA);
		//cout<<"correl="<<correl<<" chisqr="<<chisqr<<" intersect="<<intersect<<" bhattacharyya="<<bhattacharyya<<endl;
		return correl;
	}
	void PrintMat(Mat src)
	{
		int rows,cols;
		rows = src.rows;
		cols = src.cols;
		for( int i = 0; i<rows; i++ )
		{
			const float* inData1=src.ptr<float>(i);
			for(int j=0; j<cols;j++)
			{
				//cout<<(*inData1++)<<" ";
				cout<<(inData1[j])<<" ";
			}
			cout<<endl;
		}
	}
	double varplus(unsigned char*a, unsigned char* b, int size)
	{
		double distance;
		for(int i=0;i<size;i++)
		{
				//distance += abs(pa[j]+pb[j]);
				distance += (abs(a[i])+abs(b[i]));
		}
		return distance;
	}
	double vardiff(const unsigned char*a, const unsigned char* b, int size)
	{
		double distance;
		for(int i=0;i<size;i++)
		{
				//distance += abs(pa[j]+pb[j]);
				distance += (abs(a[i]-b[i]));
		}
		cout<<"vardiff="<<distance<<"cal="<<-log(distance+1)<<endl;
		return distance;
	}
	double varplus(Mat a, Mat b, int size)
	{
		double distance;
		int rows = a.rows;
		int cols = b.cols;
		for(int i=0;i<rows;i++)
		{
			float *pa= a.ptr<float>(i);
			float *pb= b.ptr<float>(i);
			for(int j=0;j<cols;j++)
			{
				//distance += abs(pa[j]+pb[j]);
				distance += (abs(pa[j])+abs(pb[j]));
			}
		}
		return distance;
	}
	double vardiff(Mat &a, Mat &b, int size)
	{
		double distance = 0;
		int rows = a.rows;
		int cols = b.cols;
		for(int i=0;i<rows;i++)
		{
			float *pa= a.ptr<float>(i);
			float *pb= b.ptr<float>(i);
			for(int j=0;j<cols;j++)
			{
				if((pa[j]>0 && pb[j]<0)||(pa[j]<0&&pb[j]>0))
					distance += abs(pa[j]-pb[j])*2;
				else
					distance +=abs(pa[j]-pb[j]);
			}
		}
		return distance;
	}
	double chiSquare(Mat &a ,Mat &b, int size)
	{
		return (vardiff(a,b,size)/varplus(a, b, size));
	}

};
#endif
