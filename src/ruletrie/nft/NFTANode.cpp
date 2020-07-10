#include "NFTANode.h"
#include <cassert>
#include <iostream>
#include "../../lib/profile.h"
#include "NFTRuleTrie.h"

#include <set>

#include "NFTVNode.h"

namespace cnf { 

#ifdef NFTANode_DLL
NFTANode::NFTANode(int val, NFTVNode& p, NFTTreeNode& prev, NFTTreeNode& next) : flagged_value_(val), parent_(p), previous_(prev), next_(next), poschild_(NULL), negchild_(NULL) {}
#else
NFTANode::NFTANode(int val, NFTVNode& p) : flagged_value_(val), parent_(p), poschild_(NULL), negchild_(NULL) {}
#endif

NFTANode::~NFTANode()
{
	clear();
	//std::cout << "destructing NFTANode " << this << std::endl;
}

void NFTANode::clear() {
	clearEndOfRule();
	delete poschild_;
	delete negchild_;
#ifdef NFTANode_DLL
	getPrev(getValue()).setNext(getValue(),getNext(getValue()));
	getNext(getValue()).setPrev(getValue(),getPrev(getValue()));
#endif
}

unsigned int NFTANode::length() const { return 1+getParent().length(); }

NFTTreeNode* NFTANode::findRule(const int* rule,unsigned int length) const {
	if (length == 1) { // Last node of rule found (is current one).
		
		if (isEndOfRule(*rule>0)) return const_cast<NFTANode*>(this);
		else return NULL;
	} else {
		if (hasChild(*rule>0)) {
			return getChild(*rule>0).findRule(rule+1,length-1);
		} else {
			return NULL;
		}
	}
}

void NFTANode::addChild(bool value) {
	assert(!hasChild(value));
	NFTVNode* child = new NFTVNode(*this,value);
	addChild(value, child);
}

NFTTreeNode& NFTANode::addRule(const int* rule, unsigned int length, int ex_id) {
	assert(ex_id != 0);	// precondition
	
	if (length == 1) { // current node is the last one -> add flag
		setExampleId(ex_id);
		setEndOfRulePosition(*rule>0);
		return *this;
	} else {
		if (!hasChild(*rule>0)) addChild(*rule>0);
		return getChild(*rule>0).addRule(rule+1,length-1, ex_id);
	}
}

const NFTTreeNode* NFTANode::firstRule() const {
	if (isEndOfRule(true)) return this;
	if (hasPosChild()) {
		const NFTTreeNode* result = getPosChild().firstRule();
		if (result) return result;
	}
	if (isEndOfRule(false))	return const_cast<NFTANode*>(this);
	if (hasNegChild()) return getNegChild().firstRule();
	return NULL;
}

const NFTTreeNode* NFTANode::nextRule(const int* rule, unsigned int length) const {
	assert(length > 0);
	assert(*rule != 0);
	
	if (length==1 && isEndOfRule(*rule>0)) return NULL; // exact rule found
 	if (*rule>0) {
		if (hasPosChild()) {
			const NFTTreeNode* result = getPosChild().nextRule(rule+1,length-1);
			if (result) return result;
		} 
		if (hasNegChild()) {
			const NFTTreeNode* result = getNegChild().firstRule();
			if (result) return result;
		}
	} else { // *rule < 0
		if (hasNegChild()) return getNegChild().nextRule(rule+1,length-1);
	}
	return NULL;
}

//#ifdef NFTANode_DLL
const NFTRuleTrie& NFTANode::getRoot() const { return getParent().getRoot(); }
//#endif 

void NFTANode::getRuleInner(std::vector<int>& rule, unsigned int split) const {
//	 std::cout << getValue() << " " << split << std::endl;
	if (getValue() >= split) {
//		std::cout << "adding to rule" << std::endl;
		getParent().getRuleInner(rule,split);
		rule.push_back(getValue());
	} else {
	//	std::cout << "FAIL" << std::endl;
	}
}

void NFTANode::getRule(std::vector<int>& rule,unsigned int split) const {
	if (getValue() >= split) {
		getParent().getRuleInner(rule,split);
		rule.push_back(getEndOfRuleValue());
	}
}

const NFTTreeNode* NFTANode::next(bool value) const {
	const NFTTreeNode* result = NULL;
	if (value && hasNegChild()) result = getNegChild().firstRule();
	if (result == NULL) return next();
	return result;
}

const NFTTreeNode* NFTANode::next() const {
	return getParent().next(getValue());
}
	

bool NFTANode::findSubrules(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules) const {
	assert(length > 0);
	assert(getValue() == uabs(*rule));
	
	if (isEndOfRule(*rule>0)) { // current node is end node
		if (rules != NULL) {
			rules->push_back(const_cast<NFTANode*>(this));
			if (hasChild(*rule>0)) getChild(*rule>0).findSubrules(rule+1,length-1,rules);
		}
		return true;
	} else {
		if (hasChild(*rule>0)) return getChild(*rule>0).findSubrules(rule+1,length-1,rules); 
		else return false;
	}
}

bool NFTANode::findSubrule(const int* rule, unsigned int length, int extension) const {
	if (uabs(extension) == getValue()) {
		// Current value equals extension.
		// If the current value is EOR: stop and return true;
		if (isEndOfRule(extension>0)) 
			return true;
		else // Current node is not a rule.
			return (hasChild(extension>0) && getChild(extension>0).findSubrules(rule,length,NULL));
	} else {
		return (hasChild(*rule>0) && getChild(*rule>0).findSubrule(rule+1,length-1,extension));
	}
	
}

bool NFTANode::findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const {
	assert(mcl > 0);
	assert((uabs(*mc)==getValue() && mcl == 1) || (length > 0 && uabs(*rule) == getValue()));
	// Either mc[0] == value or rule[0] == value.

	
	if (uabs(*mc) == getValue()) {
		// We have found mustcontain.
		// If current rule is EOR: stop and return true.
		if (isEndOfRule(*mc > 0)) {
			*mc = 0;
			return true;
		} else {
			// Not EOR: continue with regular version.
			// Don't advance rule since we did not use the current head.
			if (hasChild(*mc>0) && getChild(*mc>0).findSubrules(rule,length,NULL)) {
				*mc = 0;
				return true;
			} else {
				return false;
			}
		}
	} else { // We did not find mustcontain yet. (We know length > 0.) 
		// Advance with rest of rule.
		return (hasChild(*rule>0) && getChild(*rule>0).findSubrules(rule+1,length-1,mc,mcl));
	}	
}

bool NFTANode::findFailing(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules, int maxl,unsigned int split) const {
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
			// Current node could be the end of a rule (must be positive).
			if (isEndOfRule(true)) { 
				if (rules != NULL) {
					rules->push_back(const_cast<NFTANode*>(this));
					if (hasPosChild()) getPosChild().findFailing(rule,length,rules,maxl-1,split);
				}
				return true;
			} else { // or not. Look for subrules in the children.
				return (hasPosChild() && getPosChild().findFailing(rule,length,rules,maxl-1,split));
			}
		} else if (getValue() == uabs(*rule)) {
			// Current node is in the rule so it should be negative.
			if (isEndOfRule(false)) { // Could be end of rule.
				if (rules != NULL) {
					rules->push_back(const_cast<NFTANode*>(this));
					if (hasNegChild()) getNegChild().findFailing(rule+1,length-1,rules,maxl-1,split);
				}
				return true;
			} else { // or not. Continue with children.
				return (hasNegChild() && getNegChild().findFailing(rule+1,length-1,rules,maxl-1,split));
			}
		} else { // Should not happen.
			assert(false);
			return false;
		}
	}
}

