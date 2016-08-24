#ifndef _FaceRecognition_h
#define _FaceRecognition_h


#include "FaceRect.h"
#include "lbp.h"
#include "sift.h"
#include "pca.h"

class CFaceRecognition
{
	public:
		vector<Eigen::MatrixXf> vecList;
		CLda clda;
		CFaceRecognition( );
		~CFaceRecognition( );
		bool Recognition( char* dir,char* name, Mat &srcmat,CFaceRect &facerect,vector<Mat> &dstmat );
		bool Recognition( Mat &srcmat,CFaceRect &facerect,vector<Mat> &dstmat );
		bool RecognitionPCATrain( Mat &srcmat,CFaceRect &facerect,vector<Mat> &dstmat );
		bool RecognitionLDATrain( Mat &srcmat,CFaceRect &facerect,vector<Mat> &dstmat );
		bool Load(String filepath);
};

#endif
