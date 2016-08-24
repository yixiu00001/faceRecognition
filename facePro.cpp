/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*
* Last modified: 2015-11-15 09:12
*
* Filename: facePro.cpp
*
* Description:the main function to compute the face image features
*
=============================================================================*/
#include <stdarg.h>
#include<iostream>
#include<fstream>
#include <iterator>  

#include<opencv2/opencv.hpp>
#include "src/FaceDetect.h"
#include "src/FaceRecognition.h"
#include "src/pca.h"
#include "src/lda.h"
#include "src/FaceCompare.h"
#include "src/base.h"
#include "src/cvtool.h"
#include "src/FaceToDemo.h"

#define demoflag 3

using namespace std;
//#define PRINT	
int readFile(const char*path, vector<string>&namelist)
{
	char filename[200];
	char tmp[200];
	FILE *fp;
	if((fp=fopen(path, "r"))==NULL)
	{
		cout<<"open failed! "<<path<<endl;
		return -1;
	}	
	while(!feof(fp))
	{
		filename[0]='\0';
		tmp[0]='\0';
		if(fgets(filename,200,fp)!=NULL)
		{
			int len = strlen(filename);
			strncpy(tmp, filename, len);
			tmp[len-1]='\0';
			string name(tmp);
			namelist.push_back(name);
		}
			
	}
	fclose(fp);
	return 0;
}
//feature compute
int listfeatureCompute(const char* path)
{
	CFaceDetect faceDetect;
	vector<CFaceRect> srcRect;
	if(!faceDetect.Init("./model",1))
		return -1;
	CFaceRecognition faceRecognition;
	faceRecognition.Load("./model");
	vector<Mat> dstImgList;


	vector<string> namelist;
	readFile(path, namelist);
	char name[200];
	char split='/';
	for(unsigned int i=0;i<namelist.size();i++)
	{
		memset(name, 0 , 200);
		Mat srcImg;
		strncpy(name, namelist[i].c_str(),namelist[i].length());
		srcImg = imread(name);
		faceDetect.Detect(srcImg,srcRect,true);
		if(srcRect.empty())
		{
			cout<<"No face found !!" <<endl;
			return -1;
		}
		cout<<"=====name="<<name<<endl;
		int start=0;
		int tail = 0;
		int res =0;
		res = namelist[i].find_first_of(split, start);
		start = res;
		res = namelist[i].find_first_of(split, start+1);
		start = res;
		res = namelist[i].find_first_of(split, start+1);
		tail = res;
		res = namelist[i].find_first_of(split, tail+1);

		char dir[200]={'\0'};
		char dstname[200]={'\0'};
		strncpy(dir ,namelist[i].substr(start+1,tail - start).c_str(), tail -start);
		strncpy(dstname ,namelist[i].substr(tail+1,namelist[i].size() - tail).c_str(), namelist[i].size() -tail);
		//cout<<namelist[i].substr(res+1,namelist[i].size()-res).c_str()<<endl;
		faceRecognition.Recognition(dir, dstname, srcImg, srcRect[0],dstImgList );
		
	}

	return 0;	
}
//compute one image's feature
int   faceCompute(const char* path, const int trace, vector<Mat> &dstImgList)
{

	Mat srcImg;
	srcImg = imread(path);

	if(srcImg.empty())
	{
		cout<<"imread srcImg is failed!"<<endl;
		return -1;
	}
	if (trace < 0 || trace > 1)
	{
		cout<<"Usage: facePro IMAGE TRACE, with TRACE 0 or 1"<<endl;
		return -1;
	}

	//face detect ,multiple face
	CFaceDetect faceDetect;
	vector<CFaceRect> srcRect;
	if(!faceDetect.Init("./model",1))
		return -1;
	faceDetect.Detect(srcImg,srcRect,true);
	if(srcRect.empty())
	{
		cout<<"No face found !!" <<endl;
		return -1;
	}

	//face recognition
	CFaceRecognition faceRecognition;
	faceRecognition.Load("./model");
	cout<<"reg"<<endl;
	faceRecognition.Recognition(srcImg, srcRect[0],dstImgList );

	//char buff[4000];
	//int res;
	//res = MatToChar(dstImgList[0], buff);
	//if(res != 3616)
	//	return -1;
	//cout<<"feature="<<dstImgList[0]<<endl;
	return 0;

}
//comptute the similar of two images
int faceSimilar(const char* imagePath1, const char* imagePath2)
{
	Mat srcImg1, srcImg2;
	srcImg1 = imread(imagePath1);
	srcImg2 = imread(imagePath2);

	if(srcImg1.empty() || srcImg2.empty())
	{
		cout<<imagePath1<<" or "<<imagePath2<<"Imread image failed!"<<endl;
		return -1;
	}
	//face detect
	CFaceDetect faceDetect;
	vector<CFaceRect> srcRect1;
	vector<CFaceRect> srcRect2;
	if(!faceDetect.Init("./model", 0))
	{
		cout<<"---FaceDetect init model failed!---"<<endl;
		return -1;
	}
	faceDetect.Detect(srcImg1, srcRect1, true);
	faceDetect.Detect(srcImg2, srcRect2, true);

	bool empty1 = false;
	bool empty2 = false;
	empty1 = srcRect1.empty();
	empty2 = srcRect2.empty();

	if(empty1||empty2)
	{
		if(empty1)
			cout<<imagePath1<<" ---no face  found!---"<<endl;
		if(empty2)
			cout<<imagePath2<<" ---no face  found!---"<<endl;

		return -1;
	}

	//face recognition
	vector<Mat> dstImgList1;
	vector<Mat> dstImgList2;
	CFaceRecognition faceRecognition;
	faceRecognition.Load("./model");

	faceRecognition.Recognition(srcImg1, srcRect1[0], dstImgList1 );
	faceRecognition.Recognition(srcImg2, srcRect2[0], dstImgList2 );
	if(dstImgList1.size()==0 || dstImgList2.size()==0)
	{
		cout<<"---Compute feature failed!---"<<endl;
		return -1;
	}
		
	char buff1[4000];
	char buff2[4000];
	int res;
	res = MatToChar(dstImgList1[0], buff1);
	res = MatToChar(dstImgList2[0], buff2);
	Mat m1, m2;
	m1 = CharToMat(buff1);
	m2 = CharToMat(buff2);
	//face compare
	CFaceCompare faceCompare;
	float dist = 0;
	bool flag;
	flag = faceCompare.Compare(dstImgList1[0], dstImgList2[0], dist);
	//flag = faceCompare.Compare(m1, m2, dist);
	if(!flag)
	{
		cout<<"---Compare feature failed!---"<<endl;
		return -1;
	}
	//cout<<dstImgList1[0]<<endl;
	//cout<<dstImgList2[0]<<endl;
	cout<<imagePath1<<" [vs] "<<imagePath2<<"  =  "<<dist<<endl;

	return 0;
}
//do pca train
int pcaTrain(const char* path, const int trace)
{
	//read the imageName of list
	vector<string> namelist;
	readFile(path, namelist);
	//=========================================================================
	char name[200];
	vector<vector<Mat> > featureList;//(1000,vector<Mat>(296));
	vector<Mat> dstImgList;
	//deal each image 
	for (unsigned int i =0; i < namelist.size(); i++) 
	{
		memset(name, 0 , 200);
		Mat srcImg;
		strncpy(name, namelist[i].c_str(),namelist[i].length());
		srcImg = imread(name);
		cout<<"index="<<i<<"|"<<name<<endl;
		if(srcImg.empty())
		{
			cout<<"imread srcImg is failed!"<<endl;
			return -1;
		}
		dstImgList.clear();
		//compute the feature

		CFaceDetect FaceDetect;
		vector<CFaceRect> srcRect;
		if(!FaceDetect.Init("./model",1))
			return -1;
		FaceDetect.Detect(srcImg,srcRect,true);
		if(srcRect.empty())
		{
			cout<<"No face found !!" <<endl;
			continue;
		}
		//face recognition
		CFaceRecognition faceRecognition;
		faceRecognition.Load("./model");
		faceRecognition.RecognitionPCATrain(srcImg, srcRect[0],dstImgList );
		if(dstImgList.size()>0)
		{
			featureList.push_back(dstImgList);
		}
		else
		{
			cout<<"---no face="<<name<<endl;
		}
	}
	vector<vector<Mat> >trainList(296);//(296,vector<Mat>(1000));
	for(int i=0;i<296;i++)
		trainList[i].resize(featureList.size());
	//each vector is a feature vec
	for(unsigned int i=0;i<featureList.size();i++)
	{
		for(unsigned int j=0;j<featureList[i].size();j++)
		{
			if(featureList[i][j].rows>0)
				trainList[j][i] = featureList[i][j];
			else
				cout<<"----no feature="<<i<<" "<<j<<endl;
		}
	}
	//do pca train
	CPca cpca;
	cpca.Train(trainList);
	
	return 0;
}
//do lda train
int ldaTrain(const char* path, const int trace,const char* typepath)
{
	//read image name of list
	cout<<"path="<<path<<endl<<"typepath"<<typepath<<endl;
	vector<string> namelist;
	readFile(path, namelist);

	//each type of list
	vector<string> typelist;
	readFile(typepath, typelist);
	vector<int> classlist;
	for(unsigned int i=0;i<typelist.size();i++)
	{
		classlist.push_back(atoi(typelist[i].c_str()));
	}

	char name[200];
	vector<vector<cv::Mat> > featureList;
	vector<Mat> dstImgList;
	//get the feature of each image
	for (unsigned int i =0; i < namelist.size(); i++) 
	{
		memset(name, 0 , 200);
		Mat srcImg;
		strncpy(name, namelist[i].c_str(),namelist[i].length());
		srcImg = imread(name);
		cout<<"index="<<i<<"|"<<name<<endl;
		if(srcImg.empty())
		{
			cout<<"imread srcImg is failed!"<<endl;
			return -1;
		}
		dstImgList.clear();
		//face detect
		CFaceDetect FaceDetect;
		vector<CFaceRect> srcRect;
		if(!FaceDetect.Init("./model",1))
			return -1;
		FaceDetect.Detect(srcImg,srcRect,true);
		if(srcRect.empty())
		{
			cout<<"No face found !!" <<endl;
			continue;
		}
		//face recognition
		CFaceRecognition faceRecognition;
		faceRecognition.Load("./model");
		faceRecognition.RecognitionLDATrain(srcImg, srcRect[0],dstImgList );
		if(dstImgList.size()>0)
		{
			featureList.push_back(dstImgList);
		}
		else
		{
			cout<<"---no face="<<name<<endl;
		}
	}
	vector<vector<cv::Mat> >trainList(1);//(296,vector<Mat>(1000));
	for(int i=0;i<1;i++)
		trainList[i].resize(featureList.size());
	//each vector is a feature vec
	for(unsigned int i=0;i<featureList.size();i++)
	{
		for(unsigned int j=0;j<featureList[i].size();j++)
		{
			if(featureList[i][j].rows>0)
				trainList[j][i] = featureList[i][j];
			else
				cout<<"----no feature="<<i<<" "<<j<<endl;
		}
	}
	//do pca train
	
	CLda clda;
	clda.Train(trainList, classlist);


	return 0;
}
//save the mat 
int saveMat(Mat &srcmat, const string fileName)
{
	ofstream outFile(fileName.c_str(), ios_base::out); 
	if(!outFile.is_open())
	{
		return -1;
	}
	int rows = srcmat.rows;
	int cols = srcmat.cols;

	//outFile<<rows<<"\t"<<cols<<"\t"<<channels<<"\t";
	for(int i=0;i<rows;i++)
	{
		float *p = srcmat.ptr<float>(i);
		for(int j=0;j<cols;j++)
		{
			outFile<<p[j]<<"\t";
		}
	}
	outFile<<endl;

	return 0;
}
//load the mat
int loadMat(Mat &dstmat, const string filename)
{
	ifstream inFile(filename.c_str(), ios_base::in);
	if(!inFile.is_open())
	{
		cout<<filename<<" is not open!"<<endl;
		return -1;
	}

	istream_iterator<float> begin(inFile);
	istream_iterator<float> end;
	vector<float> inData(begin,end);  
	cv::Mat tmpMat = cv::Mat(inData);  
	int matRows = 1;
	int matChns = 1;
	size_t dataLength = inData.size();  
	int matCols = dataLength / matChns / matRows;  
	if (dataLength != (matRows * matCols * matChns))  
	{
		matChns = 1; 
		matRows = 1;  
	}
	
	dstmat = tmpMat.reshape(matChns, matRows).clone();  
	//cout<<"============================="<<endl;
	//cout<<dstmat.data<<endl;
	/*
	for(int i=0;i<matRows;i++)
	{
		float *p = dstmat.ptr<float>(i);
		for(int j=0;j<matCols;j++)
		{
			cout<<p[j]<<"\t";
		}
	}
	cout<<endl;
	*/
	return 0;

}
//similar train
int similarTrain(const char* path, const int trace,const char* typepath)
{
	int writeFlag = 0;
	//read image name of list
	vector<string> namelist;
	readFile(path, namelist);

	//each type of list
	vector<string> typelist;
	readFile(typepath, typelist);
	vector<int> classlist;
	for(unsigned int i=0;i<typelist.size();i++)
	{
		classlist.push_back(atoi(typelist[i].c_str()));
	}

	char name[200];
	Mat srcImg;
	vector<vector<cv::Mat> > featureList;
	vector<Mat> dstImgList;
	//get the feature of each image
	for (unsigned int i =0; i < namelist.size(); i++) 
	{
			if(writeFlag==0)
			{
			memset(name, 0 , 200);
			strncpy(name, namelist[i].c_str(),namelist[i].length());
			cout<<i<<" "<<name<<endl;
			srcImg = imread(name);
			if(srcImg.empty())
			{
				cout<<"imread srcImg is failed!"<<endl;
				return -1;
			}
			dstImgList.clear();

			//face detect
			CFaceDetect FaceDetect;
			vector<CFaceRect> srcRect;
			if(!FaceDetect.Init("./model",1))
				return -1;
			FaceDetect.Detect(srcImg,srcRect,true);
			if(srcRect.empty())
			{
				cout<<"No face found !!" <<endl;
				continue;
			}

			//face recognition
			CFaceRecognition faceRecognition;
			faceRecognition.Load("./model");
			faceRecognition.Recognition(srcImg, srcRect[0],dstImgList );
			char savename[200];
			sprintf(savename, "mid/%d", i);
			saveMat(dstImgList[0], savename);
		}
		else if(writeFlag==1)
		{
			char savename[200];
			sprintf(savename, "mid/%d", i);
			Mat tmpMat;
			vector<Mat> ldaList;
			if(loadMat(tmpMat, savename)==-1)
				continue;

			ldaList.clear();
			ldaList.push_back(tmpMat);

			CNormalize normalize2(CNumberAlgorithm::L2) ;
			dstImgList.clear();
			normalize2.Normalize(ldaList, dstImgList);
		}
		if(dstImgList.size()>0)
		{
			featureList.push_back(dstImgList);
		}
		else
		{
			cout<<"---no face="<<name<<endl;
		}
	}
	vector<vector<cv::Mat> >trainList(1);//(296,vector<Mat>(1000));
	for(int i=0;i<1;i++)
		trainList[i].resize(featureList.size());

	//each vector is a feature vec
	for(unsigned int i=0;i<featureList.size();i++)
	{
		for(unsigned int j=0;j<featureList[i].size();j++)
		{
			if(featureList[i][j].rows>0)
			{
				trainList[j][i] = featureList[i][j];
			}
			else
				cout<<"----no feature="<<i<<" "<<j<<endl;
		}
	}
	
	CFaceCompare ccompare;
	ccompare.Train(trainList, classlist);


	return 0;
}
static void markLandmarks(cv::Mat_<unsigned char> &img,float landmarks[],int nlandmarks=stasm_NLANDMARKS)
{
	for(int i=0;i<nlandmarks;i++)
	{
		//if(i==L_LPupil || i==L_RPupil ||i==L_CTopOfTopLip)
		img(cvRound(landmarks[i*2+1]),cvRound(landmarks[2*i]))=255;
	}
}
static void printLandmarks(const float* landmarks)
{
	cout<<"=======PrintLandmarks=========="<<endl;	
	for (int i = 0; i < stasm_NLANDMARKS; i++)
		stasm_printf("%3d: %4.0f %4.0f\n",
				i, landmarks[i*2], landmarks[i*2+1]);//x,y
}
void demoFaceFind(const char* filepath, const char *path, const char *path2)
{
	CFaceDemo demo;
	demo.initmodel(filepath);
	if(demoflag==1)
	{
		Mat img;
		img = imread(path);
		vector<FACEDIST_RECT> retlist;

		//bool onlybig;
		imshow("source", img);
		demo.facefindRect(img, retlist, true);
		cout<<"size="<<retlist.size()<<endl;
		FACEDIST_RECT rect = retlist[0];
		cv::rectangle(img,Point(rect.face_x, rect.face_y),Point(rect.face_x+rect.face_width, rect.face_y + rect.face_height),Scalar(255,0,0));  
		cv::imshow("true", img);

		demo.facefindRect(img, retlist, false);
		for(int i=0;i<retlist.size();i++)
		{
			FACEDIST_RECT rect = retlist[i];
			cv::rectangle(img,Point(rect.face_x, rect.face_y),Point(rect.face_x+rect.face_width, rect.face_y + rect.face_height),Scalar(255,0,0));  
		}
		cv::imshow("false", img);
		cv::waitKey(0);
	}
	else if(demoflag ==2)
	{
		Mat img,graymat;
		img = imread(path);
		cvtColor( img, graymat, CV_BGR2GRAY );
		vector<float *> vlandmarks;
		Mat_<unsigned char> out(graymat.clone());


		demo.facefindMarks(img, vlandmarks, false);
		for(unsigned int i=0;i<vlandmarks.size();i++)
		{
			printLandmarks(vlandmarks[i]);
			markLandmarks(out, vlandmarks[i]);
		}
		imshow("false", out);
		imshow("s", img);


		waitKey(0);
	}
	else if(demoflag ==3)
	{
		Mat img,img2;
		img = imread(path);
		img2 = imread(path2);
		//vector<FACEDIST_RECT> rcRectList;
		//char *retbuf = new char[200];
		//demo.facefindRect(img, rcRectList, true);
		//FACEDIST_RECT rect = rcRectList[0];
		//cv::rectangle(img,Point(rect.face_x, rect.face_y),Point(rect.face_x+rect.face_width, rect.face_y + rect.face_height),Scalar(255,0,0));  
		//cv::imshow("true", img);
		//waitKey(0);
		//demo.buildmodel(img, rect, retbuf);
		//cout<<"buf="<<retbuf<<endl;

		//char *retbuf2 = retbuf;
		//float similar=0.0;

		//demo.facecompareModel(retbuf, retbuf2, similar);
		//cout<<"similar="<<similar<<endl;

		vector<float> similarity;
		demo.facecompareImg(img, img2, false, similarity);
		for(unsigned int i=0;i<similarity.size();i++)
			cout<<i<<"="<<similarity[i]<<endl;
		
	}
	return ;

}
/*
void demoBuildModel(Mat &img,PFACEDIST_RECT &rect,char **retbuf)
{
	buildmodel(img, rect, retbuf);
}
void demoFaceCompare(char *retbuf1,char *retbuf2,float &similarity)
{
	facecompare(retbuf1, retbuf2, similar);
	cout<<"similar=%f"<<similarity<<endl;
}
void demoFaceCompare(Mat &img1,Mat &img2,bool onlybig, vector<float*> &similarity)
{
	facecompare(img1, img2, true, similarity);
}
*/
void Help(char* argv[])
{
	cout<<"|-------------------HELP-------------------------------------------------------|"<<endl;
	cout<<"|----------LDA train ["<<argv[0]<<" -d imageListPath ,typeListPath]------------|"<<endl; 
	cout<<"|----------similar train ["<<argv[0]<<" -t imageListPath ,typeListPath]--------|"<<endl; 
	cout<<"|----------PCA train ["<<argv[0]<<" -p imageListPath ]-------------------------|"<<endl; 
	cout<<"|----------compute image ["<<argv[0]<<" -c image1Path ]------------------------|"<<endl; 
	cout<<"|----------compute images ["<<argv[0]<<" -l imageListPath ]--------------------|"<<endl; 
	cout<<"|----------compute similar ["<<argv[0]<<" -s imagePath1 imagePath2]------------|"<<endl; 
	cout<<"|------------------------------------------------------------------------------|"<<endl;
}
int main(int argc, char* argv[])
{
	char ch;
	const char* imagePath;
	const char* filePath;
	vector<Mat> dstList;
	const char* imageListPath;
	const char* typeListPath;
	const char* imagePath1;
	const char* imagePath2;

	while((ch = getopt(argc, argv, "d:p:s:c:t:l:a:")) != EOF)  
	{
		switch(ch)
		{
			//similar train
			case 't':
				if(argc<4)
				{
					Help(argv);
					return -1;
				}
				imageListPath = optarg;
				typeListPath = argv[optind];
				similarTrain(imageListPath, 0, typeListPath);
				return 0;
			//lda train
			case 'd':
				if(argc<4)
				{
					Help(argv);
					return -1;
				}
				imageListPath = optarg;
				typeListPath = argv[optind];
				ldaTrain(imageListPath, 0, typeListPath);
				return 0;
			//pca train
			case 'p':
				if(argc<3)
				{
					Help(argv);
					return -1;
				}
				imageListPath = optarg;
				pcaTrain(imageListPath, 0);
				cout<<"t:"<<optarg<<endl;
				return 0;
			//two images similar
			case 's':
				if(argc<4)
				{
					Help(argv);
					return -1;
				}
				imagePath1 = optarg;
				imagePath2 = argv[optind];
				faceSimilar(imagePath1, imagePath2);
				return 0;
			//one image feature
			case 'c':
				if(argc<3)
				{
					Help(argv);
					return -1;
				}
				imagePath = optarg;
				faceCompute(imagePath, 1, dstList);
				//cout<<"feature="<<endl<<dstList[0]<<endl;
				return 0;
			case 'a':
				if(argc<4)
				{
					Help(argv);
					return -1;
				}
				filePath = optarg;
				imagePath = argv[optind];
				imagePath2 = argv[optind+1];
				cout<<"filePath="<<filePath<<" imagePath="<<imagePath<<" imgpath2="<<imagePath2<<endl;
				demoFaceFind(filePath,imagePath,imagePath2);
				return 0;

			//a list of  images feature
			case 'l':
				if(argc<3)
				{
					Help(argv);
					return -1;
				}
				imagePath = optarg;
				listfeatureCompute(imagePath );
				return 0;
			default:
				Help(argv);	
				return -1;
		}
	}
	Help(argv);	
	return -1;

}
