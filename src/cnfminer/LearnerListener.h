/*
 *  LearnerListener.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 18/03/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef LEARNERLISTENER_H
#define LEARNERLISTENER_H


namespace cnf {

class RuleLearner;

class LearnerListener {

	
	
public:
	virtual void learnerInit(const RuleLearner& src) = 0;
	
	virtual void learnerLangAdd(const RuleLearner& src, int element, int ex_id) = 0;
	
	virtual void learnerFailingRules(const RuleLearner& src, const RuleList& rules, int ex_id) = 0;
	
	virtual ~LearnerListener() {}
};


} // END NAMESPACE CNF

#endif /* LEARNERLISTENER_H */
