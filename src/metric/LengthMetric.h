#ifndef LENGTHMETRIC_H_
#define LENGTHMETRIC_H_

#include "Metric.h"
#include "../ruletrie/RuleTrie.h"


namespace cnf
{

class LengthMetric : public cnf::Metric
{
	
public:
	LengthMetric(bool abs=false) : Metric(abs) {}
	virtual ~LengthMetric() {}
	
	virtual inline double calculate(const RuleList& rules, RuleTrie& theory) const {	
		int k = theory.maxlength();
		double rl[k+1];
		for (int i=0; i<=k; i++) rl[i] = 0;
		
		for (RuleList::const_iterator i = rules.begin(); i != rules.end(); i++) {
			rl[(*i)->length()]++;
		}
		
		if (!isAbsolute()) {
			print_message(1,str(rules.size()) + " - ");
			for (int i=0; i<=k; i++) {
				print_message(1,str(rl[i]) + " ");	
				int div = theory.numberOfRules(i);
				if (div == 0 && rl[i] != 0) print_warning("Non existing rule failed");
				else if (div == 0) rl[i] = 0;
				else rl[i] = rl[i] / div;
			 	
			}
			print_message(1,"\n");
		}
		
		double result = 0;
		for (int i=0; i<=k; i++) {
			double p=1;
			if (i==0) p = 2; else p = (double)1/i;
			result += p*rl[i];
		}
			
		return result;
	}
	
	virtual inline std::string getName() const { return (isAbsolute() ? "length_abs" : "length"); }
};

}

#endif /*LENGTHMETRIC_H_*/
