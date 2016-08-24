/*
cvTool
*/
#ifndef cvtool_h__
#define cvtool_h__


#include "Serialization.h"

CDataStream &operator>>(CDataStream &stream, cv::Mat &m);

CDataStream &operator<<(CDataStream &stream, const cv::Mat &m);

cv::Mat ConverToMat(char *buff,int width,int height,int bpp,int size);

int MatToChar(cv::Mat &srcmat,char *buff);

cv::Mat CharToMat(char *buff);

cv::Mat VectorToMat(vector<cv::Mat> &matlist);

#endif // cvtool_h__