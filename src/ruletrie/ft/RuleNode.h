/*
 *  RuleNode.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 07/08/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#ifndef RULENODE_H
#define RULENODE_H

#include "FTVNode.h"

namespace cnf {

class FTRuleTrie;
class RRecord;
class FRecord;

class RuleNode : public FTVNode {

  private:
  
	FRecord* frecord_;
	
	RRecord* rrecord_;
	
	ExampleList* examples_;
	
  public:
    RuleNode(FTANode& parent, bool value) : 
		FTVNode(parent,value), frecord_(NULL), rrecord_(NULL), examples_(NULL) {
#if VERBOSE_MEM > 0
		std::cout << "RuleNode(" << &parent << "," << value << ") " << this << std::endl;
#endif

		} 
		
	~RuleNode();

  protected: 	/* Trie operations */

	friend class AtomNode;
  
	virtual inline const RuleNode* findRule(const int* rule, unsigned int length) const { 
		if (length == 0) return this;  else return NULL;
	}
	
	virtual inline RuleNode* findRule(const int* rule, unsigned int length) { return this; }
	
	virtual inline RuleNode& addRule(const int* rule, unsigned int length) {
//#if VERBOSE > 2
//		std::cout << "RuleNode::addRule([" << str(rule,length) << "]) " << this << std::endl; 
//#endif
		return *this; 
	}
	
	virtual inline bool findSubrules(const int* rule, unsigned int length, 
															RuleList* rules) const {
		if (rules != NULL) rules->push_back(const_cast<RuleNode*>(this));
		return true;
	}
	
	virtual inline bool findSubrule(const int* rule, unsigned int length, 
										const int* mc, unsigned int mcl, RuleList* rules) const {
		// This case function is only called with mcl > 0 => current rule doesn't contain *mc.
		return false;
	}
	
	virtual inline bool findSubrules(const int* rule, unsigned int length,
																int* mc, unsigned int mcl) const {
		// This case function is only called with mcl > 0 => current rule doesn't contain *mc.
		return false;
	}
	
	virtual inline bool findFailing(const int* rule, unsigned int length, RuleList* rules,int maxl,unsigned int split) const {
//	#if VERBOSE > 2
//		std::cout << "RuleNode::findFailing([" << str(rule,length) << "]," << rules << ") " << this << std::endl; 
//	#endif
		if (rules != NULL) rules->push_back(const_cast<RuleNode*>(this));
		return true;
	}

	virtual inline int countFailing(const int* rule, unsigned int length, int maxl) const {
		return 1;
	}
	
	virtual void listRules(RuleList* rules) const {
		if (rules != NULL) rules->push_back(const_cast<RuleNode*>(this));
	}
	
	virtual bool removeRule(const int* rule, unsigned int length) { return length==0; }
	
	virtual const RuleNode* firstRule() const { return this; }
	virtual RuleNode* firstRule() { return this; }
	
	virtual const RuleNode* nextRule(const int* rule, unsigned int length) const {
		return this;
	}

	virtual void insertNode(const int* rule, unsigned int length, RuleNode* node) {
		// This function should never be called!
		assert(false);
	}
	
  public:	/* Rule operations */
  
    virtual inline void getRule(std::vector<int>& rule, unsigned int split=0) const {
		return FTVNode::getRule(rule,split);
	}
	
	virtual inline const FTRuleTrie& getRoot() const { return FTVNode::getRoot(); }
	virtual inline FTRuleTrie& getRoot() { return FTVNode::getRoot(); }	
  
	virtual const RuleNode* next() const;
	virtual RuleNode* next();	
	
	virtual inline unsigned int length() const { return getParent().length(); }
	
	virtual void unlink();
	
	bool verify(const int* example, unsigned int length) {
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
		// We processed entire rule and reached this point: so far rule is subset of cc(e) -> fail
		return false;
	}
	

	
	virtual void moveTo(FTRuleTrie& newtrie);
		
	virtual inline bool hasFRecord() { return frecord_ != NULL; }
	virtual FRecord& getFRecord() { assert(frecord_); return *frecord_; }
	virtual void setFRecord(FRecord* frecord) { frecord_ = frecord; }
	virtual void clearFRecord();
	
	virtual inline bool hasRRecord() { return rrecord_ != NULL; }
	virtual void initRRecord();
	virtual RRecord& getRRecord() { 
		if (!hasRRecord()) initRRecord();
		return *rrecord_;
	}
	
	inline ExampleList& getExamples() { return *examples_; }
	inline bool hasExamples() { return (examples_ != NULL); }
	void clearExamples();
	void initExamples();
	
	bool isFailing();
	
	std::string toString();

	
};

