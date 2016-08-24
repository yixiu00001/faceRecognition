#ifndef _FaceRect_h
#define _FaceRect_h

#include "FaceRect.h"
#include "../include/stasm_lib.h"
#include "../include/stasm_landmarks.h"
class CFaceDetect
{
	private:
		
		bool detectByAsm(const Mat &faceImage,vector<CFaceRect> &rclist,bool onlybig);
		void getAsmPointer(float *landmarks,Rect &rcface);
	public:
		CFaceDetect();

		~CFaceDetect();
		bool Init(const char*path , int trace);

		bool Detect(const Mat &srcimg,vector<CFaceRect> &outputrectlist,bool onlybig);
		bool DetectMarks( const Mat &imagemat,vector<float *> &vlandmarks,bool onlybig );
		bool detectByAsmMarks( const Mat &faceImage,vector<float*> &vlandmarks,bool onlybig );
};


#endif
