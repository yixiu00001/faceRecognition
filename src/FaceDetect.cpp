/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*
*
* Last modified: 2015-11-18 9:33
*
* Filename: FaceDetect.cpp
*
* Description: detect the face 
*
=============================================================================*/
#include "FaceDetect.h"
CFaceDetect::CFaceDetect()
{
}
CFaceDetect::~CFaceDetect()
{
}
bool CFaceDetect::Init(const char*path , int trace)
{

	return stasm_init(path, trace);

}
static void printLandmarks(const float* landmarks)
{
	cout<<"=====Func==PrintLandmarks=========="<<endl;	
	for (int i = 0; i < stasm_NLANDMARKS; i++)
		stasm_printf("%3d: %4.0f %4.0f\n",
				i, landmarks[i*2], landmarks[i*2+1]);//x,y
}
static void markLandmarks(cv::Mat_<unsigned char> &img,float landmarks[],int nlandmarks=stasm_NLANDMARKS)
{
	for(int i=0;i<nlandmarks;i++)
	{
		if(i==L_LPupil || i==L_RPupil ||i==L_CTopOfTopLip)
		img(cvRound(landmarks[i*2+1]),cvRound(landmarks[2*i]))=255;
	}
}

static void drawLandmarks(
		cv::Mat_<unsigned char>& img,
		float                    landmarks[],
		int                      nlandmarks = stasm_NLANDMARKS)
{
	for (int i = 0; i < nlandmarks-1; i++)
	{
		const int ix  = cvRound(landmarks[i*2]);       // this point
		const int iy  = cvRound(landmarks[i*2+1]);
		const int ix1 = cvRound(landmarks[(i+1)*2]);   // next point
		const int iy1 = cvRound(landmarks[(i+1)*2+1]);
		cv::line(img,
				cv::Point(ix, iy), cv::Point(ix1, iy1), 255, 1);
	}
}
bool  CFaceDetect::DetectMarks( const Mat &imagemat,vector<float *> &vlandmarks,bool onlybig )
{

	Mat graymat;

	cvtColor( imagemat, graymat, CV_BGR2GRAY );
	//cv::equalizeHist(graymat,graymat);

	detectByAsmMarks(graymat,vlandmarks,onlybig);

	return true;
}
bool CFaceDetect::detectByAsmMarks( const Mat &faceImage,vector<float *> &vlandmarks,bool onlybig )
{
	int foundface = 0;
	Mat_<unsigned char> matimage = faceImage;
	//detect face
	if (!stasm_open_image((const char*)matimage.data, faceImage.cols, faceImage.rows, "image",
				onlybig?0:1 /*multiface*/, 10 /*minwidth*/))
	{
		return false;
	}
	Mat_<unsigned char> outimg(faceImage.clone());
	for(;;)
	{
		CFaceRect facerect;
		float *landmarks = new float[2 * stasm_NLANDMARKS]; // x,y coords (note the 2)
		//find all the face
		if(!stasm_search_auto(&foundface, landmarks))
		{
			return false;
		}

		if(foundface == 0)
		{
			break;
		}

		facerect.noalign = false;
		//landmarks inner the image
		stasm_force_points_into_image(landmarks, faceImage.cols, faceImage.rows);
		//printLandmarks(landmarks);
		//drawLandmarks(outimg, landmarks);
		//markLandmarks(outimg, landmarks);
		//imshow("tmp", outimg);

		vlandmarks.push_back(landmarks);
	}
	return true;
}

bool CFaceDetect::Detect( const Mat &imagemat,vector<CFaceRect> &rclist,bool onlybig )
{

	Mat graymat;

	cvtColor( imagemat, graymat, CV_BGR2GRAY );
	//cv::equalizeHist(graymat,graymat);

	detectByAsm(graymat,rclist,onlybig);

	return true;
}


bool CFaceDetect::detectByAsm( const Mat &faceImage,vector<CFaceRect> &rclist,bool onlybig )
{
	int foundface = 0;
	float landmarks[2 * stasm_NLANDMARKS]; // x,y coords (note the 2)
	//if(!stasm_init("../model",0))
	//{
	//	return ;
	//}
	Mat_<unsigned char> matimage = faceImage;
	//detect face
	if (!stasm_open_image((const char*)matimage.data, faceImage.cols, faceImage.rows, "image",
				onlybig?0:1 /*multiface*/, 10 /*minwidth*/))
	{
		//cout<<"detect face failed !!"<<stasm_lasterr()<<endl;	
		return false;
	}
	Mat_<unsigned char> outimg(faceImage.clone());
	for(;;)
	{
		CFaceRect facerect;
		//find all the face
		if(!stasm_search_auto(&foundface, landmarks))
		{
			return false;
		}

		if(foundface == 0)
		{
			break;
		}

		facerect.noalign = false;
		//landmarks inner the image
		stasm_force_points_into_image(landmarks, faceImage.cols, faceImage.rows);
		getAsmPointer(landmarks,facerect.rcface);
		//printLandmarks(landmarks);
		//drawLandmarks(outimg, landmarks);
		//markLandmarks(outimg, landmarks);

		facerect.rcfullface = facerect.rcface;
		facerect.rclefteye = Rect((int)landmarks[L_LPupil * 2],(int)landmarks[L_LPupil * 2 + 1],1,1);
		facerect.rcrighteye = Rect((int)landmarks[L_RPupil * 2],(int)landmarks[L_RPupil * 2 + 1],1,1);
		facerect.rcmouth = Rect((int)landmarks[L_CTopOfTopLip * 2],(int)landmarks[L_CTopOfTopLip * 2 + 1],1,1);
		//cout<<"eye "<<facerect.rclefteye.x<<" "<<facerect.rclefteye.y<<" "<<facerect.rcrighteye.x<<" "<<facerect.rcrighteye.y<<" "<<facerect.rcmouth.x<<" "<<facerect.rcmouth.y<<endl;
		outimg(cvRound(landmarks[L_LPupil*2+1]),cvRound(landmarks[2*L_LPupil]))=255;
		outimg(cvRound(landmarks[L_RPupil*2+1]),cvRound(landmarks[2*L_RPupil]))=255;
		outimg(cvRound(landmarks[L_CTopOfTopLip*2+1]),cvRound(landmarks[2*L_CTopOfTopLip]))=255;
		rclist.push_back(facerect);
	}
	return true;
	//imwrite("test_stasm_lib_auto.bmp", outimg);

}

void CFaceDetect::getAsmPointer( float *landmarks,Rect &rcface )
{
	vector<float> xlist;
	vector<float> ylist;

	for (int i = 0; i < stasm_NLANDMARKS; i++)
	{
		xlist.push_back((float)cvRound(landmarks[i*2]));
		ylist.push_back((float)cvRound(landmarks[i*2+1]));
	}

	std::sort(xlist.begin(),xlist.end());
	std::sort(ylist.begin(),ylist.end());

	rcface.x = (int)xlist[0];
	rcface.y = (int)ylist[0];
	rcface.width = (int)(xlist[xlist.size() - 1] - xlist[0]);
	rcface.height = (int)(ylist[ylist.size() - 1] - ylist[0]);
}

