#include "misc.h"

map<int,int> sum_class(const vector<int> &classlist)
{
	map<int,int> m;

	for(int i = 0;i < (int)classlist.size();i++)
	{
		int n = classlist[i];

		map<int,int>::iterator it = m.find(n);
		if(it == m.end())
		{
			m[n] = 1;
		}
		else
		{
			it->second++;
		}
	}

	return m;
}