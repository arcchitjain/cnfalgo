#ifndef RULEFILEREADER_H
#define RULEFILEREADER_H

#include "FileReader.h"

namespace cnf {

/*
 * Class for reading files.
 * 
 */
 
class RuleFileReader : public FileReader {
						
		
		virtual inline bool isNegative(int nc) {
			return (nc == '-');
		}
		
		virtual inline bool isSeparator(int nc) {
			return (nc == 32 || nc == ',');
		}
		
		virtual inline bool isComment(int nc) {
			return (nc == '!' || FileReader::isComment(nc));
		}
				
	public:
	
		RuleFileReader(std::string filename) :  
			FileReader(filename) {}	
	
};

}

#endif /* RULEFILEREADER_H */
