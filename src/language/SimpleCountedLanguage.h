#ifndef SIMPLECOUNTEDLANGUAGE_H_
#define SIMPLECOUNTEDLANGUAGE_H_

#include "Language.h"

#include <map>

namespace cnf
{

class SimpleCountedLanguage : public cnf::Language
{
	
private: 

	std::map<int,int> elements;
	
protected:

	/*
	 * Updates the counter for the given element (adds one).
	 * Returns:
	 * 	true 	if the element is new
	 *  false 	if the element was already present
	 */
	virtual inline bool updateElement(int el) {
		std::map<int,int>::iterator record = elements.find(el);
		if (record == elements.end()) { // Element is new
			elements.insert(std::make_pair(el,1));
			return true;			
		} else { // Elements already exists
			record->second++;
			return false;
		}
	}
	
public:
	SimpleCountedLanguage() {}
	virtual ~SimpleCountedLanguage() {}
	
	virtual bool add(const std::vector<int>& example, int ex_id);
	
	virtual inline unsigned int getSize() const { return static_cast<int>(elements.size()); } 
	
	virtual void getElements(std::vector<int>& elements) const;
	
	virtual inline int getOccurence(int element) const {
		std::map<int,int>::const_iterator record = elements.find(element);
		if (record == elements.end()) { // Element not found
			return 0;			
		} else { // Elements exists
			return record->second;
		}	
	}
	
	
};

}

#endif /*SIMPLECOUNTEDLANGUAGE_H_*/
