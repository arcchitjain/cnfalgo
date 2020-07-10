#ifndef EXAMPLEREADER_H_
#define EXAMPLEREADER_H_

#include "FileReader.h"
#include "../lib/defaults.h"

namespace cnf {


class ExampleReader : public cnf::FileReader {
	
	private :
	
		
		bool mvclass;
		
		int bfilter;
	
	
		virtual inline void moveclass(std::vector<int>& line) {
			if (mvclass && line.size() > 0) {
				int classlabel = line.front();	// read first element
				line.erase(line.begin());	// remove first element
				line.push_back(CLASS_WIDTH*CLASS_OFFSET + classlabel + 1); // reinsert class label
			}	
		}
	
	
		virtual inline void filter(std::vector<int>& line) {
			std::vector<int>::iterator i = line.begin();
			if (i != line.end()) {
				++(*i);
				++i;	// keep class = first element (add 1 to avoid 0)
			}
			while (i != line.end()) {
				if (*i < bfilter) {
					int val = (*i);
					i = line.erase(i);
					if(val % 2 == 1) line.push_back(val +  + BINARY_START);
				} else {
					++i;
				}
			}
		}
	
	public :
	
		ExampleReader(std::string filename, int binfilter=CLASS_WIDTH, bool moveclass=true) : 
			FileReader(filename), mvclass(moveclass), bfilter(binfilter) {}
	
		virtual bool readline(std::vector<int>& line) {
			if (FileReader::readline(line)) {
	//			moveclass(line);
				filter(line);
				return true;
			}		
			return false;
		}
	
};
	
}

#endif /*EXAMPLEREADER_H_*/

