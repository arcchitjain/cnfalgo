#ifndef DISTBASEDDRIFTFILTER_H
#define DISTBASEDDRIFTFILTER_H

//#include "../experiment/metric/Metric.h"
#include "../distribution/Distribution.h"
#include "ExampleFilter.h"
#include "../ruletrie/RuleTrie.h"

#include <iostream>

namespace cnf {

class Metric;
//class RuleTrie;

class DistBasedDriftFilter : public ExampleFilter {

public:

	static int metric_value;


private:
	
	Distribution& dist_;
	
	double conf_;
	
	Metric& metric_;
	
	int bwin_;	// buffer window
	
	int twin_;	// time-out window (no filtering for this time after last drift)
	
	bool empty_buffer_;
	
	int last_drift_;	// ex_id at which last drift occured
	
	double drift_thresh_;
	
protected:
	
	inline Metric& getMetric() { return metric_; }
	
	inline double getConfidence() { return conf_; }
	
	inline Distribution& getDistribution() { return dist_; }
	
	inline int getBufferWindow() { return bwin_; }
	
	inline bool emptyBuffer() { return empty_buffer_; }
	
	inline int getTimeOutWindow() { return twin_; }
	
	inline double getDriftThreshold() { return drift_thresh_; }
	
	inline bool filterEnabled(int exid) { return exid > last_drift_ + twin_; }
	
public :
	
	DistBasedDriftFilter(ExampleContainer& cont, Distribution& dist, Metric& metric, double confidence, int bwin, int twin, double dth) : 
		ExampleFilter(cont), dist_(dist), conf_(confidence), metric_(metric), bwin_(bwin),
		twin_(twin), empty_buffer_(false), last_drift_(0), drift_thresh_(dth)  {}
	
	virtual ~DistBasedDriftFilter();
	
	virtual bool process_example(RuleLearner& learner,const std::vector<int>& example, const RuleList& rules, 
			RuleTrie& theory, int ex_id) ;
	
	virtual void stream_buffer(int ex_id, RuleLearner& learner);

};

} // end namespace

#endif /*DISTBASEDDRIFTFILTER_H*/
