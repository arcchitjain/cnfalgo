/*!  
   \file KEqMetric.h 

   $Id$
   $URL$
   
   \version   $Rev$
   \author    Anton Dries
   \date      17/03/08
   \namespace cnfalgo
*/
#ifndef cnfalgoKEqMetric
#define cnfalgoKEqMetric

#include "Metric.h"
#include "../test/theorysize.h"
#include "../cnfminer/LearnerListener.h"

namespace cnf {

class KEqMetric : public Metric, public LearnerListener {

  private:
  
	int k_;
	
	int L_;
	
	int value_;
	
	Metric* om1;
	Metric* om2;

  public:
	
	KEqMetric(int k, int L) : k_(k), L_(L)  {
		om1 = new LengthMetric(false);
		om2 = new CountMetric(false);
	}
	
	virtual double calculate(const RuleList& rules, RuleTrie& theory) const {
		return calc_theory_size(rules,k_,L_,true);
	}


	~KEqMetric() {}
	
	virtual std::string getName() const { return "Mk-eq"; }
	
	virtual void learnerInit(const RuleLearner& src) {
		value_ = calc_rule_space_size(0,k_,L_,false);
		std::cout << "initialized MkEq to " << value_ << std::endl;
	}
	
	virtual void learnerFailingRules(const RuleLearner& src, const RuleList& rules, int ex_id) {
		int oldvalue = value_;
		value_ -= calc_theory_size(rules,k_,L_,true);
		
		double m1 = om1->calculate(rules,src.getRules());
		double m2 = om2->calculate(rules,src.getRules());
		//std::cout << ex_id << " => updated MkEq by " << oldvalue-value_ << " to " << value_ << " (" << (double)(oldvalue-value_)/(double)oldvalue << ")" <<  std::endl;
		std::cerr << ex_id << " " << (double)(oldvalue-value_)/(double)oldvalue << " " << m1  << " " << m2 << " " << oldvalue << " " << value_ << " " << oldvalue-value_ << " " << rules.size() << std::endl;
	}
	
	virtual void learnerLangAdd(const RuleLearner& src, int element, int ex_id) {}

};

}  // end namespace cnf

#endif // cnfalgoKEqMetric
