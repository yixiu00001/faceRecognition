/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*
*include "lda.h"
*
* Last modified: 2015-11-23 10:38
*
* Filename: lda.cpp
*
* Description: train and compute the lda feature
*
=============================================================================*/
#include "Serialization.h"
#include "pca.h"
#include "misc.h"
#include "lda.h"

bool CLda::Project( vector<cv::Mat> &src,vector<cv::Mat> &dst )
{
	cv::Mat m;
	m = cv::Mat(1, dimsOut, CV_32FC1);

	// Map Eigen into OpenCV
	Eigen::Map<Eigen::MatrixXf> inMap((float*)src[0].ptr<float>(), src[0].rows*src[0].cols, 1);
	Eigen::Map<Eigen::MatrixXf> outMap(m.ptr<float>(), dimsOut, 1);

	// Do projection
	outMap = projection.transpose() * (inMap - mean);
	dst.push_back(m);
	//cout<<"lda========"<<m<<endl;

	return true;
}

bool CLda::Load( CDataStream &stream )
{
	stream >> pcaKeep >> directLDA >> directDrop >> dimsOut >> mean >> projection;

	return true;
}
bool CLda::Load(const string& filepath)
{
	CFileDataStream stream;
	if(!stream.load(filepath))
	{
		return false;
	}
	return Load(stream);
}

bool CLda::Save( CDataStream &stream )
{
	stream << pcaKeep << directLDA << directDrop << dimsOut << mean << projection;

	return true;
}

