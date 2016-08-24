#ifndef PAEL_TEMPLATE_HPP
#define PAEL_TEMPLATE_HPP

#include"pael.h"

class IPaelAlgorithmTemplate:public IPaelAlgorithm
{
public:
	IPaelAlgorithmTemplate()
	{}
	virtual ~IPaelAlgorithmTemplate()
	{}
	
	virtual void destroy()
	{
		if(this != 0)
		{
			delete this;
		}
	}
};

#endif