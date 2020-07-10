/*
 *  RuleContainer.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 14/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RULEITERATOR_H
#define RULEITERATOR_H

#include <vector>
#include "../../ruletrie/RuleTrie.h"

namespace cnf {


//class RuleList;

class RuleIterator {

	public:
	
		typedef std::vector<int> Rule;
	
		virtual unsigned int size() const = 0;

		virtual void reset() = 0;
		
		virtual bool moveNext() = 0;
		
		virtual const Rule& current() const = 0;
		
		static RuleIterator* create(RuleTrie& parent);
		
		static RuleIterator* create(RuleList& parent);
		
		static RuleIterator* create(std::vector<RuleIterator::Rule>& parent);
		
		virtual ~RuleIterator() {}
};

} /* end namespace cnf */

#endif /* RULEITERATOR_H */
