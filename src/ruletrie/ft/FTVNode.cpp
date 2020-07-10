/*
 *  FTVNode.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 24/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#include "FTVNode.h"
#include "FTANode.h"
#include "FTRuleTrie.h"
#include <iostream>
#include <cassert>

namespace cnf
{
	
	FTVNode::~FTVNode()
	{
#if VERBOSE_MEM > 1
		std::cout << "~FTVNode() " << this << std::endl;
#endif		
		clear();
	}
	
	unsigned int FTVNode::length() const { return getParent().length(); }
	
	void FTVNode::clear() {
		FTNodeMap::iterator stop = getChildren().end();
		for (FTNodeMap::iterator i = getChildren().begin();i != stop; ++i) {
			delete (*i).second;
		}
		getChildren().clear();	
	}
	
	void FTVNode::addChild(FTANode* child) {
//#ifdef VERBOSE 
//		std::cout << "FTVNode::addChild(" << child << ") " << this << std::endl;
//#endif	
		getChildren().insert(std::make_pair(child->getValue(),child));
		
		
	}
	
	FTANode* FTVNode::addChild(unsigned int value) {
		FTANode* child = new FTANode(value,this);
		addChild(child);
		return child;
	}
	
	RuleNode* FTVNode::findRule(const int* rule,unsigned int length) {
		if (length == 0) {
			return NULL;
		} else {
			FTANode* child = getChild(abs(*rule));
			if (child == NULL) {
				return NULL;
			} else {
				return child->findRule(rule,length);
			}
		}
	}

	const RuleNode* FTVNode::findRule(const int* rule,unsigned int length) const {
		if (length == 0) {
			return NULL;
		} else {
			FTANode* child = getChild(abs(*rule));
			if (child == NULL) {
				return NULL;
			} else {
				return child->findRule(rule,length);
			}
		}
	}
	
	RuleNode& FTVNode::addRule(const int* rule, unsigned int length) {
		assert(length > 0);
		FTANode* child = getChild(abs(*rule));
		if (child == NULL) { // no subnode for the current AtomNode
			child = addChild(abs(*rule));
		}
		return child->addRule(rule,length);
	}
	
	void FTVNode::getRule(std::vector<int>& rule,unsigned int split) const {
		getParent().getRule(rule, split);
		if (!getValue() && rule.size() > 0) rule.back() *= -1;	
	}
	
	const RuleNode* FTVNode::firstRule() const {
		FTNodeMap::const_iterator child;
		FTNodeMap::const_iterator stop = getChildren().end();
		for (child = getChildren().begin(); child != stop; ++child) {
			const RuleNode* result = child->second->firstRule();
			if (result) return result;
		}
		return NULL;
	}

	RuleNode* FTVNode::firstRule() {
		FTNodeMap::iterator child;
		FTNodeMap::iterator stop = getChildren().end();
		for (child = getChildren().begin(); child != stop; ++child) {
			RuleNode* result = child->second->firstRule();
			if (result) return result;
		}
		return NULL;
	}	


	const RuleNode* FTVNode::nextRule(const int* rule, unsigned int length) const {
		if (length == 0) {
			return firstRule();
		} else {
			const RuleNode* result = NULL;
			FTANode* child = getChild(abs(*rule));
			if (child != NULL) result = child->nextRule(rule,length); 
			if (!result) {
				FTNodeMap::const_iterator iter;
				FTNodeMap::const_iterator stop = getChildren().end();
				for (iter = getChildren().upper_bound(abs(*rule)); iter != stop; ++iter ) {
					result = iter->second->firstRule();
					if (result) return result;
				}
				return NULL;
			}
			return result;
		}
	}
	
	const RuleNode* FTVNode::next(int value) const {
		FTNodeMap::const_iterator n = getChildren().upper_bound(value);
		if (n == getChildren().end()) {
			return getParent().next(getValue());
		} else {
			return n->second->firstRule(); // should always exist
		}
	}

	RuleNode* FTVNode::next(int value) {
		FTNodeMap::iterator n = getChildren().upper_bound(value);
		if (n == getChildren().end()) {
			return getParent().next(getValue());
		} else {
			return n->second->firstRule(); // should always exist
		}
	}	
/*	
	const RuleNode* FTVNode::next() const {
		FTNodeMap::const_iterator n = getChildren().begin();
		if (n == getChildren().end()) {
			return getParent().next(getValue());
		} else {
			return n->second->firstRule(); // should always exist
		}
	}
	
	FTVNode* FTVNode::next() {
		FTNodeMap::const_iterator n = getChildren().begin();
		if (n == getChildren().end()) {
			return getParent().next(getValue());
		} else {
			return n->second->firstRule(); // should always exist
		}
	}
*/
	
	const FTRuleTrie& FTVNode::getRoot() const { return getParent().getRoot(); }
	FTRuleTrie& FTVNode::getRoot() { return getParent().getRoot(); }
	
	bool FTVNode::findSubrules(const int* rule, unsigned int length, RuleList* rules) const {
		// All nodes in the given rule have been handled and there are still nodes left.
		//					-> no subrules can exist in the rest of this branch
		if (length == 0) return false;
		
		FTANode* child = getChild(abs(*rule));
		if (child == NULL) {
			// Current rule head not found -> continue with next element in rule
			return findSubrules(rule+1,length-1,rules);
		} else {
			// A child starting with the current element was found.
			if (child->findSubrules(rule,length,rules)) { // a subrule was found
				if (rules == NULL) return true;
				else {
					findSubrules(rule+1,length-1,rules);
					return true;
				}
			} else {
				return findSubrules(rule+1,length-1,rules);
			}
		}
	}
	
	bool FTVNode::findSubrule(const int* rule, unsigned int length, const int* mc, unsigned int mcl, RuleList* rules) const {
		if (length==0 || abs(*mc) < abs(*rule)) {
			// The given rule is empty or mc is smaller than the rulehead.
			// The current branch can only be a subrule if this node contains
			//   a child == mc[i].
			FTANode* child = getChild(abs(*mc));
			if (child == NULL) { // no such child
				return false; // no solution in this branch
			} else { // such a child exists
				// A child was found with the given value.
				// Continue with that child.
				return child->findSubrule(rule,length,mc,mcl,rules);
			}
		} else {
			FTANode* child = getChild(abs(*rule));
			bool result = false; 
			if (child != NULL) { // child found -> first try out child
				result = (child->findSubrule(rule,length,mc,mcl,rules));
			} 
			if (rules == NULL && result) return result;
			// maybe there are subrules without the current child
			return findSubrule(rule+1,length-1,mc,mcl,rules) || result;
		}
		
	}
	
	bool FTVNode::findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const {
		assert(mcl>0);
		assert(length == 0 || *rule != 0);
		
		// return value: true if for all values in mc a subrule was found (and mc only contains 0's)
		bool result = true;
		for (unsigned int i=0; i<mcl; ++i) {
			//std::cout << str(mc,mcl) << std::endl;
			if (mc[i] != 0) { // only consider valid items
				if (length==0 || abs(mc[i]) < abs(*rule)) {
					// The given rule is empty or mc is smaller than the rulehead.
					// The current branch can only be a subrule if this node contains
					//   a child == mc[i].
					FTANode* child = getChild(abs(mc[i]));
					if (child == NULL) { // no such child
						result = false;  // no solution in this branch
					} else { // such a child exists
						// A child was found with the given value.
						// Continue with that child (only pass current value for mc)
						result = child->findSubrules(rule,length,mc+i,1) && result;
					}
				} else { // first consider head of rule (optional)
					FTANode* child = getChild(abs(*rule));
					if (child == NULL) { // child not found
						// continue with rest of rule
						// drop mc up to current value (other ones are < current node)
						return findSubrules(rule+1,length-1,mc+i,mcl-i) && result;
					} else if (child->findSubrules(rule,length,mc+i,mcl-i)) {
						// A subrule was found for all remaining values in mc.
						return result;
					} else {
						// There is still an unmatched value remaining in mc.
						return findSubrules(rule+1,length-1,mc+i,mcl-i) && result;
						// We can return here because the first test will always fail.
						// We know length > 0 and mc[i] not < *rule.
						// The else branch does not depend on mc[i].
					}
				}
			}
		}
		return result;
	}
	
	bool FTVNode::findFailing(const int* rule, unsigned int length, 
							RuleList* rules, int maxl,unsigned int split) const {							
		
		if (maxl == 0) { // Reached maximal depth and stop here. 
			return false;
		} else {
			bool result = false;
				// Iterate through all children.
				// If child's value is in rule -> neg branch.
				// If child's value not in rule -> pos branch.
			FTNodeMap::const_iterator child = getChildren().begin();
			FTNodeMap::const_iterator stop = getChildren().end();
			while ((rules!=NULL || !result) && child != stop) {
				result = child->second->findFailing(rule,length,rules,maxl,split) || result;
				++child;
			};
			return result;
		}
	}
	
	void FTVNode::listRules(RuleList* rules) const {
		FTNodeMap::const_iterator stop = getChildren().end();
		for (FTNodeMap::const_iterator child = getChildren().begin(); child != stop; ++child) {
			child->second->listRules(rules);
		}
	}
	
	int FTVNode::countFailing(const int* rule, unsigned int length, int maxl) const {
		if (maxl == 0) { // Reached maximal depth and stop here. 
			return 0;
		} else {
			int result = 0;
			// Iterate through all children.
			// If child's value is in rule -> neg branch.
			// If child's value not in rule -> pos branch.
			FTNodeMap::const_iterator child = getChildren().begin();
			FTNodeMap::const_iterator stop = getChildren().end();
			while (child != stop) {
				result += child->second->countFailing(rule,length,maxl);
				++child;
			};
			return result;
		}
	}
	
	/*
	void FTVNode::remove() {
		std::vector<int> rule; getRule(rule);
		removeRule(&rule[0], rule.size());
	}
	
*/	
	bool FTVNode::removeRule(const int* rule, unsigned int length) {
		if (length == 0) {
			return empty();
		} else {
			FTNodeMap::iterator child = getChildren().find(abs(*rule));
			if (child == getChildren().end()) return false;
			else {
				if (child->second->removeRule(rule,length)) { // alteration in child
					if (child->second->empty()) {
						delete (*child).second;
						getChildren().erase(child);
					
						return empty();
					} else {
						return false;
					}
				} else { 
					return false;
				}
			}
		}
	}
	

	bool FTVNode::prune(unsigned int length) {
		FTNodeMap::iterator child = getChildren().begin();
		while (child != getChildren().end()) { // retest end() on each step
			if (child->second->prune(length)) {
				delete (*child).second;
				getChildren().erase(child++);
			} else {
				++child;
			}
		}
		return empty();
	}
	
	bool FTVNode::prune(const int* rule, unsigned int length) {
		assert(length > 0);
		
		FTNodeMap::iterator i = getChildren().begin();
		while (i != getChildren().end() && i->first < (unsigned int)abs(*rule)) {
			// For all the children before the rulehead -> try and find rulenodes.
			if (i->second->prune(rule,length)) {
				// Child did some pruning and became empty.
				delete (*i).second;
				getChildren().erase(i++);
			} else {
				++i;
			}
		}
		
		if (i != getChildren().end() && i->first == (unsigned int)abs(*rule)) {
			// exact value found
			if (i->second->prune(rule,length)) {
				// Prune but only use one branch.
				delete (*i).second;
				getChildren().erase(i);
			}
		}
		return empty();
	}
	
	void FTVNode::insertNode(const int* rule, unsigned int length, FTVNode* node) {	
		FTANode* child = getChild(abs(*rule));
		if (child == NULL) {
			child = addChild(abs(*rule));
		}
		child->insertNode(rule,length,node);
	}
	
	

