#include "cvtool.h"

CDataStream &operator>>(CDataStream &stream, cv::Mat &m)
{
	// Read header
	int rows, cols, type;
	stream >> rows >> cols >> type;
	m.create(rows, cols, type);

	// Read data
	int len;
	stream >> len;
	if (len > 0) {
		if (!m.isContinuous()) return stream;
		int written = stream.readraw((char*)m.data, len);
		if (written != len) return stream;
	}
	return stream;
}

CDataStream &operator<<(CDataStream &stream, const cv::Mat &m)
{
	// Write header
	int rows = m.rows;
	int cols = m.cols;
	int type = m.type();
	stream << rows << cols << type;

	// Write data
	int len = rows*cols*m.elemSize();
	stream << len;
	if (len > 0) {
		if (!m.isContinuous()) return stream;
		int written = stream.writeraw((char*)m.data, len);
		if (written != len) return stream;
	}
	return stream;
}

cv::Mat ConverToMat( char *buff,int width,int height,int bpp,int size )
{
	int nChannels = bpp == 1 ? 1 : bpp/8 ;
	int depth = bpp == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;

	IplImage *img = cvCreateImageHeader(cv::Size(width,height),depth,nChannels);
	if(NULL == img)
	{
		return cv::Mat();
	}

	img->imageData = (char*)malloc(width * height * nChannels);
	memcpy(img->imageData,buff,width * height * nChannels);

	cv::Mat imgmat(img,true);

	cvReleaseImage(&img);

	return imgmat;
}

int MatToChar( cv::Mat &srcmat,char *buff )
{
	int size = srcmat.elemSize() * srcmat.rows * srcmat.cols;

	if(buff != NULL)
	{
		char *curptr = buff;

		*(int*)curptr = srcmat.rows;
		curptr += sizeof(int);

		*(int*)curptr = srcmat.cols;
		curptr += sizeof(int);

		*(int*)curptr = srcmat.type();
		curptr += sizeof(int);
	
		*(int*)curptr = size; 
		curptr += sizeof(int);

		memcpy(curptr,srcmat.data,size);
	}

	return size + (sizeof(int) * 4);
}

cv::Mat CharToMat( char *buff )
{
	cv::Mat m;
	char *curptr = buff;

	int rows = *(int*)curptr;
	curptr += sizeof(int);
	int cols = *(int*)curptr;
	curptr += sizeof(int);
	int type = *(int*)curptr;
	curptr += sizeof(int);
	int len = *(int*)curptr;
	curptr += sizeof(int);

	m.create(rows,cols,type);

	memcpy(m.data,curptr,len);

	return m;
}

cv::Mat VectorToMat( vector<cv::Mat> &matlist )
{
	if (matlist.empty()) return cv::Mat();

	int rows = matlist.size();
	size_t total = matlist[0].total();
	int type = matlist[0].type();
	cv::Mat dst(rows, total, type);

	for (int i=0; i<rows; i++) {
		const cv::Mat &m = matlist[i];
		if ((m.total() != total) || (m.type() != type) || !m.isContinuous())
			return cv::Mat();
		memcpy(dst.ptr(i), m.ptr(), total * matlist[0].elemSize());
	}
	return dst;
}
