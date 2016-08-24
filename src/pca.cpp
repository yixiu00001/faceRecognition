/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*
*include "pca.h"
*
* Last modified: 2015-12-21 10:37
*
* Filename: pca.cpp
*
* Description: train and compute the pca feature
*
=============================================================================*/
#include "Serialization.h"
#include "pca.h"

bool CPca::Project( vector<cv::Mat> &src,vector<cv::Mat> &dst )
{
	cv::Mat m;
	m = cv::Mat(1, (int)keep, CV_32FC1);

	// Map Eigen into OpenCV
	Eigen::Map<const Eigen::MatrixXf> inMap(src[0].ptr<float>(), src[0].rows*src[0].cols, 1);
	Eigen::Map<Eigen::MatrixXf> outMap(m.ptr<float>(), (int)keep, 1);

	// Do projection
	outMap = eVecs.transpose() * (inMap - mean);
	dst.push_back(m);

	return true;
}
bool CPca::Pca(cv::Mat &src, cv::Mat &dst)
{
	dst = cv::Mat(1, (int)keep, CV_32FC1);
	
	Eigen::Map<const Eigen::MatrixXf> inMap(src.ptr<float>(), src.rows*src.cols, 1);
	Eigen::Map<Eigen::MatrixXf> outMap(dst.ptr<float>(), 1, (int)keep);
		
	outMap = inMap.transpose() * eVecs;

	return true;
}

bool CPca::Load( CDataStream &stream )
{
	stream >> keep >> drop >> whiten >> originalRows >> mean >> eVals >> eVecs;

	return true;
}

bool CPca::Save( CDataStream &stream )
{
	stream << keep << drop << whiten << originalRows << mean << eVals << eVecs;

	return true;
}
bool CPca::Load(const string& filepath)
{
	CFileDataStream stream; 
	if(!stream.load(filepath)) 
	{
		return false;
	}
	return Load(stream);
}
bool CPca::Train( vector<vector<cv::Mat> > &matlist )
{
	for(unsigned int i=0;i<matlist.size();i++)
	{
		this->keep = 0.95f;
		this->drop = 0;
		this->whiten = false;
		this->originalRows = 1;
		vector<Mat> matlistTmp = matlist[i];
		if (matlistTmp[0].type() != CV_32FC1)
			return false;

		originalRows = matlistTmp[0].rows;
		int dimsIn = matlistTmp[0].rows * matlistTmp[0].cols;
		int instances = matlistTmp.size();
		cout<<"In="<<dimsIn<<" instances="<<instances<<endl;
		// Map into 64-bit Eigen matrix
		Eigen::MatrixXd data(dimsIn, instances);
		for (int j=0; j<instances; j++)
		{
			//cout<<"src="<<matlist[i]<<endl;
			data.col(j) = Eigen::Map<const Eigen::MatrixXf>(matlistTmp[j].ptr<float>(), dimsIn, 1).cast<double>();
		}
		if(!trainCore(data))
			cout<<"----index "<<i <<"pcaTrain failed!"<<endl;
		char pcafile[260] = {0};
		const String filepath="model";
		sprintf(pcafile,"%s/pca296/pca%d",filepath.c_str(),i);
		cout<<"feaName="<<pcafile<<endl;
		CDataStream stream;
		if(!stream.load(pcafile, true)) 
		{
			cout<<"load file failed!"<<pcafile<<endl;
		}
		Save(stream);
		cout<<"eVals="<<eVals<<endl;
		cout<<"eVecs="<<eVecs<<endl;
	}

		//return trainCore(data);
}

bool CPca::trainCore( Eigen::MatrixXd data )
{
	int dimsIn = data.rows();
	int instances = data.cols();
	const bool dominantEigenEstimation = (dimsIn > instances);
	
	Eigen::MatrixXd allEVals, allEVecs;
	if (keep != 0) {
		// Compute and remove mean
		mean = Eigen::VectorXf(dimsIn);
		for (int i=0; i<dimsIn; i++)
		{
			//cout<<"row="<<data.row(i)<<endl;
			mean(i) = (float)data.row(i).sum() / (float)instances;
		}
		for (int i=0; i<dimsIn; i++) 
		{
			data.row(i).array() -= mean(i);
		}

		// Calculate covariance matrix
		Eigen::MatrixXd cov;
		if (dominantEigenEstimation) cov = data.transpose() * data / (instances-1.0);
		else                         cov = data * data.transpose() / (instances-1.0);

		// Compute eigendecomposition. Returns eigenvectors/eigenvalues in increasing order by eigenvalue.
		//Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eSolver(cov);
		Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eSolver(cov);
		allEVals = eSolver.eigenvalues();
		allEVecs = eSolver.eigenvectors();
		cout<<"allEvals sum = "<<allEVals.sum()<<endl;
		if (dominantEigenEstimation) allEVecs = data * allEVecs;
	} else {
		// Null case
		mean = Eigen::VectorXf::Zero(dimsIn);
		allEVecs = Eigen::MatrixXd::Identity(dimsIn, dimsIn);
		allEVals = Eigen::VectorXd::Ones(dimsIn);
	}

	if (keep <= 0) {
		keep = (float)(dimsIn - drop);
	} else if (keep < 1) {
		// Keep eigenvectors that retain a certain energy percentage.
		const double totalEnergy = allEVals.sum();
		if (totalEnergy == 0) {
			keep = 0;
		} else {
			double currentEnergy = 0;
			int i=0;
			while ((currentEnergy / totalEnergy < keep) && (i < allEVals.rows())) {
				currentEnergy += allEVals(allEVals.rows()-(i+1));
				i++;
			}
			keep = (float)(i - drop);
		}
	} else {
		if (keep + drop > allEVals.rows())
			return false;
	}

	// Keep highest energy vectors
	cout<<"keep="<<keep<<endl;
	eVals = Eigen::VectorXf((int)keep, 1);
	eVecs = Eigen::MatrixXf(allEVecs.rows(), (int)keep);
	for (int i=0; i<keep; i++) {
		int index = allEVals.rows()-(i+drop+1);
		eVals(i) = (float)allEVals(index);
		eVecs.col(i) = allEVecs.col(index).cast<float>() / allEVecs.col(index).norm();
		if (whiten) eVecs.col(i) /= sqrt(eVals(i));
	}
	


	return true;
}
bool CPca::Train( vector<cv::Mat>  &matlist )
{
	if (matlist[0].type() != CV_32FC1)
		return false;

	originalRows = matlist[0].rows;
	int dimsIn = matlist[0].rows * matlist[0].cols;
	const int instances = matlist.size();

	// Map into 64-bit Eigen matrix
	Eigen::MatrixXd data(dimsIn, instances);
	for (int i=0; i<instances; i++)
		data.col(i) = Eigen::Map<const Eigen::MatrixXf>(matlist[i].ptr<float>(), dimsIn, 1).cast<double>();

	return trainCore(data);
}
bool CPca::Train( vector<vector<cv::Mat> > &matlist,const vector<int> &classlist )
{
	if (matlist[0][0].type() != CV_32FC1)
		return false;

	originalRows = matlist[0][0].rows;
	int dimsIn = matlist[0][0].rows * matlist[0][0].cols;
	const int instances = matlist[0].size();

	// Map into 64-bit Eigen matrix
	Eigen::MatrixXd data(dimsIn, instances);
	for (int i=0; i<instances; i++)
		data.col(i) = Eigen::Map<const Eigen::MatrixXf>(matlist[0][i].ptr<float>(), dimsIn, 1).cast<double>();

	return trainCore(data);
}
