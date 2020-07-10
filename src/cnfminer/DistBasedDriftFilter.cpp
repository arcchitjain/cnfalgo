#include "DistBasedDriftFilter.h"
#include "RuleLearner.h"
#include "../metric/Metric.h"
#include "../exampletrie/ExampleContainer.h"
#include "../test/theorysize.h"

namespace cnf {

int DistBasedDriftFilter::metric_value = 0 ;

DistBasedDriftFilter::~DistBasedDriftFilter() { delete &dist_; delete &metric_; }

bool DistBasedDriftFilter::process_example(RuleLearner& learner, const std::vector<int>& example, const RuleList& rules, 
	RuleTrie& theory, int ex_id) {
		
	//std::cout << "Filtering example " << ex_id << std::endl;
	
	//double mval = getMetric().calculate(rules,theory);
	//double mval = rules.size();
	double mval = calc_theory_size(rules,learner.getK(),learner.getLanguage().getSize(),true); // TODO add real value
	//double mval = calc_theory_size(rules,learner.getK(),34,true); // TODO add real value
	
	
	
	
	if (filterEnabled(ex_id) && ex_id > 0 && getDistribution().inverse_probability(mval) < getConfidence()) {
		metric_value += mval;	
		std::cout << getDistribution().toString() << " " << mval << " -> " << metric_value << " " << ex_id << std::endl;
		if (getBufferWindow() != 0) {
			bufferExample(example,ex_id);
			std::cout << "Buffering example " << ex_id << " (buffer size: " << getBuffer().size() << "/" << getBufferWindow() << ", probability: " << 1-getDistribution().probability(mval) << ")" << std::endl;
			if (getBuffer().size() > getBufferWindow()*getDriftThreshold()) {
				std::cout << "=====================>>>>>> Drift detected between example " << getBuffer().getFirst() << " and " << ex_id << std::endl;
			//	last_drift_ = getBuffer().getFirst();
			//	theory.clear();
			//	getDistribution().reset();
				
			}
		}

		return false; // probability that this example is in theory is lower than confidence
	} else {
		//std::cout << "Example " << ex_id << " cleared filter" << std::endl;
		getDistribution().update_data(mval);
		return true;
	}

}


void DistBasedDriftFilter::stream_buffer(int ex_id, RuleLearner& learner) {
	int to = -1;
	if (emptyBuffer()) {
		to = getBuffer().getLast();
	} else if (getBufferWindow() <= 0) {
		to = -1;
	} else {
		to = ex_id - getBufferWindow();
	}
	
	for (int i=getBuffer().getFirst(); i <= to; i++) {
		std::vector<int> ex;
		if (getBuffer().getExample(i,ex)) {
			//std::cout << "Unbuffering example " << i<< std::endl;
			getBuffer().remove(i);
			learner.process_example(ex,-i); 
			
			// first call process -> when filter is called ex_id <= buff.getLast()
			// so filter does not do anything
			
		}
	}
}

}
