#include "Language.h"
#include "../lib/message.h"

using namespace cnf;

void Language::extensions(const std::vector<int>& example, std::vector<int>& extensions) const {
	
	// First read all the elements of the language into extensions.
	getElements(extensions);
	
	unsigned int exa = 0;
	unsigned int ext = 0;

	// Then switch their sign if not in example.
	while (ext < extensions.size()) {
		if (exa >= example.size()) {
			// current atom from language not in example -> make negative
			extensions[ext] = -extensions[ext];
			ext++;  
		} else if (example[exa] == extensions[ext]) {
			// current atom from language in example -> keep positive
			ext++; exa++;
		} else if (example[exa] > extensions[ext]) {
			// current atom from language not in example -> make negative
			extensions[ext] = -extensions[ext];
			ext++;
		} else {
			// current atom in example not in language (should not happen)
			print_warning("found atom that was not in language", "", __FILE__,__LINE__);
			exa++;
		}
	}
}
