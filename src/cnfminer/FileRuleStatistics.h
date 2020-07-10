#ifndef FILERULESTATISTICS_H_
#define FILERULESTATISTICS_H_

#include "RuleStatistics.h"
#include <fstream>
#include <string>

namespace cnf
{

class FileRuleStatistics : public cnf::RuleStatistics
{
	
private: 
	std::ofstream file;

	virtual void printMap(std::map<int,int>& m);
	
	virtual void printExtraStatistics();
		
public:
	FileRuleStatistics(int k, std::string filename ) : RuleStatistics(k) {
		file.open(filename.c_str(),std::ofstream::out);
	};
	
	virtual ~FileRuleStatistics() {
		if (file != NULL) { 
			file.flush();
			file.close();
		}
	};
	
	virtual void nextExample();

};

}

#endif /*FILERULESTATISTICS_H_*/
