/*
 *  FTANode.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 25/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#include "FTANode.h"
#include <cassert>
#include <iostream>
#include "FTRuleTrie.h"

#include <set>

#include "FTVNode.h"

namespace cnf { 
	
	FTANode::FTANode(int val, FTVNode* p) : value_(val), parent_(p), poschild_(NULL), negchild_(NULL) {
#if VERBOSE_MEM > 1
		std::cout << "FTANode(" << p << "," << val << ") " << this << std::endl;
#endif
		
	}
	
	FTANode::~FTANode() {
#if VERBOSE_MEM > 1
		std::cout << "~FTANode() " << this << std::endl;
#endif
		
		clear();
	}
	
	void FTANode::clear() {
		delete poschild_;
		delete negchild_;
	}
	
	unsigned int FTANode::length() const { return 1+getParent().length(); }
	
	const RuleNode* FTANode::findRule(const int* rule,unsigned int length) const {
		if (hasChild(*rule>0)) {
			return getChild(*rule>0).findRule(rule+1,length-1);
		} else {
			return NULL;
		}
	}

	RuleNode* FTANode::findRule(const int* rule,unsigned int length) {
		if (hasChild(*rule>0)) {
			return getChild(*rule>0).findRule(rule+1,length-1);
		} else {
			return NULL;
		}
	}
	
	
	void FTANode::addChild(bool value, bool is_rule) {
		assert(!hasChild(value));
		
		
		FTVNode* child;
		if (is_rule) {
			child = new RuleNode(*this,value);
		} else {
			child = new FTVNode(*this,value);
		}
		addChild(value, child);
	}
	
	RuleNode& FTANode::addRule(const int* rule, unsigned int length) {
		if (!hasChild(*rule>0)) addChild(*rule>0, length==1);
		return getChild(*rule>0).addRule(rule+1,length-1);
	}
	
	const RuleNode* FTANode::firstRule() const {
		if (hasPosChild()) {
			const RuleNode* result = getPosChild().firstRule();
			if (result) return result;
		}
		if (hasNegChild()) return getNegChild().firstRule();
		return NULL;
	}

	RuleNode* FTANode::firstRule() {
		if (hasPosChild()) {
			RuleNode* result = getPosChild().firstRule();
			if (result) return result;
		}
		if (hasNegChild()) return getNegChild().firstRule();
		return NULL;
	}
	
	const RuleNode* FTANode::nextRule(const int* rule, unsigned int length) const {
		assert(length > 0);
		assert(*rule != 0);
		
		if (*rule>0) {
			if (hasPosChild()) {
				const RuleNode* result = getPosChild().nextRule(rule+1,length-1);
				if (result) return result;
			} 
			if (hasNegChild()) {
				const RuleNode* result = getNegChild().firstRule();
				if (result) return result;
			}
		} else { // *rule < 0
			if (hasNegChild()) return getNegChild().nextRule(rule+1,length-1);
		}
		return NULL;
	}
	
	const FTRuleTrie& FTANode::getRoot() const { return getParent().getRoot(); }
	FTRuleTrie& FTANode::getRoot() { 
//#ifdef VERBOSE
//		std::cout << "FTANode::getRoot() " << this << std::endl;
//#endif		
		assert(hasParent());
		return getParent().getRoot(); 
	}
	
	void FTANode::getRule(std::vector<int>& rule, unsigned int split) const {
		if (getValue() >= split) {
			getParent().getRule(rule,split);
			rule.push_back(getValue());
		}
	}
	
//	void FTANode::getRule(std::vector<int>& rule,unsigned int split) const {
//		if (getValue() >= split) {
//			getParent().getRuleInner(rule,split);
//			rule.push_back(getEndOfRuleValue());
//		}
//	}
	
	const RuleNode* FTANode::next(bool value) const {
//		std::cout << "FTANode::next(" << value << ")" << std::endl;
		const RuleNode* result = NULL;
		if (value && hasNegChild()) result = getNegChild().firstRule();
		if (result == NULL) return getParent().next(getValue());
		return result;
	}

	RuleNode* FTANode::next(bool value) {
		//		std::cout << "FTANode::next(" << value << ")" << std::endl;
		RuleNode* result = NULL;
		if (value && hasNegChild()) result = getNegChild().firstRule();
		if (result == NULL) return getParent().next(getValue());
		return result;
	}
	
/*	const FTVNode* FTANode::next() const {
		std::cout << "FTANode::next()" << std::endl;
		if (hasParent()) return getParent().next(getValue()); return NULL;
	}
*/	
	bool FTANode::findSubrules(const int* rule, unsigned int length, RuleList* rules) const {
		if (hasChild(*rule>0)) return getChild(*rule>0).findSubrules(rule+1,length-1,rules); 
		else return false;
	}
	
	bool FTANode::findSubrule(const int* rule, unsigned int length, const int* mc, unsigned int mcl, RuleList* rules) const {
//	bool FTANode::findSubrule(const int* rule, unsigned int length, int extension, std::vector<FTVNode*>* rules) const {
		if (uabs(*mc) == getValue()) {
			// Current value equals extension -> continue with regular subrule search
			if (mcl == 1) {
				return (hasChild(*mc>0) && getChild(*mc>0).findSubrules(rule,length,rules));
			} else {
				return (hasChild(*mc>0) && getChild(*mc>0).findSubrule(rule,length,mc+1,mcl-1,rules));
			}
		} else {
			return (hasChild(*rule>0) && getChild(*rule>0).findSubrule(rule+1,length-1,mc,mcl,rules));
		}
	}
	
	bool FTANode::findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const {
		assert(mcl > 0);
		assert((uabs(*mc)==getValue() && mcl == 1) || (length > 0 && uabs(*rule) == getValue()));
		// Either mc[0] == value or rule[0] == value.
		
		if (uabs(*mc) == getValue()) {
			// We have found mustcontain.
			// Not EOR: continue with regular version.
			// Don't advance rule since we did not use the current head.
			if (hasChild(*mc>0) && getChild(*mc>0).findSubrules(rule,length,NULL)) {
				*mc = 0;
				return true;
			} else {
				return false;
			}
		} else { // We did not find mustcontain yet. (We know length > 0.) 
			// Advance with rest of rule.
			return (hasChild(*rule>0) && getChild(*rule>0).findSubrules(rule+1,length-1,mc,mcl));
		}	
	}
	
	bool FTANode::findFailing(const int* rule, unsigned int length, RuleList* rules, int maxl,unsigned int split) const {
		// *rule > 0
		
		if (split > 0 && getValue() >= split && rules != NULL) {
			//	std::cout << "split reached: " << getValue() << ">=" << split << " " << rules->size() << std::endl;
			listRules(rules);
			//	std::cout  << rules->size() << std::endl;
			return true;
		} else {
			// Skip over the values in the rule that are not in the current branch.
			// We can safely skip these because we are looking for a subrule so not all 
			//   elements in the given rule should be in the branch.
			while (length > 0 && uabs(*rule) < getValue()) {
				++rule; --length;
			}
			
			// Now three cases can occur:
			//  - rule is empty -> all the rest of the values in the branch should be negative
			//  - next element in rule bigger than current node -> current node should be positive
			//  - next element in rule == current node -> current node should be negative
			if (length == 0 || uabs(*rule) > getValue()) {
				// Current node is not in the rule so it should be positive.
				// Current node could be the end of a rule (must be positive) or not. Look for subrules in the children.
				return (hasPosChild() && getPosChild().findFailing(rule,length,rules,maxl-1,split));
			} else if (getValue() == uabs(*rule)) {
				return (hasNegChild() && getNegChild().findFailing(rule+1,length-1,rules,maxl-1,split));
			} else { // Should not happen.
				assert(false);
				return false;
			}
		}
	}
	
	void FTANode::listRules(RuleList* rules) const {
		if (hasPosChild()) getPosChild().listRules(rules);
		if (hasNegChild()) getNegChild().listRules(rules);
	}
	
	int FTANode::countFailing(const int* rule, unsigned int length, int maxl) const {
		// *rule > 0
		
		// Skip over the values in the rule that are not in the current branch.
		// We can safely skip these because we are looking for a subrule so not all 
		//   elements in the given rule should be in the branch.
		while (length > 0 && uabs(*rule) < getValue()) {
			++rule; --length;
		}
		
		// Now three cases can occur:
		//  - rule is empty -> all the rest of the values in the branch should be negative
		//  - next element in rule bigger than current node -> current node should be positive
		//  - next element in rule == current node -> current node should be negative
		if (length == 0 || uabs(*rule) > getValue()) {
			// Current node is not in the rule so it should be positive.
			// Current node could be the end of a rule (must be positive).
			if (hasPosChild()) return getPosChild().countFailing(rule,length,maxl-1);
			else return 0;
		} else if (getValue() == uabs(*rule)) {
			// Current node is in the rule so it should be negative.
			if (hasNegChild()) return getNegChild().countFailing(rule+1,length-1,maxl-1);
			else return 0;
		} else { // Should not happen.
			assert(false);
			return 0;
		}
	}
	
	
	bool FTANode::removeRule(const int* rule, unsigned int length) {
		assert(length != 0);
		if (hasChild(*rule>0) && getChild(*rule>0).removeRule(rule+1, length-1)) {
			removeChild(*rule>0);
			return true;
		} else { return false; }
	}
	
	void FTANode::removePosChild() { delete poschild_; poschild_ = NULL; }
	void FTANode::removeNegChild() { delete negchild_; negchild_ = NULL; }


	bool FTANode::prune(unsigned int length) {
		if (length == 0) {
			// Maximal depth reached: delete all rules deeper.
			clear();
			return true;
		} else {
			if (hasPosChild() && getPosChild().prune(length-1)) { 
				removePosChild();
			}
			if (hasNegChild() && getNegChild().prune(length-1)) {
				removeNegChild();
			}
			return empty();
		}
	}
	
	bool FTANode::prune(const int* rule, unsigned int length) {
		//	assert(length > 0);
		
		if (length == 1) {
			// All values except last were found.
			if ((unsigned int)abs(*rule) == getValue()) {
				// Last value also found: prune this branch.
				removeChild(*rule > 0);
			} else {
				// Last value not yet found, continue with children.
				if (hasPosChild() && getPosChild().prune(rule,length)) {
					removePosChild();
				}
				if (hasNegChild() && getNegChild().prune(rule,length)) {
					removeNegChild();
				}
			}
		} else if ((unsigned int)abs(*rule) == getValue()) {
			// Current node == rulehead -> continue with children.
			if (hasChild(*rule > 0) && getChild(*rule > 0).prune(rule+1,length-1)) {
				removeChild(*rule > 0);
			}		
		} else {
			// Current rulehead not found.
			if (hasPosChild() && getPosChild().prune(rule,length)) {
				removePosChild();
			}
			if (hasNegChild() && getNegChild().prune(rule,length)) {
				removeNegChild();
			}
		}
		return empty();	
	}


