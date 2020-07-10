#ifndef METRIC_H_
#define METRIC_H_

#include <vector>
#include <string>
#include "../ruletrie/RuleTrie.h"

namespace cnf
{

//class RuleTrie;
//class RuleList;

class Metric {
	

	
private :

	bool absolute;
	
public:
	Metric(bool a=false) : absolute(a) {}
	virtual ~Metric() {}
	
	virtual double calculate(const std::vector<int>& example, RuleTrie& theory) const;
	
	virtual double calculate(const RuleList& rules, RuleTrie& theory) const = 0;
	
	inline bool isAbsolute() const { return absolute; }
	
	virtual std::string getName() const = 0;
};

}

#endif /*METRIC_H_*/
