#include "SimpleLanguage.h"

void cnf::SimpleLanguage::getElements(std::vector<int>& el) const {
	el.reserve(elements.size());
	
	for (std::set<int>::iterator iter = elements.begin(); iter != elements.end(); iter++) {
		el.push_back(*iter);
	} 
}

bool cnf::SimpleLanguage::add(const std::vector<int>& example, int ex_id) {
	bool result = false;
	
	for (unsigned int i=0; i < example.size(); i++) {
		std::pair<std::set<int>::iterator,bool> r;
		r = elements.insert(example[i]);
		if (r.second) {
			result = true;
			notifyListenersAdd(example[i], ex_id);	
		}
	}
	return result;
}
