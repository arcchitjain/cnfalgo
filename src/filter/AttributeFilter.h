#ifndef ATTRIBUTEFILTER_H_
#define ATTRIBUTEFILTER_H_

#include "Filter.h"
#include "../lib/defaults.h"

namespace cnf
{

/*
 * This filter filters a given list of extensions by dividing the 
 *  different values in value classes.
 * The values within such a class are mutually exclusive.
 * This is useful when, for instance, working with data with attributes that can take
 *  different values (hence the name).
 * To explain the working of this filter, note that when values a and b are mutually 
 *  exclusive, then the rule -a or -b is true.
 * Thus it is not useful to add -a when -b is already in the rule or vice versa.
 * It is also not possible to add -a when b is present (or to add b when -a is present)
 *  because this will lead to the rule (if a then b) which is clearly not true, since a
 *  and b are mutually exclusive.
 * 
 * The filter follows this procedure:
 * 	- determine the classes currently present in the rule and indicate them as positive 
 *  	or negative. A class is positive if a positive element of that class is present, 
 * 		negative if a negative element of that class is present.
 *  - for each extension:
 * 		if it is part of a negative class: remove it from the list of extensions
 * 		if it is part of a positive class: remove it if it is negative
 * 
 * Classes are identified by dividing the value by a fixed constant (i.e. 1000).
 * Elements in class 0, i.e., values smaller than 1000, are NOT filtered.
 * 
 */
class AttributeFilter : public virtual cnf::Filter
{

protected:

	const int modn;

	void getClasses(const std::vector<int>& rule, std::vector<int>& classes) const ;
	
	inline int getClass(int value) const {
		if (abs(value) > BINARY_START || abs(value) < ATTR_WIDTH) {
			return 0;
		} else {
			return  (value/modn) + (value > 0 ? 1 : -1);
		}
	}
	
	inline int getModN() const { return modn; }	
	
public:
	AttributeFilter() : modn(CLASS_WIDTH) {}
	virtual ~AttributeFilter() {}

	virtual void filterExtensions(const std::vector<int>& rule, const std::vector<int>& example, std::vector<int>& extension) const;
	
};

}

#endif /*ATTRIBUTEFILTER_H_*/
