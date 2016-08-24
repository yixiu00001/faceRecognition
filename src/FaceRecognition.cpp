/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*
*include "FaceRecognition.h"
*
* Last modified: 2015-11-18 10:33
*
* Filename: FaceRecognition.cpp
*
* Description: compute the feature of image
*
=============================================================================*/
#include "FacePreProcess.h"
#include "base.h"
#include "FaceRecognition.h"
#include <sys/stat.h> 
CFaceRecognition::CFaceRecognition()
{}
CFaceRecognition::~CFaceRecognition()
{}
//load pca model and lda model
bool CFaceRecognition::Load(String filepath )
{
	//load pca model
	CPca pca;
	for(unsigned int i=0; i<296; i++ )
	{
		char pcafile[260] = {0};
		sprintf(pcafile,"%s/pca296/pca%d",filepath.c_str(),i);
		pca.Load(pcafile);
		vecList.push_back(pca.eVecs);
	}
	//load lda model
	char ldafile[260] = {0};
	sprintf(ldafile, "%s/lda", filepath.c_str());
	clda.Load(ldafile);
	
	return true;
}
//compute the feature that going to do pca training
bool CFaceRecognition::RecognitionPCATrain(Mat &srcmat, CFaceRect &facerect, vector<Mat> &dstmat)	
{
	Mat midmat;
	Point2f ptview[3];

	//pre processing
	ptview[0].x = (float)facerect.rclefteye.x/* - srcrect.x*/;
	ptview[0].y = (float)facerect.rclefteye.y/* - srcrect.y*/;
	ptview[1].x = (float)facerect.rcrighteye.x/* - srcrect.x*/;
	ptview[1].y = (float)facerect.rcrighteye.y/* - srcrect.y*/;
	
	//do pre process
	//1.BGR2GRAY 2.OrientationCorrect 3.FaceMask
	CFacePreProc facePreProc;
	facePreProc.ProProcFace(srcmat, ptview, midmat, true);

	//sift
	CSift siftC;
	siftC.Project(midmat, dstmat);

	//lbp
	CLbp lbp;
	lbp.Project(midmat, dstmat);


	return true;
}

