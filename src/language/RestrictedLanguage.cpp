#include "RestrictedLanguage.h"

void cnf::RestrictedLanguage::extensions(const std::vector<int>& example, std::vector<int>& extensions) const {
	
	// Generates extensions for the given example E.
	// This language is based on example D.
	// The following cases exist:
	//	- if x in D and x in E 		(1)
	//		--> discard: E requires x, D requires -x
	//	- if x nin D and x nin E	(2)	(implicit)
	//		--> discard: E required -x, D requires x
	//	- if x in D and not in E	(3)
	//		--> add -x
	//  - if x not in D and in E	(4)
	//		--> add x

	unsigned int l = 0;
	unsigned int e = 0;
	while (l < getSize() || e < example.size()) {
		//if (e < example.size()) std::cout << "example[" << e << "] = " << example[e] << std::endl; 
		//std::cout << "getElement(" << l << ") = " << getElement(l) << std::endl;
		
		if (e >= example.size()) {
			// all atoms in E handled -> remaining exist only in D
			//  --> case (3)
			extensions.push_back(-getElement(l));
			l++;
		} else if (l >= getSize()) {
			// --> case (4)
			extensions.push_back(example[e]);
			e++;
 		} else if (example[e] == getElement(l)) {
			// --> case (1)
			l++; e++;
		} else if (example[e] < getElement(l)) {
			// --> case (4)
			extensions.push_back(example[e]);
			e++;
		} else { // --> case (3)
			extensions.push_back(-getElement(l));
			l++;
		}
	}	
	
//	std::cout << "Generated Extensions :" << std::endl;
//	std::cout << "  --- exa --- " << str(example) << std::endl;
//	std::cout << "  --- lan --- " << str(language_) << std::endl;
//	std::cout << "  --- ext --- " << str(extensions) << std::endl;
}
