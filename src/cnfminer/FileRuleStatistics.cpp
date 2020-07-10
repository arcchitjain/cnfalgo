#include "FileRuleStatistics.h"
#include <iostream>
#include "sys/resource.h"
#include "../lib/memory.h"

namespace cnf
{

void FileRuleStatistics::printMap(std::map<int,int>& m) {
	std::map<int,int>::iterator a = m.begin();
	for (int i=0; i<=k; i++) {
		if (a == m.end()) {
			file << 0 << " ";
		} else if (a->first == i) {
			file << a->second << " ";
			a++;
		} else {
			file << "0 ";
		}
	}
}

void FileRuleStatistics::nextExample() {
	printMap(rulesizes_add);
	printMap(rulesizes_remove);
	printMap(rulesizes_test);
	file << language_inc << " " << language_dec << " ";
	printExtraStatistics();
	file << std::endl;
	clear();
}

void FileRuleStatistics::printExtraStatistics() {
	struct rusage(stats);
	if (getrusage(0,&stats) != 0)
		std::cerr << "rusage failed" << std::endl; 

	file << convert_timeval(stats.ru_utime) << " ";
	file << convert_timeval(stats.ru_stime) << " ";
	
	file << memoryUsage() << " 0 0";
	// file << stats.ru_maxrss << " ";
	// file << stats.ru_idrss << " ";
	// file << stats.ru_isrss << " ";
	
	
	//delete stats;
}



}