//compute the feature that going to do lda training
bool CFaceRecognition::RecognitionLDATrain(Mat &srcmat, CFaceRect &facerect, vector<Mat> &dstmat)	
{
	Mat midmat;
	Point2f ptview[3];


	//pre processing
	ptview[0].x = (float)facerect.rclefteye.x/* - srcrect.x*/;
	ptview[0].y = (float)facerect.rclefteye.y/* - srcrect.y*/;
	ptview[1].x = (float)facerect.rcrighteye.x/* - srcrect.x*/;
	ptview[1].y = (float)facerect.rcrighteye.y/* - srcrect.y*/;

	//do pre process
	//1.BGR2GRAY 2.OrientationCorrect 3.FaceMask
	CFacePreProc facePreProc;
	facePreProc.ProProcFace(srcmat, ptview, midmat, true);

	vector<Mat> lbpmat;
	//sift
	CSift siftC;
	siftC.Project(midmat, lbpmat);
	//lbp
	CLbp lbp;
	lbp.Project(midmat, lbpmat);


	//pca
	vector<Mat> pcaList;
	for(unsigned int i=0; i<296; i++ )
	{
		
		Mat mImg;
		int keep = vecList[i].cols();
		mImg = Mat(1, int(keep), CV_32FC1);
		Eigen::Map<const Eigen::MatrixXf> inMap(lbpmat[i].ptr<float>(), lbpmat[i].rows*lbpmat[i].cols, 1);

		Eigen::Map<Eigen::MatrixXf> outMap(mImg.ptr<float>(), 1, (int)keep);
		
		outMap = inMap.transpose()* vecList[i];
		pcaList.push_back(mImg);
	}

	//normalize
	vector<Mat> normList;
	CNormalize normalize(CNumberAlgorithm::L2);
	normalize.Normalize(pcaList, normList);

	//cat
	vector<Mat> catList;
	CRegionsAlgorithm::Cat(normList,dstmat);

	return true;
}
//compute the feature that going to compare
bool CFaceRecognition::Recognition(char* dir, char* name, Mat &srcmat, CFaceRect &facerect, vector<Mat> &dstmat)	
{
	Mat midmat;
	Point2f ptview[3];


	//pre processing
	ptview[0].x = (float)facerect.rclefteye.x/* - srcrect.x*/;
	ptview[0].y = (float)facerect.rclefteye.y/* - srcrect.y*/;
	ptview[1].x = (float)facerect.rcrighteye.x/* - srcrect.x*/;
	ptview[1].y = (float)facerect.rcrighteye.y/* - srcrect.y*/;

	//do pre process
	//1.BGR2GRAY 2.OrientationCorrect 3.FaceMask
	CFacePreProc facePreProc;
	facePreProc.ProProcFace(srcmat, ptview, midmat, true);
  
	char dstname[200];
	sprintf(dstname, "%s%s", "NormalSet/", dir);
	bool bCreate = mkdir(dstname,0755);
	strcat(dstname,name);
	imwrite(dstname, midmat);


}
//compute the feature that going to compare
bool CFaceRecognition::Recognition( Mat &srcmat, CFaceRect &facerect, vector<Mat> &dstmat)	
{
	Mat midmat;
	Point2f ptview[3];


	//pre processing
	ptview[0].x = (float)facerect.rclefteye.x/* - srcrect.x*/;
	ptview[0].y = (float)facerect.rclefteye.y/* - srcrect.y*/;
	ptview[1].x = (float)facerect.rcrighteye.x/* - srcrect.x*/;
	ptview[1].y = (float)facerect.rcrighteye.y/* - srcrect.y*/;

	//do pre process
	//1.BGR2GRAY 2.OrientationCorrect 3.FaceMask
	CFacePreProc facePreProc;
	facePreProc.ProProcFace(srcmat, ptview, midmat, true);


	vector<Mat> lbpmat;
	//sift
	CSift siftC;
	siftC.Project(midmat, lbpmat);
	//for(unsigned int i=0;i<lbpmat.size();i++)
	//	cout<<"lbp["<<i<<"="<<lbpmat[i]<<endl;
	//lbp
	CLbp lbp;
	lbp.Project(midmat, lbpmat);

	//pca
	vector<Mat> pcaList;
	Mat mImg;
	for(unsigned int i=0; i<296; i++ )
	{
		
		int keep = vecList[i].cols();
		mImg = Mat(1, int(keep), CV_32FC1);
		Eigen::Map<const Eigen::MatrixXf> inMap(lbpmat[i].ptr<float>(), lbpmat[i].rows*lbpmat[i].cols, 1);

		Eigen::Map<Eigen::MatrixXf> outMap(mImg.ptr<float>(), 1, (int)keep);
		
		//Eigen::MatrixXf inTrans = inMap.transpose();
		//Eigen::MatrixXf vec = vecList[i];
		//cout<<"keep="<<keep<<" in="<<inTrans.rows()<<"*"<<inTrans.cols()<<endl<<inTrans<<endl;
		//cout<<"eVecs="<<vec.rows()<<"*"<<vec.cols()<<endl<<vec<<endl;
		//cout<<"inMap.trans="<<inMap.transpose()<<endl;

		outMap = inMap.transpose()* vecList[i];
		pcaList.push_back(mImg);
	}

	//normalize
	vector<Mat> normList;
	CNormalize normalize(CNumberAlgorithm::L2);
	normalize.Normalize(pcaList, normList);
	//cout<<"normList="<<normList[0]<<endl;

	//cat
	vector<Mat> catList;
	CRegionsAlgorithm::Cat(normList,catList);

	//lda
	vector<Mat> ldaList;
	clda.Project(catList, ldaList);
	
	//normalize
	vector<Mat> normList2;
	CNormalize normalize2(CNumberAlgorithm::L2) ;
	//CNormalize normalize2(CNumberAlgorithm::L1) ;
	normalize2.Normalize(ldaList, normList2);
	//cout<<"norm="<<normList2[0]<<endl;

	//quantize
	CQuantize quantize;
	vector<Mat> quanList;
	quantize.Project(normList2, quanList);
	//cout<<"quan="<<quanList[0]<<endl;

	//dstmat = quanList;
	dstmat = normList2;
	
	return true;
}
