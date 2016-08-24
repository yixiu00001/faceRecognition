#ifndef paecvutils_hpp__
#define paecvutils_hpp__

#include <opencv2/opencv.hpp>
#include <string.h>

namespace paecvutils
{
	inline int MatToChar( cv::Mat &srcmat,char *buff )
	{
		int size = (int)srcmat.elemSize() * srcmat.rows * srcmat.cols;

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

	inline cv::Mat CharToMat( char *buff )
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
}

#endif // paecvutils_hpp__
