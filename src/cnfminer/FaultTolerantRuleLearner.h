/*
 *  FaultTolerantRuleLearner.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 25/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#ifndef FAULTTOLERANTRULELEARNER_H_
#define FAULTTOLERANTRULELEARNER_H_


#include "RuleLearner.h"
#include <map>
#include "../ruletrie/ft/ExampleStore.h"

namespace cnf {
	
class FaultTolerantRuleLearner : public cnf::RuleLearner {

  private:
	
	const unsigned int tolerance_;

	ExampleStore examples_;
		
  public:
	FaultTolerantRuleLearner(int k, unsigned int tolerance) :
		RuleLearner(k), tolerance_(tolerance) {}
	
	virtual ~FaultTolerantRuleLearner();
		
	virtual inline unsigned int getTolerance() const { return tolerance_; }
		
	virtual double process_example(const std::vector<int>& example, int ex_id);
		
	virtual void printInfo();
	
	virtual int getCacheSize() { return getExamples().size(); }
	
  protected:
  
	inline ExampleStore& getExamples() { return examples_; }
		
  private :
  
	void filterFailing(const std::vector<int>& example, RuleList& failing);
  
#ifdef FTRULETRIE
  
	void merge_extensions(Example* ex_id, std::vector<std::pair<int, ExampleList > >& a, const std::vector<int>& b);

	void subtract_examplelists(ExampleList& a, ExampleList& b, ExampleList&c);
	
	void sort_failing(RuleList& failing);
#endif	
};
	
}

#endif /*FTRULELEARNER_H_*/
