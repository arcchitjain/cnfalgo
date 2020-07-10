/*
 *  RuleIterator.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 14/02/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#include "RuleIterator.h"
#include "../RuleTrie.h"

#include "RuleTrieIterator.h"
#include "RuleListIterator.h"
#include "RuleVectorIterator.h"


namespace cnf {

RuleIterator* create(RuleTrie& parent) {
	return new RuleTrieIterator(parent);
}

RuleIterator* create(RuleList& parent) {
	return new RuleListIterator(parent);
}

RuleIterator* create(std::vector<RuleIterator::Rule>& parent) {
	return new RuleVectorIterator(parent);
}

} // end namespace cnf
