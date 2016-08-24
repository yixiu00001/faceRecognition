/*
pca Algorithm
*/
#ifndef pca_h__
#define pca_h__


#include "eigenheader.h"
#include "lda.h"

class CPca
{
private:

	friend class CLda;

	int drop;
	bool whiten;
	int originalRows;


private:

	bool trainCore(Eigen::MatrixXd data);

public:
	float keep;
	Eigen::VectorXf mean, eVals;
	Eigen::MatrixXf eVecs;
	CPca(float keep = 0.95f,int drop = 0,bool whiten = false,int originalRows = 1)
	{
		this->keep = keep;
		this->drop = drop;
		this->whiten = whiten;
		this->originalRows = originalRows;
	}
	~CPca()
	{}

	bool Project(vector<cv::Mat> &src,vector<cv::Mat> &dst);
	bool Pca(cv::Mat &src, cv::Mat &dst);

	bool Load(CDataStream &stream);

	bool Save(CDataStream &stream);

	bool Load(const string& fileName);
	bool Train(vector<vector<cv::Mat> > &matlist,const vector<int> &classlist);
	bool Train(vector<vector<cv::Mat> >&matlist);
	bool Train(vector<cv::Mat> &matlist);

};

#endif // pca_h__
