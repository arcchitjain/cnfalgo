#ifndef FTRULELEARNER_H_
#define FTRULELEARNER_H_

/*
 *  FTRuleLearner.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 24/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */
 
#include "RuleLearner.h"
#include <map>
#include "../exampletrie/ExampleTrie.h"

namespace cnf {
	
	class FTRuleLearner : public cnf::RuleLearner {

	public:
		FTRuleLearner(int k, unsigned int tolerance);
		virtual ~FTRuleLearner();
		
		virtual inline unsigned int getTolerance() const { return tolerance_; }
		
		virtual double process_example(const std::vector<int>& example, int ex_id);
		
		virtual void printInfo();
		
	protected :
	
		void filterFailing(const std::vector<int>& example, RuleList& failing);
		
		virtual inline ExampleTrie& getExamples() const { return examples_; }
		
		//virtual double add_example(const std::vector<int>& example, int ex_id);
		
		//virtual double remove_example(int ex_id);
		
		virtual std::vector<int>& getFExamples(TreeNode* clause);
		
		virtual inline std::map<TreeNode*, std::vector<int> >& getFRules() const { return rules_; }
		
		virtual void initialize();
		
	private:
		
		unsigned int tolerance_;
		
		ExampleTrie& examples_;
		
		std::map<TreeNode*, std::vector<int> >& rules_;
		
		std::map<int,int>& example_count_;
		
		void increaseExampleCount(int ex_id);
		
		void decreaseExampleCount(int ex_id);
		
		void merge_extensions(int ex_id, std::vector<std::pair<int, std::vector<int> > >& a, const std::vector<int>& b);
		
	};
	
}

#endif /*FTRULELEARNER_H_*/
