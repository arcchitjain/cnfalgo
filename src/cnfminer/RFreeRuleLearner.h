/*
 *  RFreeRuleLearner.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 06/08/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

/*
 *  FaultTolerantRuleLearner.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 25/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#ifndef RFREERULELEARNER_H_
#define RFREERULELEARNER_H_

#include "RuleLearner.h"
#include <map>

namespace cnf {
	
	class RFreeRuleLearner : public cnf::RuleLearner {
		
	private:
		
		smart_ptr<RuleTrie> rrules_;
		
	public:
		RFreeRuleLearner(int k) :
		RuleLearner(k), rrules_(NULL) {}
		
		virtual ~RFreeRuleLearner();
				
		virtual double process_example(const std::vector<int>& example, int ex_id);
		
		virtual void printInfo();
		
		virtual void initialize();
		
		virtual bool hasSecondaryRules() const { return true; }
		
		virtual RuleTrie& getSecondaryRules() const { return *rrules_; }
		
		
	protected:
		
		inline RuleTrie& getRRules() { return *rrules_; }
		
		virtual bool refineRule(RuleNode& rulenode, const std::vector<int>& example, const std::vector<int>& extensions);
				
		void refineRuleRecursive(RuleNode& rulenode, const std::vector<int>& example, const std::vector<int>& extensions);
	};
	
}

#endif /*FTRULELEARNER_H_*/
