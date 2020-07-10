#include "NFTVNode.h"
#include "NFTANode.h"
#include "NFTRuleTrie.h"
#include "NFTTreeNode.h"
#include <iostream>
#include <cassert>
namespace cnf
{

NFTVNode::~NFTVNode()
{
	//std::cout << "destructing NFTVNode " << this << std::endl; 
	clear();
}

unsigned int NFTVNode::length() const { return getParent().length(); }

void NFTVNode::clear() {
	NFTNodeMap::iterator stop = getChildren().end();
	for (NFTNodeMap::iterator i = getChildren().begin();i != stop; ++i) {
		delete (*i).second;
	}
	getChildren().clear();	
}

void NFTVNode::addChild(NFTANode* child) {
	getChildren().insert(std::make_pair(child->getValue(),child));
}

NFTANode* NFTVNode::addChild(unsigned int value) {
	assert(value > 0);
#ifdef NFTANode_DLL
	NFTRuleTrie& root = getRoot();
	NFTANode* child = new NFTANode(value,*this,root.getLastNode(value), root);
	root.addNode(*child);
#else
	NFTANode* child = new NFTANode(value,*this);
#endif
	addChild(child);
	return child;
}

NFTTreeNode* NFTVNode::findRule(const int* rule,unsigned int length) const {
	assert(length > 0);
	NFTANode* child = getChild(abs(*rule));
	if (child == NULL) {
		return NULL;
	} else {
//		std::cout << "child found " << std::endl;
		return child->findRule(rule,length);
	}
}

NFTTreeNode& NFTVNode::addRule(const int* rule, unsigned int length, int exid) {
//	std::cout << "finding child in "; for (int i=0; i<getChildren().size(); i++) std::cout << getChildren()[i].first << " "; std::cout << std::endl;
	NFTANode* child = getChild(abs(*rule));
//	std::cout << "finding child " << ((child == NULL) ? "failed" : "success") << std::endl; 
		
	if (child == NULL) { // no subnode for the current AtomNode
		child = addChild(abs(*rule));
	}
	return child->addRule(rule,length,exid);
}

void NFTVNode::getRuleInner(std::vector<int>& rule,unsigned int split) const {
	getParent().getRuleInner(rule, split);
	if (!getValue() && rule.size() > 0) rule.back() *= -1;	
}

const NFTTreeNode* NFTVNode::firstRule() const {
	NFTNodeMap::const_iterator child;
	NFTNodeMap::const_iterator stop = getChildren().end();
	for (child = getChildren().begin(); child != stop; ++child) {
		const NFTTreeNode* result = child->second->firstRule();
		if (result) return result;
	}
	return NULL;
}

const NFTTreeNode* NFTVNode::nextRule(const int* rule, unsigned int length) const {
	if (length == 0) {
		return firstRule();
	} else {
		const NFTTreeNode* result = NULL;
		NFTANode* child = getChild(abs(*rule));
		if (child != NULL) result = child->nextRule(rule,length); 
		if (!result) {
			NFTNodeMap::const_iterator iter;
			NFTNodeMap::const_iterator stop = getChildren().end();
			for (iter = getChildren().upper_bound(abs(*rule)); iter != stop; ++iter ) {
			//	for (NFTNodeMap::const_iterator i = getChildren().begin(); i != getChildren().end(); ++i) {
			//		std::cout << &(*i) << " | " << i->first << " == " << i->second->getValue() << " ? " << std::endl; 
			//	}
			//	std::cout << " selected : " << &(*iter) << " | " << std::endl;
			//	std::cout << iter->first << " == " << iter->second->getValue() << " ? " << std::endl; 
				result = iter->second->firstRule();
				if (result) return result;
			}
			return NULL;
		}
		return result;
	}
}

const NFTTreeNode* NFTVNode::next(int value) const {
	NFTNodeMap::const_iterator n = getChildren().upper_bound(value);
	if (n == getChildren().end()) {
		return getParent().next(getValue());
	} else {
		return n->second->firstRule(); // should always exist
	}
}

//#ifdef NFTANode_DLL
const NFTRuleTrie& NFTVNode::getRoot() const { return getParent().getRoot(); }
//#endif

bool NFTVNode::findSubrules(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules) const {
//#define NEWVERSION_SUBRULE
#ifdef NEWVERSION_SUBRULE
	assert(rules == NULL);
	if (length == 0) return false;
	
	NFTNodeMap::const_iterator child = getChildren().lower_bound(abs(*rule));
	NFTNodeMap::const_iterator stop = getChildren().end();
	
	if (child != stop && length > 0) {
	//	std::cout << child->first << " <--> " << abs(*rule) << std::endl;
		assert(child->first >= abs(*rule));
		if (child != getChildren().begin()) {
			assert((child-1)->first < abs(*rule));
		}
		
	}
	while (child != stop && length > 0) {
		while (length > 0 && abs(*rule) < child->first) {
			++rule; --length;
		}
		
		//if (length > 0) std::cout << child->first << " <--> " << *rule << std::endl;
		if (length > 0 && child->first == abs(*rule)) { // found rule head continue looking for rules in child
			if (child->second->findSubrules(rule,length,rules)) { // subrule found
				return true;
			} else { // the child corresponding to the current rulehead does not contain a subrule -> skip rule + child
				++rule; --length;
			}
		}
		++child;
	}
	return false;
	
#else
	if (length == 0) return false;
	// All nodes in the given rule have been handled and there are still nodes left.
	// (Otherwise this node would not exist.) -> current branch does not contain subrules.
		
	NFTANode* child = getChild(abs(*rule));
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
#endif
}

bool NFTVNode::findSubrule(const int* rule, unsigned int length, int extension) const {
	if (length==0 || abs(extension) < abs(*rule)) {
		// The given rule is empty or mc is smaller than the rulehead.
		// The current branch can only be a subrule if this node contains
		//   a child == mc[i].
		NFTANode* child = getChild(abs(extension));
		if (child == NULL) { // no such child
			return false; // no solution in this branch
		} else { // such a child exists
			// A child was found with the given value.
			// Continue with that child (only pass current value for mc)
			return child->findSubrule(rule,length,extension);
		}
	} else {
		NFTANode* child = getChild(abs(*rule));
		if (child == NULL) { // child not found
			// continue with rest of rule
			// drop mc up to current value (other ones are < current node)
			return findSubrule(rule+1,length-1,extension);
		} else {
			return (child->findSubrule(rule,length,extension));
		}
	}

}

bool NFTVNode::findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const {
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
				NFTANode* child = getChild(abs(mc[i]));
				if (child == NULL) { // no such child
					result = false;  // no solution in this branch
				} else { // such a child exists
					// A child was found with the given value.
					// Continue with that child (only pass current value for mc)
					result = child->findSubrules(rule,length,mc+i,1) && result;
				}
			} else { // first consider head of rule (optional)
				NFTANode* child = getChild(abs(*rule));
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
	//std::cout << "mcout" << str(mc,mcl) << std::endl;
	return result;
}

bool NFTVNode::findFailing(const int* rule, unsigned int length, 
							std::vector<NFTTreeNode*>* rules, int maxl,unsigned int split) const {
	if (maxl == 0) { // Reached maximal depth and stop here. 
		return false;
	} else {
		bool result = false;
		// Iterate through all children.
		// If child's value is in rule -> neg branch.
		// If child's value not in rule -> pos branch.
		NFTNodeMap::const_iterator child = getChildren().begin();
		NFTNodeMap::const_iterator stop = getChildren().end();
		while ((rules!=NULL || !result) && child != stop) {
			result = child->second->findFailing(rule,length,rules,maxl,split) || result;
			++child;
		};
		return result;
	}
}

void NFTVNode::listRules(std::vector<NFTTreeNode*>* rules) const {
	NFTNodeMap::const_iterator stop = getChildren().end();
	for (NFTNodeMap::const_iterator child = getChildren().begin(); child != stop; ++ child) {
		child->second->listRules(rules);
	}
}

int NFTVNode::countFailing(const int* rule, unsigned int length, int maxl) const {
	if (maxl == 0) { // Reached maximal depth and stop here. 
		return 0;
	} else {
		int result = 0;
		// Iterate through all children.
		// If child's value is in rule -> neg branch.
		// If child's value not in rule -> pos branch.
		NFTNodeMap::const_iterator child = getChildren().begin();
		NFTNodeMap::const_iterator stop = getChildren().end();
		while (child != stop) {
			result += child->second->countFailing(rule,length,maxl);
			++child;
		};
		return result;
	}
}

bool NFTVNode::removeRule(const int* rule, unsigned int length) {
	NFTNodeMap::iterator child = getChildren().find(abs(*rule));
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

bool NFTVNode::prune(unsigned int length) {
	NFTNodeMap::iterator child = getChildren().begin();
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

bool NFTVNode::prune(const int* rule, unsigned int length) {
	assert(length > 0);
	
	NFTNodeMap::iterator i = getChildren().begin();
	while (i != getChildren().end() && i->first < (unsigned int)(abs(*rule))) {
		// For all the children before the rulehead -> try and find rulenodes.
		if (i->second->prune(rule,length)) {
			// Child did some pruning and became empty.
			delete (*i).second;
			getChildren().erase(i++);
		} else {
			++i;
		}
	}
	
	if (i != getChildren().end() && i->first == (unsigned int)(abs(*rule))) {
		// exact value found
		if (i->second->prune(rule,length)) {
			// Prune but only use one branch.
			delete (*i).second;
			getChildren().erase(i);
		}
	}
	return empty();
}

/*
void NFTVNode::getRedundant(std::set<NFTTreeNode*>& rules) const {
	NFTNodeMap::const_iterator iter;
	NFTNodeMap::const_iterator stop = getChildren().end();
	for (iter = getChildren().begin(); iter != stop; ++iter ) {
		iter->second->getRedundant(rules);
	}

}
*/
	
#ifdef NFTVNode_MEMALLOC
NFTVNode* NFTVNode::freelist = NULL;
#endif

} // end namespace cnf
