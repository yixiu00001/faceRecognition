#ifndef todemo_h_
#define todemo_h_
#include "Unified.h"
class FACEDIST_RECT
{
	public:
				int face_x;
				int face_y;
				int face_width;
				int face_height;

				int lefteye_x;
				int lefteye_y;

				int righteye_x;
				int righteye_y;

				int mouth_x;
				int mouth_y;
	
};
class CFaceDemo
{
	public:

		
		//模型文件初始化 绝对路径 模型文件根目录 true 成功 flase失败
		bool initmodel(const char *filepath);

		//人脸检测,形参返回矩形坐标，实参-1表示执行失败，0 1 人脸张数，onlybig只返回最大一张人脸
		int facefindRect(Mat &img,vector<FACEDIST_RECT> &retlist,bool onlybig);

		//人脸检测，返回特征点landmark ,实参-1表示执行失败，0 1 人脸张数，onlybig只返回最大一张人脸
		int facefindMarks(Mat &img,vector<float *> &vlandmarks,bool onlybig);

		//人脸建模 rect 人脸矩阵信息 retbuf 模型信息 false 失败 true成功
		bool buildmodel(Mat &img,FACEDIST_RECT &rect,char *retbuf);

		//人脸比对 模型文件比对 similarity返回相似度 false比对失败
		bool facecompareModel(char *retbuf1,char *retbuf2,float &similarity);

		//人脸比对 图片人脸比对 onlybig 为false时是一对多比对，及img1一张人脸比对img2后面所有的人脸 true时img2一张人脸
		//实参-1表示执行失败或者没有人脸，0 1 表示img2中人脸张数 similarity 相似度向量
		int facecompareImg(Mat &img1,Mat &img2,bool onlybig, vector<float> &similarity);


};
#endif
