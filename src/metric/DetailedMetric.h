#ifndef DETAILEDMETRIC_H_
#define DETAILEDMETRIC_H_

#include "Metric.h"

namespace cnf
{

class DetailedMetric : public cnf::Metric
{
	
private:

	Metric* parent;
	
public:
	DetailedMetric(Metric* metric) : Metric(metric->isAbsolute()), parent(metric) {}
	virtual ~DetailedMetric() {
		delete parent;
	}
	
	virtual inline double calculate(const RuleList& rules, RuleTrie& theory) const {	
		
		
		std::cout << "Failing rules :" << std::endl;
				
		for (RuleList::const_iterator i=rules.begin(); i != rules.end(); i++) {
			//std::cout << (*i).toString() << std::endl;	
		}
		
		double result = parent->calculate(rules,theory);
		
		std::cout << "Metric result :" << result << std::endl;
		
		return result;
	}
	
	virtual inline std::string getName() const { return (isAbsolute() ? "count_abs" : "count"); }
};

}


#endif /*DETAILEDMETRIC_H_*/
