#include "SimpleCountedLanguage.h"

using namespace cnf;

void SimpleCountedLanguage::getElements(std::vector<int>& el) const {
	for (std::map<int,int>::const_iterator iter = elements.begin(); iter != elements.end(); iter++) {
		el.push_back(iter->first);
	} 
}

bool SimpleCountedLanguage::add(const std::vector<int>& example, int ex_id) {
	bool result=false;
	for (unsigned int i=0; i<example.size(); i++) {
		if (updateElement(example[i])) {
			result = true;
			notifyListenersAdd(example[i],ex_id);
		}
	}
	return result;
}