#ifdef ANODE_MEMALLOC
	ANode* ANode::freelist = NULL;
#endif

/*	
	bool FTANode::verifyStructure() {
		if (hasPosChild()) {
			if (&getPosChild().getParent() != this) {
				std::cerr << "ERROR: trie structure invalid " << getValue() << " pos" << std::endl;
			  return false;
			}
			if (!getPosChild().verifyStructure()) return false;
		}
		if (hasNegChild()) {
			if (&getNegChild().getParent() != this) {
			std::cerr << "ERROR: trie structure invalid " << getValue() << " neg" << std::endl;
			return false;
			}
			if (!getNegChild().verifyStructure()) return false;
		}
		return true;
	}	
*/	
	
	void FTANode::insertNode(const int* rule, unsigned int length, FTVNode* node) {	
		if (length==1) {
			assert(!hasChild(*rule>0));	// can't have the rule yet!!!
			addChild(*rule>0,node);
			node->setParent(this);
		} else {
			if (!hasChild(*rule>0)) addChild(*rule>0,false);
			getChild(*rule>0).insertNode(rule+1,length-1,node);
		}
	}
	
	void FTANode::unlinkChild(bool cval) { 
//#ifdef VERBOSE
//		std::cout << "FTANode::unlinkChild(" << cval << ") " << this << std::endl;
//#endif	
		if (cval) poschild_ = NULL; else negchild_ = NULL; 
	//	if (empty() && hasParent()) {
	//		getParent().removeChild(getValue());
	//	}
	}
	
	void FTANode::cleanUp() {
		if (empty() && hasParent()) {
			getParent().removeChild(getValue());
		}
	}
	
	
	
} // end namespace