void NFTANode::listRules(std::vector<NFTTreeNode*>* rules) const {
	if (isEndOfRule()) {
		rules->push_back(const_cast<NFTANode*>(this));
	} else {
		if (hasPosChild()) getPosChild().listRules(rules);
		if (hasNegChild()) getNegChild().listRules(rules);
	}
}

int NFTANode::countFailing(const int* rule, unsigned int length, int maxl) const {
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
		if (isEndOfRule(true)) { 
			if (hasPosChild()) return 1 + getPosChild().countFailing(rule,length,maxl-1);
			else return 1;
		} else { // or not. Look for subrules in the children.
			if (hasPosChild()) return getPosChild().countFailing(rule,length,maxl-1);
			else return 0;
		}
	} else if (getValue() == uabs(*rule)) {
		// Current node is in the rule so it should be negative.
		if (isEndOfRule(false)) { // Could be end of rule.
			if (hasNegChild()) return 1 + getNegChild().countFailing(rule+1,length-1,maxl-1);
			else return 1;
		} else { // or not. Continue with children.
			if (hasNegChild()) return getNegChild().countFailing(rule+1,length-1,maxl-1);
			else return 0;
		}
	} else { // Should not happen.
		assert(false);
		return 0;
	}
}
	

bool NFTANode::removeRule(const int* rule, unsigned int length) {
	assert(length != 0);
	if (length == 1) {
		if (isEndOfRule(*rule>0)) clearEndOfRule();
		return empty();
	} else {
		if (hasChild(*rule>0) && getChild(*rule>0).removeRule(rule+1, length-1)) {
			removeChild(*rule>0);
			return true;
		} else { return false; }
	}
}

