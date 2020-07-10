#ifndef FILTER_H_
#define FILTER_H_

#include <vector>

namespace cnf
{
/*
 * This class represents a filter for examples and extensions.
 * 
 * The default behavior for this class is to copy the input to the output without
 *  modifications.
 */ 
class Filter
{
public:
	Filter() {}
	virtual ~Filter() {}
	
	virtual void filterExtensions(const std::vector<int>& rule, const std::vector<int>& example, std::vector<int>& extension) const {}
	
};

}

#endif /*FILTER_H_*/
