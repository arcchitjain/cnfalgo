#include "RuleStatistics.h"
#include <iostream>

namespace cnf
{

void RuleStatistics::insert(std::map<int,int>& m, int length) {
	std::map<int,int>::iterator entry = m.find(length);
	if (entry == m.end()) {
		m.insert(std::make_pair(length,1));	
	} else {
		entry->second++;
	}
}

void RuleStatistics::addRule(int length) {
	insert(rulesizes_add,length);
}

void RuleStatistics::removeRule(int length) {
	insert(rulesizes_remove,length);
}

void RuleStatistics::testRule(int length) {
	insert(rulesizes_test,length);
}

void RuleStatistics::clear() {
	rulesizes_add.clear();
	rulesizes_remove.clear();
	rulesizes_test.clear();
	language_inc = 0;
	language_dec = 0;	
}


}