void NFTANode::removePosChild() { delete poschild_; poschild_ = NULL; }
void NFTANode::removeNegChild() { delete negchild_; negchild_ = NULL; }

bool NFTANode::prune(unsigned int length) {
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

bool NFTANode::prune(const int* rule, unsigned int length) {
//	assert(length > 0);
	
	if (length == 1) {
		// All values except last were found.
		if ((unsigned int)abs(*rule) == getValue()) {
			// Last value also found: prune this branch.
			removeChild(*rule > 0);
			if (isEndOfRule(*rule > 0)) clearEndOfRule();
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

/*
void NFTANode::getRedundant(std::set<NFTTreeNode*>& rules )  const {
	if (hasPosChild() && hasNegChild()) {
		std::vector<NFTTreeNode*> posChildren;
		std::vector<NFTTreeNode*> negChildren;
		getPosChild().listRules(&posChildren);
		getNegChild().listRules(&negChildren);
		
		const NFTRuleTrie& root = getRoot();
		int depth = length();
		int maxdepth = root.maxlength(); 
		
		for (std::vector<NFTTreeNode*>::iterator r1 = posChildren.begin(); r1 != posChildren.end(); ++r1) {
			for (std::vector<NFTTreeNode*>::iterator r2 = negChildren.begin(); r2 != negChildren.end(); ++r2) {	
				std::vector<int> rule1; 
				(*r1)->getRule(rule1);
				std::vector<int> rule2;
				(*r2)->getRule(rule2);
				std::vector<int> prefix;
				getRuleInner(prefix);
				prefix.pop_back();
				
				std::vector<int> result;
				if (calc_union(&rule1[depth],rule1.size()-depth,&rule2[depth],rule2.size()-depth,result,maxdepth-depth+1)) {
					for (std::vector<int>::iterator x = result.begin(); x!= result.end(); ++x) {
						prefix.push_back(*x);
					}
					
					
					

	//				for (int i=0; i<prefix.size();i++) { fprintf(stdout, "%i ",prefix[i]); } 
	//				fprintf(stdout, "RULE");
					
					NFTTreeNode* r = root.findRule(&prefix[0], prefix.size());
					if (r != NULL) {
						rules.insert(r);
					} 
				} else {
					//				for (int i=0; i<rule1.size();i++) { fprintf(stdout, "%i ",rule1[i]); } 
					//				fprintf(stdout, " + ");
					
					//				for (int i=0; i<rule2.size();i++) { fprintf(stdout, "%i ",rule2[i]); } 
					//				fprintf(stdout, " \n ");
					
				}
			}
		}
	}

	
	if (hasPosChild()) getPosChild().getRedundant(rules);
	if (hasNegChild()) getNegChild().getRedundant(rules);
}



bool NFTANode::calc_union(const int* a, const int al, const int* b, const int bl, std::vector<int>& result, unsigned int k) const {
		
//	for (int i=0; i<al;i++) { fprintf(stdout, "%i ",*(a+i)); } 
//	fprintf(stdout, " + ");
//	for (int i=0; i<bl;i++) { fprintf(stdout, "%i ",*(b+i)); } 
//	fprintf(stdout, " = ");

	
		if (al == k && bl == k) return false;
		unsigned int i=0;
		unsigned int j=0;
			
		while (i < al || j < bl) {
			if (i >= al) {
				result.push_back(b[j]); ++j;
			} else if (j >= bl) {
				result.push_back(a[i]); ++i;
			} else if (a[i] == b[j]) {
				result.push_back(a[i]); ++i; ++j;
			} else if (a[i] == -b[j]) {
				return false;
			} else if (abs(a[i]) < abs(b[j])) {
				result.push_back(a[i]); ++i;
			} else {
				result.push_back(b[j]); ++j;
			}
			if (result.size() > k) {
				return false;
			}
		}
//	for (int i=0; i<result.size();i++) { fprintf(stdout, "%i ",result[i]); } 
//	fprintf(stdout, "\n");

	
		return true;
}
*/	

#ifdef NFTANode_MEMALLOC
NFTANode* NFTANode::freelist = NULL;
#endif


} // end namespace

