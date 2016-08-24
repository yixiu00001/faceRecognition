#include "imgproc.h"
//#include "imgprocalg.h"
#include "mathhelper.h"

cv::Size getKernelSize(double sigma)
{
	// Inverts OpenCV's conversion from kernel size to sigma:
	// sigma = ((ksize-1)*0.5 - 1)*0.3 + 0.8
	// See documentation for cv::getGaussianKernel()
	int ksize = (int)(((sigma - 0.8) / 0.3 + 1) * 2 + 1);
	if (ksize % 2 == 0) ksize++;
	return cv::Size(ksize, ksize);
}

void CFilterAlgorithm::Dog( cv::Mat &srcmat,cv::Mat &dstmat,float a /*= 1.0f*/,float b /*= 2.0f*/ )
{
	cv::Size ksize0, ksize1;

	ksize0 = getKernelSize(a);
	ksize1 = getKernelSize(b);

	cv::Mat g0, g1;
	cv::GaussianBlur(srcmat, g0, ksize0, 0);
	cv::GaussianBlur(srcmat, g1, ksize1, 0);
	cv::subtract(g0, g1, dstmat);
}

void CFilterAlgorithm::Gamma( cv::Mat &srcmat,cv::Mat &dstmat )
{
	static cv::Mat lutmat;

	if(lutmat.empty())
	{
		lutmat.create(256, 1, CV_32FC1);
		for (int i=0; i<256; i++) 
			lutmat.at<float>(i,0) = (float)pow(i, 0.2f);
	}

	if (srcmat.depth() == CV_8U) cv::LUT(srcmat, lutmat, dstmat);
	else                          cv::pow(srcmat, 0.2f, dstmat);
}

void CFilterAlgorithm::ContrastEq( cv::Mat &srcmat,cv::Mat &dstmat,float a /*= 0.1f*/,float t /*= 10.0f */ )
{
	// Stage 1
	cv::Mat stage1;
	{
		cv::Mat abs_dst;
		cv::absdiff(srcmat, cv::Scalar(0), abs_dst);
		cv::Mat pow_dst;
		cv::pow(abs_dst, a, pow_dst);
		float denominator = cv::pow((float)cv::mean(pow_dst)[0], 1.f/a);
		srcmat.convertTo(stage1, CV_32F, 1/denominator);
	}

	// Stage 2
	cv::Mat stage2;
	{
		cv::Mat abs_dst;
		cv::absdiff(stage1, cv::Scalar(0), abs_dst);
		cv::Mat min_dst;
		cv::min(abs_dst, t, min_dst);
		cv::Mat pow_dst;
		cv::pow(min_dst, a, pow_dst);
		float denominator = cv::pow((float)cv::mean(pow_dst)[0], 1.f/a);
		stage1.convertTo(stage2, CV_32F, 1/denominator);
	}

	// Hyperbolic tangent
	const int nRows = srcmat.rows;
	const int nCols = srcmat.cols;
	const float* p = (const float*)stage2.ptr();
	cv::Mat m(nRows, nCols, CV_32FC1);
	for (int i=0; i<nRows; i++)
		for (int j=0; j<nCols; j++)
			m.at<float>(i, j) = fast_tanh(p[i*nCols+j]); 

	dstmat = m;
}

void CNumberAlgorithm::Normalize( cv::Mat &srcmat,cv::Mat &dstmat,NormType type )
{
	cv::normalize(srcmat, dstmat, 1, 0, type, CV_32F);
}

void CNumberAlgorithm::Hist( cv::Mat &srcmat,cv::Mat &dstmat,float _max /*= 256*/,float _min /*= 0*/,int _dims /*= -1*/ )
{
	const int dims = _dims == -1 ? (int)(_max - _min) : _dims;

	vector<cv::Mat> mv;
	cv::split(srcmat, mv);
	cv::Mat m(mv.size(), dims, CV_32FC1);

	for (size_t i=0; i<mv.size(); i++) {
		int channels[] = {0};
		int histSize[] = {dims};
		float range[] = {_min, _max};
		const float* ranges[] = {range};
		cv::Mat hist, chan = mv[i];
		// calcHist requires F or U, might as well convert just in case
		if (mv[i].depth() != CV_8U && mv[i].depth() != CV_32F)
			mv[i].convertTo(chan, CV_32F);
		cv::calcHist(&chan, 1, channels, cv::Mat(), hist, 1, histSize, ranges);
		memcpy(m.ptr(i), hist.ptr(), dims * sizeof(float));
	}

	dstmat = m;
}

void CNumberAlgorithm::Quantize( cv::Mat &srcmat,cv::Mat &dstmat,float a /*= 5093.03f*/,float b /*= 128.00f*/ )
{
	srcmat.convertTo(dstmat, CV_8U, a, b);
}

void CRegionsAlgorithm::RectRegions( cv::Mat &srcmat,vector<cv::Mat> &dst,int width /*= 8*/, int height /*= 8*/,int _widthStep /*= 6*/,int _heightStep /*= 6*/ )
{
	const int widthStep = _widthStep == -1 ? width : _widthStep;
	const int heightStep = _heightStep == -1 ? height : _heightStep;
	const cv::Mat &m = srcmat;
	const int xMax = m.cols - width;
	const int yMax = m.rows - height;
	for (int x=0; x <= xMax; x += widthStep)
		for (int y=0; y <= yMax; y += heightStep)
			dst.push_back(m(cv::Rect(x, y, width, height)));
}

void CRegionsAlgorithm::ByRow( cv::Mat &srcmat,vector<cv::Mat> &dst )
{
	for (int i=0; i<srcmat.rows; i++)
		dst.push_back(srcmat.row(i));
}

void CRegionsAlgorithm::Cat( vector<cv::Mat> &src,vector<cv::Mat> &dst,int partitions )
{
	if (src.size() % partitions != 0)
		return;

	vector<int> sizes(partitions, 0);
	for (int i=0; i<(int)src.size(); i++)
		sizes[i%partitions] += src[i].total();

	if (!src.empty())
		for (int i = 0;i < (int)sizes.size();i++)
		dst.push_back(cv::Mat(1, sizes[i], src[0].type()));

	vector<int> offsets(partitions, 0);
	for (int i=0; i<(int)src.size(); i++) {
		size_t size = src[i].total() * src[i].elemSize();
		int j = i % partitions;
		memcpy(&dst[j].data[offsets[j]], src[i].ptr(), size);
		offsets[j] += size;
	}
	// cout<<"ccat=============="<<endl;
	// for(int i=0;i<dst.size();i++)
	//	cout<<i<<"="<<dst[i]<<endl;
}
