#ifndef SIMPLELANGUAGE_H_
#define SIMPLELANGUAGE_H_

#include "Language.h"

#include <set>

namespace cnf
{

/*
 * The SimpleLanguage class provides a language collection using a set to store the different
 * literals of the language. These can be added using the add method.
*/
class SimpleLanguage : public Language
{

private:

	std::set<int> elements;
	
public:
	SimpleLanguage() {}
	virtual ~SimpleLanguage() {}
	
	/*
	 * Adds all the literals in the example to the language (provided they are not already
	 *  in the language). 
	 */
	virtual bool add(const std::vector<int>& example, int ex_id);
	
	virtual inline int unsigned getSize() const { return static_cast<int>(elements.size()); } 
	
	virtual void getElements(std::vector<int>& elements) const;
	
	virtual inline int getOccurence(int element) const {
		return (elements.find(element) != elements.end());	
	}
};

}

#endif /*SIMPLELANGUAGE_H_*/