class FRecord {
  public:
	std::vector<int> refinements;		
	std::map<int,RuleNode*> rules;
	bool failing;
	int refined;
	FRecord(bool f=false) : failing(f), refined(0) {}
};
	
class RRecord {

  public:
	typedef std::vector<RRecord*> RRecordList;
	typedef std::pair<RRecord*,RRecord*> RRecordPair;
	typedef std::vector<RRecordPair> RRecordPairList;
  
  private:
	
  
	RRecordPairList rparents_;
	RRecordList rchildren_;
	RuleNode* rule_;
	
  public:
	RRecord(RuleNode* rule) : rule_(rule) {}
	
	~RRecord();
  
	inline RRecordList& getChildren() { return rchildren_; }
		
	inline bool hasParents() { return !rparents_.empty(); }	
	inline RRecordPairList& getParents() { return rparents_; }
		
	inline RuleNode* getRule() { return rule_; }
	
	inline RRecord* getOtherParent(RRecord* parent) {
		for (RRecordPairList::iterator iter = getParents().begin(); iter!=getParents().end(); ++iter) {
			if (iter->first == parent) {
				return iter->second;
			} else if (iter->second == parent) {
				return iter->first;
			}
		}
		assert(false);
		return NULL;
	}
	
	
  protected:
	inline void addChild(RRecord* child) { getChildren().push_back(child); }

	inline void addParents(RRecord* parent1, RRecord* parent2) { 
		getParents().push_back(RRecordPair(parent1,parent2));
	}
	
	inline bool removeChild(RRecord* child) {
		for (RRecordList::iterator iter = getChildren().begin(); iter!=getChildren().end(); ++iter) {
			
			if (*iter == child) {
				getChildren().erase(iter);
				return true;
			}
		}
		return false;
	}
	
	bool removeParent(RRecord* parent);	
	

  public:
	/* 
	 Notifies the current rule that the given parent rule is no longer valid, and no longer causes this rule to be redundant.
	 The function will update the parent pair that contains the given rule, removing the current rule as child of both parents.
	 Return true if no more parents exist (i.e. the rule is no longer redundant).
	 */
	bool unsetRedundant(RuleNode* rule) {
		for (RRecordPairList::iterator iter = getParents().begin(); iter != getParents().end(); ++iter) {
			if (iter->first->getRule() == rule || iter->second->getRule() == rule) {
				iter->first->removeChild(this);
				iter->second->removeChild(this);
				getParents().erase(iter);
				break;
			}
		}
		return (getParents().empty() || !hasNonRedundantParent(this));
	}
	
	void setRedundant(RuleNode* rule1, RuleNode* rule2) {
		rule1->getRRecord().addChild(this);
		rule2->getRRecord().addChild(this);
		addParents(&(rule1->getRRecord()),&(rule2->getRRecord()));
	}
	
	void listChildren(const std::vector<int>& example, RuleList& frules, RuleList& trules);
	
	void printParentTrace(int depth);
	
	bool hasNonRedundantParent();
	bool hasNonRedundantParent(RRecord* loopcheck);
	
};
	



}
#endif /* RULENODE_H */