bool CLda::Save(const string& filepath)
{
	CDataStream stream;
	if(!stream.load(filepath,true))
	{
		cout<<"load file failed!"<<filepath<<endl;
		return false;
	}
	return Save(stream);
}
bool CLda::Train( vector<vector<cv::Mat> > &matlist,const vector<int> &classlist )
{
	int instances = (int)matlist[0].size();

	// Perform PCA dimensionality reduction
	CPca pca;
	pca.keep = pcaKeep;
	pca.whiten = pcaWhiten;
	pca.Train(matlist,classlist);
	mean = pca.mean;

	vector<cv::Mat> ldaTrainingSet;
	for(int i = 0;i < (int)matlist[0].size();i++)
	{
		vector<cv::Mat> m;

		vector<cv::Mat> mn(1,matlist[0][i]);
		pca.Project(mn, m);
		ldaTrainingSet.push_back(m[0]);
	}
	
	cout<<endl<<"------pca0eVals"<<pca.eVals<<endl;
	cout<<endl<<"------pca0eVecs"<<pca.eVecs<<endl;

	int dimsIn = ldaTrainingSet[0].rows * ldaTrainingSet[0].cols;

	// OpenBR ensures that class values range from 0 to numClasses-1.
	// Label exists because we created it earlier with relabel
	vector<int> classes = classlist;
	map<int, int> classCounts = sum_class(classlist);
	const int numClasses = (int)classCounts.size();

	// Map Eigen into OpenCV
	Eigen::MatrixXd data = Eigen::MatrixXd(dimsIn, instances);
	for (int i=0; i<instances; i++)
		data.col(i) = Eigen::Map<const Eigen::MatrixXf>(ldaTrainingSet[i].ptr<float>(), dimsIn, 1).cast<double>();

	// Removing class means
	Eigen::MatrixXd classMeans = Eigen::MatrixXd::Zero(dimsIn, numClasses);

	for (int i=0; i<instances; i++)  
	{
		classMeans.col(classes[i]) += data.col(i);
	}
	for (int i=0; i<numClasses; i++) 
	{
		if(classCounts[i]!=0)
			classMeans.col(i) /= classCounts[i];
	}
	for (int i=0; i<instances; i++)  data.col(i) -= classMeans.col(classes[i]);

	CPca space1;

	if (!directLDA)
	{
		// The number of LDA dimensions is limited by the degrees
		// of freedom of scatter matrix computed from 'data'. Because
		// the mean of each class is removed (lowering degree of freedom
		// one per class), the total rank of the covariance/scatter
		// matrix that will be computed in PCA is bound by instances - numClasses.
		space1.keep = (float)std::min(dimsIn, instances-numClasses);
		space1.trainCore(data);

		// Divide each eigenvector by sqrt of eigenvalue.
		// This has the effect of whitening the within-class scatter.
		// In effect, this minimizes the within-class variation energy.
		for (int i=0; i<space1.keep; i++) space1.eVecs.col(i) /= pow((double)space1.eVals(i),0.5);
	}
	else if (directLDA == 2)
	{
		space1.drop = instances - numClasses;
		space1.keep = (float)std::min(dimsIn, instances) - space1.drop;
		space1.trainCore(data);
	}
	else
	{
		// Perform (modified version of) Direct LDA

		// Direct LDA uses to the Null space of the within-class scatter.
		// Thus, the lower rank, is used to our benefit. We are not discarding
		// these vectors now (in non-direct code we use the keep parameter
		// to discard Null space). We keep the Null space b/c this is where
		// the within-class scatter goes to zero, i.e. it is very useful.
		space1.keep = dimsIn;
		space1.trainCore(data);

		if (dimsIn > instances - numClasses) {
			// Here, we are replacing the eigenvalue of the  null space
			// eigenvectors with the eigenvalue (divided by 2) of the
			// smallest eigenvector from the row space eigenvector.
			// This allows us to scale these null-space vectors (otherwise
			// it is a divide by zero.
			double null_eig = space1.eVals(instances - numClasses - 1) / 2;
			for (int i = instances - numClasses; i < dimsIn; i++)
				space1.eVals(i) = null_eig;
		}

		// Drop the first few leading eigenvectors in the within-class space
		vector<float> eVal_list; 
		eVal_list.reserve(dimsIn);
		float fmax = -1;
		for (int i=0; i<dimsIn; i++) 
			fmax = std::max(fmax, space1.eVals(i));
		for (int i=0; i<dimsIn; i++) 
		{
			if(fmax!=0)
				eVal_list.push_back(space1.eVals(i)/fmax);
		}

		vector<float> dSum = CumSum(eVal_list);
		int drop_idx;
		for (drop_idx = 0; drop_idx<dimsIn; drop_idx++)
			if (dSum[drop_idx]/dSum[dimsIn-1] >= directDrop)
				break;

		drop_idx++;
		space1.keep = dimsIn - drop_idx;

		cout<<endl<<"------pca1res vals"<<space1.eVals<<endl<<"Vecs="<<space1.eVals<<endl;
		Eigen::MatrixXf new_vecs = Eigen::MatrixXf(space1.eVecs.rows(), (int)space1.keep);
		Eigen::MatrixXf new_vals = Eigen::MatrixXf((int)space1.keep, 1);

		for (int i = 0; i < space1.keep; i++) {
			new_vecs.col(i) = space1.eVecs.col(i + drop_idx);
			new_vals(i) = space1.eVals(i + drop_idx);
		}

		space1.eVecs = new_vecs;
		space1.eVals = new_vals;
		cout<<"------space1 eVals="<<space1.eVals<<endl;
		cout<<"------space1 eVecs="<<space1.eVecs<<endl;

		// We will call this "agressive" whitening. Really, it is not whitening
		// anymore. Instead, we are further scaling the small eigenvalues and the
		// null space eigenvalues (to increase their impact).
		for (int i=0; i<space1.keep; i++) space1.eVecs.col(i) /= pow((double)space1.eVals(i),0.15);
	}

	// Now we project the mean class vectors into this second
	// subspace that minimizes the within-class scatter energy.
	// Inside this subspace we learn a subspace projection that
	// maximizes the between-class scatter energy.
	Eigen::MatrixXd mean2 = Eigen::MatrixXd::Zero(dimsIn, 1);

	// Remove means
	for (int i=0; i<dimsIn; i++)     
	{
		if(numClasses!=0)
			mean2(i) = classMeans.row(i).sum() / numClasses;
	}
	for (int i=0; i<numClasses; i++)
	{
		classMeans.col(i) -= mean2;
	}

	// Project into second subspace
	cout<<"input space1.eVecs="<<space1.eVecs.transpose()<<endl<<"classMean="<<classMeans<<endl;
	Eigen::MatrixXd data2 = space1.eVecs.transpose().cast<double>() * classMeans;

	// The rank of the between-class scatter matrix is bound by numClasses - 1
	// because each class is a vector used to compute the covariance,
	// but one degree of freedom is lost removing the global mean.
	int dim2 = std::min((int)space1.keep, numClasses-1);
	CPca space2;
	space2.keep = dim2;
	cout<<"space2 keep="<<space2.keep<<endl;
	space2.trainCore(data2);

	// Compute final projection matrix
	projection = ((space2.eVecs.transpose() * space1.eVecs.transpose()) * pca.eVecs.transpose()).transpose();
	dimsOut = dim2;
	
	const string filepath="model/lda";
	Save(filepath);
	cout<<"ldaname="<<filepath<<endl;
	cout<<"projection="<<projection<<endl;

	return true;
}
