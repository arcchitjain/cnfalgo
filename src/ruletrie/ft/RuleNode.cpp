/*
 *  RuleNode.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 07/08/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */


#include "RuleNode.h"
#include "FTRuleTrie.h"
#include "../../lib/functions.h"

namespace cnf {

	RuleNode::~RuleNode() { 
#if VERBOSE_MEM > 0
		std::cout << "~RuleNode() " << this << std::endl; 
#endif
		if (hasParent()) getParent().cleanUp();
		delete frecord_; 
		delete rrecord_; 
	}

	const RuleNode* RuleNode::next() const {
		return getParent().next(getValue());
	}
	
	RuleNode* RuleNode::next() {
		return getParent().next(getValue());
	}

	void RuleNode::unlink() { 
		getRoot().removeRuleLength(length());
		getParent().unlinkChild(getValue());
	}

	void RuleNode::moveTo(FTRuleTrie& newtrie) {
		std::vector<int> ruledata; getRule(ruledata);
		unlink();
		newtrie.insertNode(&ruledata[0], static_cast<int>(static_cast<int>(ruledata.size())), this);
	}	

	void RuleNode::clearFRecord() { delete frecord_; frecord_ = NULL; }
	
	void RuleNode::initRRecord() { 
		rrecord_ = new RRecord(this); 
	}

	void RuleNode::initExamples() {
		assert(!hasExamples());
		examples_ = new ExampleList();
	}
	
	bool RuleNode::isFailing() {
		return hasFRecord() && getFRecord().failing;
	}
	
	std::string RuleNode::toString() {
		std::vector<int> rr; getRule(rr);
		return str(rr);
	}
	
	void RRecord::listChildren(const std::vector<int>& example, RuleList& frules, RuleList& trules) {
	
	
		RRecordList myFailingChildren;
		for (RRecordList::iterator i = getChildren().begin(); i!=getChildren().end(); ++i) {
			RuleNode* r = (*i)->getRule();
			
			// must be in redundant rules
			if (r->getRoot().getIdentifier() == 1) {

				if (!r->verify(&example[0],static_cast<int>(example.size()))) {	// child covers example
					if (!r->hasFRecord()) {
						r->setFRecord(new FRecord(true));
						frules.push_back(r);
						myFailingChildren.push_back(*i);
					}
				} else {
					trules.push_back(r);
				}
			}
		}
		
		for (RRecordList::iterator i = myFailingChildren.begin(); i != myFailingChildren.end(); ++i) {
			(*i)->listChildren(example,frules,trules);
		}
	}
	
	bool RRecord::hasNonRedundantParent(RRecord* loopcheck) {
		if (this == loopcheck) return false;
		if (getRule()->getRoot().getIdentifier() == 0) return true;
		
		for (RRecordPairList::iterator i = getParents().begin(); i!=getParents().end(); ++i) {
			if (i->first->hasNonRedundantParent(loopcheck)) return true;
			if (i->second->hasNonRedundantParent(loopcheck)) return true;		
		}
		return false;
	}

	bool RRecord::hasNonRedundantParent() {
		for (RRecordPairList::iterator i = getParents().begin(); i!=getParents().end(); ++i) {
			if (i->first->hasNonRedundantParent(this)) return true;
			if (i->second->hasNonRedundantParent(this)) return true;		
		}
		return false;
	}
	
	
	void RRecord::printParentTrace(int depth) {
		if (depth > 2) return;
		for (int i=0; i<depth; i++) std::cout << "  ";
		std::cout << depth << " [" << getRule()->toString() << "]" << getRule()->getRoot().getIdentifier() << std::endl;
		if (getRule()->getRoot().getIdentifier() == 1) {
			for (RRecordPairList::iterator i = getParents().begin(); i!=getParents().end(); ++i) {
				i->first->printParentTrace(depth+1);
				i->second->printParentTrace(depth+1);
			}
		}
	}
	
	RRecord::~RRecord() {
		for (RRecordPairList::iterator i = getParents().begin(); i!=getParents().end(); ++i) {
			i->first->removeChild(this);
			i->second->removeChild(this);
		}
		
		// remove me from children and parents
		for (RRecordList::iterator i = getChildren().begin(); i!= getChildren().end(); ++i) {
			(*i)->removeParent(this);
		}
		
	}
	
	bool RRecord::removeParent(RRecord* parent) { 
		for (RRecordPairList::iterator iter = getParents().begin(); iter!=getParents().end(); ++iter) {
			if (iter->first == parent || iter->second == parent) {
				if (iter->first == parent) {
					iter->second->removeChild(this);
				} else {
					iter->first->removeChild(this);				
				}
				
				getParents().erase(iter);
				if (getParents().empty() && getRule()->getRoot().getIdentifier() == 1) {
					
					
					//					std::cout << std::endl;
					//					std::cout << std::endl;				
			//		std::cout << "removed last parents of " << getRule()->toString() << " " << std::endl;
					//					std::cout << std::endl;
					//					std::cout << std::endl;
					
				}
				return true;
			}
		}
		return false;
	}
	

}
