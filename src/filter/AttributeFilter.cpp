#include "AttributeFilter.h"

using namespace cnf;

void AttributeFilter::filterExtensions(const std::vector<int>& rule, const std::vector<int>& example, std::vector<int>& extension) const {
	
	std::vector<int> classes;
	getClasses(rule,classes);


	std::vector<int>::iterator e = extension.begin();
	std::vector<int>::iterator c = classes.begin();
	
	
	while(e != extension.end()) {
		if (c == classes.end()) {
			// invalid class -> keep extension
			e++;
		} else {
			int ec = abs(getClass(*e)); // get class of current extension
			
			if (ec == 0) {  // keep extension
				e++;
			} else if (ec == abs(*c)) { // example class == current class
				if (*c < 0) { // negative class -> remove ext
					e = extension.erase(e);
				} else {
					if (*e < 0) {
						e = extension.erase(e);
					} else {
						e++;
					}
				}
			} else if (ec > abs(*c)) {
				c++;
			} else {
				e++;
			}
		}
	}
}

void AttributeFilter::getClasses(const std::vector<int>& rule, std::vector<int>& classes) const {
	int current_class = -1;
	for (unsigned int i=0; i<rule.size(); i++) {
		int new_class = getClass(rule[i]);
		if (new_class != 0 && current_class != abs(new_class)) {
			classes.push_back(new_class);
			current_class = abs(new_class);
		}
	}
}