/*	
	bool FTVNode::verify(const int* example, unsigned int length) {
		std::vector<int> rule;
		getRule(rule);
		
		unsigned int i=0;
		unsigned int j=0;
		while (j < rule.size()) {
			if (i >= length) {
				// We handled complete example -> everything else in rule must be positive to make rule fail
				if (rule[j] < 0) {
					return true;
				} else {
					++j;
				}
			} else if (abs(rule[j]) < example[i]) {
				if (rule[j] < 0) {
					return true; // First literal of rule is not in example -> if it is negative -> rule succeeds
				} else {
					++j;
				}
			} else if (abs(rule[j]) > example[i]) {
				// First atom of example is not in rule -> nothing happens.
				++i;
			} else if (abs(rule[j]) == example[i]) {
				// First literal of rule is in example -> if it is positive -> rule succeeds
				if (rule[j] > 0) {
					return true;
				} else {
					++i; ++j;
				}
			}
		}
		// We handle entire rule and reached this point: so far rule is subset of cc(e) -> fail
		return false;
	}
	
	void FTVNode::unlink() {
//#ifdef VERBOSE
//		std::cout << "FTVNode::unlink()" << std::endl;
//#endif		
			
		getRoot().removeRuleLength(length());
		getParent().unlinkChild(getValue());
		
//#ifdef VERBOSE
//		std::cout << "    ==> verifying: pos:" << getParent().hasPosChild() << " neg:" << getParent().hasNegChild() << std::endl;
//#endif
	}
	

	
	void FTVNode::moveTo(FTRuleTrie& newtrie) {
		std::vector<int> ruledata; getRule(ruledata);
#ifdef VERBOSE
		std::cout << "FTVNode::moveTo(" << newtrie.getIdentifier() << ") " << str(ruledata) << std::endl;
#endif		
		unlink();	// sever connection with parents in current trie
		newtrie.insertNode(&ruledata[0], ruledata.size(), this);		
	}
	
	void FTVNode::listRChildren(const std::vector<int>& example, std::vector<FTVNode*>& frules, std::vector<FTVNode*>& trules) {
		for (std::vector<FTVNode*>::iterator i = getRChildren().begin(); i!=getRChildren().end(); ++i) {
			if (!(*i)->verify(&example[0],example.size())) {	// child covers example
				(*i)->setFRecord(new FRecord(true));
				frules.push_back(*i);
				(*i)->listRChildren(example,frules,trules);
			} else {
				trules.push_back(*i);
			}
		}
	}
	
	
#endif

	
	bool FTVNode::verifyStructure() {
		for (FTNodeMap::iterator i = getChildren().begin(); i!=getChildren().end(); ++i) {
			if (&(i->second->getParent()) != this) {
				std::vector<int> r; getRule(r);
			
				std::cerr << "ERROR: trie structure invalid (FTVNode) " << getValue() << " " << i->first << " " << str(r) << std::endl;
				
				return false;
			}
		}
		return true;
	}	
*/	
	
	
} // end namespace cnf
