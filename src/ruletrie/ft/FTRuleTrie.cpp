/*
 *  FTRuleTrie.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 24/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#include "FTRuleTrie.h"
#include "FTANode.h"
#include "FTVNode.h"
#include "../../lib/profile.h"
#include "../../lib/functions.h"


namespace cnf
{
	
	FTRuleTrie::~FTRuleTrie() {
#if VERBOSE_MEM > 1
		std::cout << "~RTRuleTrie() " << this << std::endl;
#endif
		
	}
	
	void FTRuleTrie::clear() {
		removeChild(true);
		rulelength.clear();
		addRule(NULL,0);
	}
	
	RuleNode* FTRuleTrie::findRule(const int* rule,unsigned int length) const {
#if VERBOSE > 1
		std::cout << "FTRuleTrie(" << getIdentifier() << ")::findRule([" << str(rule,length) << "])" << std::endl;
#endif	
		if (hasChild()) {
			return getChild().findRule(rule,length); 
		} else {
			return NULL;
		}
	}
	
	RuleNode& FTRuleTrie::addRule(const int* rule, unsigned int length) {
#if VERBOSE > 5
		std::cout << "FTRuleTrie(" << getIdentifier() << ")::addRule(" << str(rule,length) << ")" << std::endl;
#endif
		addRuleLength(length);
		if (!hasChild()) addChild(true,length==0);
		return getChild().addRule(rule,length);
	}
	
	bool FTRuleTrie::extendRule(const std::vector<int>& rule, int extension) {
		profile::start_timer(PROF_FINDSUBRULES);	
		if (!findSubrules(&rule[0],static_cast<int>(rule.size()),&extension,1)) {
			profile::stop_timer(PROF_FINDSUBRULES);
			// For some extensions, a subrule was found.
			// These extensions have been set to 0 in the ext-array.
			
			profile::start_timer(PROF_ADDRULE);
			// Create the new rule by inserting the extension.
			//  (maybe we shouldn't do this explicitly)
			std::vector<int> newrule;
			insert(extension,rule,newrule);
			
			// Add the new rule to the ruletrie (and set the example-number)
			//	std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << std::endl;
			addRule(&newrule[0],static_cast<int>(newrule.size()));
			profile::stop_timer(PROF_ADDRULE);
			return true;
		} else {
			profile::stop_timer(PROF_FINDSUBRULES);
			return false;
		}
	}
	
	bool FTRuleTrie::removeRule(const int* rule, unsigned int length) {
#ifdef VERBOSE 
		std::cout << "FTRuleTrie(" << getIdentifier() << ")::removeRule(" << str(rule,length) << ")" << std::endl;
#endif
		
		removeRuleLength(length);
		if (getChild().removeRule(rule,length)) {
			removeChild(true);
		}
		return false;
	}
		
	const FTRuleTrie& FTRuleTrie::getRoot() const { return *this; }
	FTRuleTrie& FTRuleTrie::getRoot() { 
//#ifdef VERBOSE
//			std::cout << "FTRuleTrie::getRoot() " << this << std::endl;
//#endif		
			return *this;
	}
	
	bool FTRuleTrie::findSubrules(const int* rule, unsigned int length, RuleList* rules) const {
		if (hasChild()) return getChild().findSubrules(rule,length,rules);
		else return false;
	}


	bool FTRuleTrie::findSubrule(const int* rule, unsigned int length, int extension,RuleList* rules) const {
		int* ext = new int[1];
		ext[0] = extension;
		return findSubrule(rule,length,ext,1,rules);
	}
	
	bool FTRuleTrie::findSubrule(const int* rule, unsigned int length, const int* mc, unsigned int mcl, RuleList* rules) const {
#if VERBOSE > 2
		std::cout << "FTRuleTrie::findSubrule([" << str(rule,length) << "], [" << str(mc,mcl) << "])" << " " << getIdentifier() << std::endl;
#endif
		if (mcl == 0) return false;
		if (hasChild()) return getChild().findSubrule(rule,length,mc,mcl,rules);
		else return false;
	}
	
	bool FTRuleTrie::findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const {
		// current empty rule cannot match because it does not contain mc value
		if (mcl == 0) return true;
		if (hasChild()) return getChild().findSubrules(rule,length,mc,mcl);
		else return false;
	}
	
	bool FTRuleTrie::findFailing(const int* rule, unsigned int length, RuleList* rules, int maxl,unsigned int split) const {
#if VERBOSE > 2
		std::cout << "FTRuleTrie::findFailing([" << str(rule,length) << "]," << rules << ") " << this << " " << (hasChild() ? &getChild() : NULL) << std::endl; 
#endif
		if (hasChild()) return getChild().findFailing(rule,length,rules,maxl,split);
		else return false;
	}
	
	void FTRuleTrie::listRules(RuleList* rules) const {
		if (hasChild()) return getChild().listRules(rules);
	}
	
	
	int FTRuleTrie::countFailing(const int* rule, unsigned int length, int maxl) const {
		if (hasChild()) return getChild().countFailing(rule,length,maxl);
		else return 0;	
	}
	
	void FTRuleTrie::getRule(std::vector<int>& rule,unsigned int split) const { /* left blank intentionally */ }
	
	const RuleNode* FTRuleTrie::firstRule() const {
		if (hasChild()) {
			return getChild().firstRule();
		} else {
			return NULL;
		}
	}

	RuleNode* FTRuleTrie::firstRule() {
		if (hasChild()) {
			return getChild().firstRule();
		} else {
			return NULL;
		}
	}
		
	const RuleNode* FTRuleTrie::nextRule(const int* rule, unsigned int length) const {
		if (hasChild()) {
			if (length == 0) return getChild().firstRule();
			return getChild().nextRule(rule,length);
		} else {
			return NULL;
		}
	}

	
	bool FTRuleTrie::prune(unsigned int length) {
		if (removeRulesLength(length)) {
			if (hasChild()) 
				if (getChild().prune(length)) 
					removeChild(true);
			return false;
		} else {
			return false;
		}
	}
	
	bool FTRuleTrie::prune(const int* rule, unsigned int length) { 
		if (length == 0) {
			removeChild(true);
		} else if (hasChild()) {
			if (getChild().prune(rule,length)) {
				removeChild(true);
			}
		}
		return false;
	}
	
	
	void FTRuleTrie::insertNode(const int* rule, unsigned int length, FTVNode* node) {
#ifdef VERBOSE 
		std::cout << "FTRuleTrie(" << getIdentifier() << ")::insertNode([" << str(rule,length) << "]) " << this << std::endl;
#endif
		addRuleLength(length);
		if (length==0) {
			assert(!hasChild());	// can't have the rule yet!!!
			addChild(true,node);
			node->setParent(this);
		} else {
			if (!hasChild()) addChild(true,false);
			getChild().insertNode(rule,length,node);
		}
		
	}
	
	
	
}
