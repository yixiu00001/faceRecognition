/*=============================================================================
*
* Author: yixiu - yixiu@inspur.com
*
*include "FaceCompare.h"
*
* Last modified: 2015-12-11 10:32
*
* Filename: FaceCompare.cpp
*
* Description: compare the similar of two images
*
=============================================================================*/
#include<math.h>
#include<stdlib.h>
#include "Serialization.h"
#include "FaceCompare.h"

template <typename T>
double Mean(const vector<T> &vals)
{
    if (vals.empty()) return 0;
    double sum = 0;
	for(int i = 0;i < (int)vals.size();i++)
		sum += vals[i];
    return sum / vals.size();
}

/*!
 * \brief Returns the mean and standard deviation of a vector of values.
 */
template <typename T>
void MeanStdDev(const vector<T> &vals, double *mean, double *stddev)
{
    *mean = Mean(vals);
    if (vals.empty()) {
        *stddev = 0;
        return;
    }

    double variance = 0;
    for(int i = 0;i < (int)vals.size();i++) {
        const double delta = vals[i] - *mean;
        variance += delta * delta;
    }
    *stddev = sqrt(variance/vals.size());
}

template <typename T>
double KernelDensityBandwidth(const vector<T> &vals)
{
    double mean, stddev;
    MeanStdDev(vals, &mean, &stddev);
    return pow(4 * pow(stddev, 5.0) / (3 * vals.size()), 0.2);
}

/*!
 * \brief Compute kernel density at value x with bandwidth h.
 */
template <typename T>
double KernelDensityEstimation(const vector<T> &vals, double x, double h)
{
    double y = 0;
    for(int i = 0;i < (int)vals.size();i++)
        y += exp(-pow((vals[i]-x)/h,2)/2)/sqrt(2*3.1415926353898);
    return y / (vals.size() * h);
}

template <typename T>
void MinMax(const vector<T> &vals, T *min, T *max, int *min_index, int *max_index)
{
	const int size = (int)vals.size();
	assert(size > 0);

	*min = *max = vals[0];
	*min_index = *max_index = 0;
	for (int i=1; i<size; i++) {
		const T val = vals[i];
		if (val < *min) {
			*min = val;
			*min_index = i;
		} else if (val > *max) {
			*max = val;
			*max_index = i;
		}
	}
}

template <typename T>
void MinMax(const vector<T> &vals, T *min, T *max)
{
	int min_index, max_index;
	MinMax(vals, min, max, &min_index, &max_index);
}

struct KDE
{
	float min, max;
	double mean, stddev;
	vector<float> bins;

	KDE() : min(0), max(1), mean(0), stddev(1) {}
	KDE(const vector<float> &scores)
	{
		MinMax(scores, &min, &max);
		MeanStdDev(scores, &mean, &stddev);
		double h = KernelDensityBandwidth(scores);
		const int size = 255;
		bins.reserve(size);
		for (int i=0; i<size; i++)
			bins.push_back((float)KernelDensityEstimation(scores, min + (max-min)*i/(size-1), h));
	}

	float operator()(float score, bool gaussian = true) const
	{
		if (gaussian)
		{
			//cout<<"stddev="<<stddev<<" cvpi="<<CV_PI<<" mean="<<mean<<" score="<<score<<" res="<<(float)(1/(stddev*sqrt(2*CV_PI))*exp(-0.5*pow((score-mean)/stddev, 2)))<<endl;
			
			return (float)(1/(stddev*sqrt(2*CV_PI))*exp(-0.5*pow((score-mean)/stddev, 2)));
		}
		if (score <= min) 
		{
			return bins[0];
		}
		if (score >= max) 
		{
			return bins[bins.size() - 1];
		}
		const float x = (score-min)/(max-min)*bins.size();
		const float y1 = bins[(int)floor(x)];
		const float y2 = bins[(int)ceil(x)];
		//cout<<"x="<<x<<" y1="<<y1<<" y2="<<y2<<"res="<<y1 + (y2-y1)*(x-floor(x))<<endl;
		return y1 + (y2-y1)*(x-floor(x));
	}
};

struct MP
{
	KDE genuine, impostor;
	MP() {}
	MP(const vector<float> &genuineScores, const vector<float> &impostorScores)
		: genuine(genuineScores), impostor(impostorScores) {}
	float operator()(float score, bool gaussian = true) const
	{
		const float g = genuine(score, gaussian);
		const float s = g / (impostor(score, gaussian) + g);
		return s;
	}
};
inline double l1(Mat &a, Mat &b, int size)
{
	double distance = 0;
	int rows = a.rows;
	int cols = b.cols;
	for(int i=0;i<rows;i++)
	{
		float *pa= a.ptr<float>(i);
		float *pb= b.ptr<float>(i);
		for(int j=0;j<cols;j++)
		{
			distance +=abs(pa[j]-pb[j]);
			//if((pa[j]>0 && pb[j]<0)||(pa[j]<0&&pb[j]>0))
			//	distance += abs(pa[j]-pb[j])*2;
			//else
			//	distance +=abs(pa[j]-pb[j]);
		}
	}
	return distance;
}
inline float l1(const uchar *a, const uchar *b, int size)
{
	int distance = 0;
	for (int i=0; i<size; i++)
		distance += abs(a[i]-b[i]);
	return (float)distance;
}
CDataStream &operator>>(CDataStream &stream,vector<float> &data)
{
	int size = 0;

	stream>>size;

	for(int i = 0;i < size;i++)
	{
		float v;

		stream >> v;
		data.push_back(v);
	}

	return stream;
}

