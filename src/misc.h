#ifndef misc_h__
#define misc_h__

#include "Unified.h"

template <typename T>
vector<T> CumSum(const vector<T> &vals)
{
	vector<T> cumsum;
	cumsum.reserve(vals.size()+1);
	cumsum.push_back(0);
	for (int i = 0;i < (int)vals.size();i++)
		cumsum.push_back(cumsum[cumsum.size() - 1]+vals[i]);
	return cumsum;
}

extern map<int,int> sum_class(const vector<int> &classlist);

template <typename T>
vector<int> RandSample(int n, const vector<T> &weights, bool unique = false)
{
	static bool seeded = false;
	if (!seeded) {
		srand((unsigned int)time(NULL));
		seeded = true;
	}

	vector<T> cdf = CumSum(weights);
	for (int i=0; i<(int)cdf.size(); i++) // Normalize cdf
		cdf[i] = cdf[i] / cdf[cdf.size() - 1];

	vector<int> samples; samples.reserve(n);
	while ((int)samples.size() < n) {
		T r = (T)rand() / (T)RAND_MAX;
		for (int j=0; j<(int)weights.size(); j++) {
			if ((r >= cdf[j]) && (r <= cdf[j+1])) {
				if (!unique || find(samples.begin(),samples.end(),j) == samples.end())
					samples.push_back(j);
				break;
			}
		}
	}

	return samples;
}

#endif // misc_h__
