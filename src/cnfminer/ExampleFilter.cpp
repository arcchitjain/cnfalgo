#include "ExampleFilter.h"

#include "../exampletrie/ExampleContainer.h"
#include "RuleLearner.h"


#include "../test/theorysize.h"
#include "../metric/CountMetric.h"
#include "../metric/LengthMetric.h"

namespace cnf {

int ExampleFilter::metric_value = 0 ;

ExampleFilter::~ExampleFilter() { delete &buffer_; }

void ExampleFilter::bufferExample(const std::vector<int>& example, int exid) { getBuffer().add(example,exid); }

bool ExampleFilter::process_example(RuleLearner& learner, const std::vector<int>& example, const RuleList& rules, 
						 RuleTrie& theory, int ex_id) {
/*
	profile::start_timer(PROF_FILTEREXA);

	double mval = calc_theory_size(rules,learner.getK(),learner.getLanguage().getSize(),true);
	metric_value += mval;
	profile::stop_timer(PROF_FILTEREXA);
	
	double total = calc_rule_space_size(0, learner.getK(), learner.getLanguage().getSize(), false) - metric_value;
	
	LengthMetric lm(true);
	CountMetric cm(true);
	
	double lval = lm.calculate(rules,theory);
	double cval = cm.calculate(rules,theory);
	
	
	std::cout << ex_id << " " << cval << " " << lval << " " << mval << " " << metric_value/total << " " << metric_value << std::endl;
*/
	return true;
}		

}