CDataStream &operator<<(CDataStream &stream,const vector<float> &data)
{
	int size = data.size();
	stream<<size;

	for(int i = 0;i < size;i++)
	{
		stream << data[i];
	}

	return stream;
}
CDataStream &operator>>(CDataStream &stream, KDE &kde)
{
	return stream >> kde.min >> kde.max >> kde.mean >> kde.stddev >> kde.bins;
}

CDataStream &operator<<(CDataStream &stream, const KDE &kde)
{
	return stream << kde.min << kde.max << kde.mean << kde.stddev << kde.bins;
}

CDataStream &operator>>(CDataStream &stream, MP &nmp)
{
	return stream >> nmp.genuine >> nmp.impostor;
}

CDataStream &operator<<(CDataStream &stream, const MP &nmp)
{
	return stream << nmp.genuine << nmp.impostor;
}

class CMatchProbabilityDistance
{
private:

	MP mp;

	bool gaussian;

public:
	CMatchProbabilityDistance():gaussian(true)
	{}
	~CMatchProbabilityDistance()
	{}

	bool Load(CDataStream &stream)
	{
		stream >> mp;
		//cout<<"mean="<<mp.genuine.mean<<" stddev="<<mp.genuine.stddev<<endl;
		//cout<<"mean="<<mp.impostor.mean<<" stddev="<<mp.impostor.stddev<<endl;
		return true;
	}
	bool Load(const string& filepath)
	{
		char simfile[260]={0};
		sprintf(simfile, "%s/similar", filepath.c_str());
		CFileDataStream stream; 
		if(!stream.load(simfile)) 
		{
			return false;
		}
		return Load(stream);
	}

	bool Save(CDataStream &stream)
	{
		stream << mp;
		return true;
	}
	bool Save(const string& filepath)
	{
		CDataStream stream;
		if(!stream.load(filepath, true)) 
		{
			//cout<<"load file failed!"<<filepath<<endl;
		}
		Save(stream);
		return true;
	}	
	bool Train(vector<vector<cv::Mat> > &matlist,const vector<int> &classlist)
	{
		float *scoretable = new float[matlist[0].size() * matlist[0].size()];

		for(unsigned int i = 0;i < matlist[0].size();i++)
		{
			//cout<<i<<"="<<matlist[0][i]<<endl;
			for(unsigned int j = 0;j < matlist[0].size();j++)
			{
				*(scoretable + (matlist[0].size() * i) + j) = l1(matlist[0][j],matlist[0][i],matlist[0][i].total());
				//*(scoretable + (matlist[0].size() * i) + j) = l1(matlist[0][j].data,matlist[0][i].data,matlist[0][i].total());
			}
		}

		vector<float> genuineScores, impostorScores;
		genuineScores.reserve(classlist.size());
		impostorScores.reserve(classlist.size()*classlist.size());
		for (unsigned int i=0; i<matlist[0].size(); i++) {
			for (unsigned int j=0; j<i; j++) {
				const float score = *(scoretable + (matlist[0].size() * i) + j);
				if (score == -std::numeric_limits<float>::max()) continue;
				if (classlist[i] == classlist[j]) genuineScores.push_back(score);
				else impostorScores.push_back(score);
			}
		}

		delete []scoretable;

		mp = MP(genuineScores, impostorScores);

		const string filepath="model/similar";
		Save(filepath);

		return true;
	}

	float Compare( cv::Mat &targetmat,cv::Mat &querymat )
	{
		//cout<<"tar="<<targetmat<<endl;
		//cout<<"que="<<querymat<<endl;
		const float rawScore = l1(targetmat, querymat,querymat.total());
		//cout<<"rawScore="<<rawScore<<endl;
		//cout<<"lim="<<-std::numeric_limits<float>::max()<<endl;
		if(rawScore==0) return 1;
		if (rawScore == -std::numeric_limits<float>::max()) return rawScore;
		return mp(rawScore, gaussian);
	}

};
bool CFaceCompare::Load(const char * filePath)
{
	CMatchProbabilityDistance distance;
	distance.Load(filePath);	
	return true;
	
}

//real compare
bool CFaceCompare::Compare(cv::Mat &targetmat, cv::Mat &querymat, float &dist)
{
	//int size = querymat.total();
	//const unsigned char *tar = targetmat.data;
	//const unsigned char *que = querymat.data;

	CMatchProbabilityDistance distance;
	distance.Load("model");	
	//distance.Load("model/similar");	
	dist = distance.Compare(targetmat,querymat);

	return true;

}
//train the compare dataset
bool CFaceCompare::Train(vector<vector<cv::Mat> >&matlist, const vector<int> &classlist)
{
	CMatchProbabilityDistance distance;
	distance.Train(matlist, classlist);
	cout<<"=====train end====="<<endl;

	return true;
}
