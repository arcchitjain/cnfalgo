#ifndef COUNTMETRIC_H_
#define COUNTMETRIC_H_

#include "Metric.h"

namespace cnf
{

class CountMetric : public cnf::Metric
{
	

	
public:
	CountMetric(bool abs=false) : Metric(abs) {}
	virtual ~CountMetric() {}
	
	virtual inline double calculate(const RuleList& rules, RuleTrie& theory) const {	
		
		int rs = static_cast<int>(rules.size());
		print_message_nl(1," " + str(rs));
		if (isAbsolute()) return rs; 
		else {
			int ts = theory.numberOfRules();
			if (ts == 0) { 
				if (rs != 0) print_warning("A non-existing rule has failed!");
				else return 0;
			}
			return (double)rs/ts;
		}
	}
	
	virtual inline std::string getName() const { return (isAbsolute() ? "count_abs" : "count"); }
};

}

#endif /*COUNTMETRIC_H_*/
