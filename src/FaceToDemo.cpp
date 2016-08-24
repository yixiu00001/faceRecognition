/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*

*
* Last modified: 2016-03-09 09:19
*
* Filename: FaceToDemo.cpp
*
* Description: the interface to jsp demo 
*
=============================================================================*/
#include "FaceToDemo.h"
#include "FaceDetect.h"
#include "FaceRecognition.h"
#include "FaceCompare.h"
#include "cvtool.h"


CFaceDetect faceDetect;
CFaceRecognition faceRecognition;
CFaceCompare faceCompare; 
		
bool CFaceDemo::initmodel(const char *filepath)
{
	if(!faceDetect.Init(filepath, 1) || !faceRecognition.Load(filepath) || !faceCompare.Load(filepath))
	{
		return false;
	}

	return true;
	
}
// false ,return -1, else ,num of faces 
int CFaceDemo::facefindRect(Mat &img,vector<FACEDIST_RECT> &retlist,bool onlybig)
{
	vector<CFaceRect> rcRectList;
	if(!faceDetect.Detect(img,rcRectList, onlybig))	
	{
		return -1;
	}
	else
	{
		for(unsigned int i=0 ; i<rcRectList.size();i++)
		{
			FACEDIST_RECT rect = {0};

			rect.face_x = rcRectList[i].rcface.x;
			rect.face_y = rcRectList[i].rcface.y;
			rect.face_width = rcRectList[i].rcface.width;
			rect.face_height = rcRectList[i].rcface.height;


			rect.lefteye_x = rcRectList[i].rclefteye.x;
			rect.lefteye_y = rcRectList[i].rclefteye.y;
			
			rect.righteye_x = rcRectList[i].rcrighteye.x;
			rect.righteye_y = rcRectList[i].rcrighteye.y;
			
			rect.mouth_x = rcRectList[i].rcmouth.x;
			rect.mouth_y = rcRectList[i].rcmouth.y;

			retlist.push_back(rect);
		}
		return rcRectList.size();
	}
	return 0;
}

int CFaceDemo::facefindMarks(Mat &img,vector<float *> &vlandmarks,bool onlybig)
{
	
	if(!faceDetect.DetectMarks(img,vlandmarks, onlybig))	
		return -1;
	return 0;
}
// success, true; failed , false
bool CFaceDemo::buildmodel(Mat &img,FACEDIST_RECT &rect,char *retbuf)
{
	CFaceRect rcRect;
	rcRect.rcface = cv::Rect(rect.face_x,rect.face_y,rect.face_width,rect.face_height);
	rcRect.rcfullface = rcRect.rcface;
	rcRect.rclefteye = cv::Rect(rect.lefteye_x,rect.lefteye_y,1,1);
	rcRect.rcrighteye = cv::Rect(rect.righteye_x,rect.righteye_y,1,1);
	rcRect.rcmouth = cv::Rect(rect.mouth_x,rect.mouth_y,1,1);
	

	vector<Mat> dstImgList;
	if(!faceRecognition.Recognition(img, rcRect,dstImgList ))
	{
		return false;
	}
	else
	{
		int count = MatToChar(dstImgList[0], NULL);
		if(retbuf != NULL)
		{
			MatToChar(dstImgList[0], retbuf);
		}
		return true;
	}

	
}

bool CFaceDemo::facecompareModel(char *retbuf1,char *retbuf2,float &similarity)
{
	float dist = 0.0;
	if(retbuf1 == NULL || retbuf2 == NULL)
		return false;
	Mat img1Mat = CharToMat(retbuf1);
	Mat img2Mat = CharToMat(retbuf2);

	if(!faceCompare.Compare(img1Mat, img2Mat, dist))
	{
		return false;
	}
	similarity = dist;

	return true;
}

//人脸比对 图片人脸比对 onlybig 为false时是一对多比对，及img1一张人脸比对img2后面所有的人脸 true时img2一张人脸
//实参-1表示执行失败或者没有人脸，0 1 表示img2中人脸张数 similarity 相似度向量
int CFaceDemo::facecompareImg(Mat &img1,Mat &img2,bool onlybig, vector<float> &similarity)
{
	vector<CFaceRect> srcRect1;
	vector<CFaceRect> srcRect2;

	faceDetect.Detect(img1, srcRect1, true);


	{
		faceDetect.Detect(img2, srcRect2, onlybig);
		if(srcRect1.empty() || srcRect2.empty())
		{
			return -1;
		}
		vector<Mat> dstImgList1;
		float dist = 0.0;

		faceRecognition.Recognition(img1, srcRect1[0], dstImgList1);
		for(unsigned int i=0;i< srcRect2.size();i++)
		{
			vector<Mat> dstImgList2;
			faceRecognition.Recognition(img2, srcRect2[i], dstImgList2);
			
			faceCompare.Compare(dstImgList1[0], dstImgList2[0], dist);
			similarity.push_back(dist);
		}
		return  srcRect2.size(); 
	}


}

