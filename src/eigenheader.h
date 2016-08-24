#ifndef eigenheader_h__
#define eigenheader_h__


#include "Eigen/Eigen"
#include "Serialization.h"

template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline CDataStream &operator>>(CDataStream &stream, Eigen::Matrix< _Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols > &mat)
{
	int r, c;
	stream >> r >> c;
	mat.resize(r, c);

	_Scalar *data = new _Scalar[r*c];
	int bytes = r*c*sizeof(_Scalar);
	int bytes_read = stream.readraw((char*)data, bytes);
	if (bytes != bytes_read) return stream;
	for (int i=0; i<r; i++)
		for (int j=0; j<c; j++)
			mat(i, j) = data[i*c+j];

	delete[] data;
	return stream;
}

template<typename _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
inline CDataStream &operator<<(CDataStream &stream, const Eigen::Matrix< _Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols > &mat)
{
	int r = (int)mat.rows();
	int c = (int)mat.cols();
	stream << r << c;

	_Scalar *data = new _Scalar[r*c];
	for (int i=0; i<r; i++)
		for (int j=0; j<c; j++)
			data[i*c+j] = mat(i, j);
	int bytes = r*c*sizeof(_Scalar);
	int bytes_written = stream.writeraw((char*)data, bytes);
	if (bytes != bytes_written) return stream;

	delete[] data;
	return stream;
}

#endif // eigenheader_h__
