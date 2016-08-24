/*
lda Algorithm
*/
#ifndef lda_h__
#define lda_h__


#include "eigenheader.h"

class CLda
{
private:

	float pcaKeep;
	int directLDA;
	float directDrop;
	int dimsOut;
	bool pcaWhiten;
	Eigen::VectorXf mean;
	Eigen::MatrixXf projection;

public:
	CLda(float pcaKeep = 0.98f,int directLDA = 0,float directDrop = 0.1f,int dimsOut = 1,bool pcaWhiten = false)
	{
		this->pcaKeep = pcaKeep;
		this->directLDA = directLDA;
		this->directDrop = directDrop;
		this->dimsOut = dimsOut;
		this->pcaWhiten = pcaWhiten;
	}
	~CLda()
	{}

	bool Project(vector<cv::Mat> &src,vector<cv::Mat> &dst);

	bool Load(CDataStream &stream);
	bool Load(const string& filepath);
	bool Save(const string& filepath);

	bool Save(CDataStream &stream);

	bool Train(vector<vector<cv::Mat> > &matlist,const vector<int> &classlist);

};

#endif // lda_h__
