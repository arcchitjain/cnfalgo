/*
 *  FTVNode.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 24/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#ifndef FTVNODE_H_
#define FTVNODE_H_

#include <vector>
#include <cassert>
#include <map>
#include "../../lib/profile.h"
#include "../../lib/functions.h"
#include <set>
//#include "FTRuleTrie.h"
#include "FTANode.h"
#include "ExampleStore.h"
#include "RuleList.h"

namespace cnf {
	
class FTRuleTrie;

class RuleNode;

typedef std::map<unsigned int, FTANode*> FTNodeMap;
	
typedef std::vector<Example*> ExampleList;	

class FTVNode {
		
  private:
  
	FTNodeMap children_;
		
	FTANode* parent_;
	
	bool value_;
  
  protected:
	friend class FTANode;
	
	friend class FTRuleTrie;
	
	FTVNode(FTANode& parent, bool value) : parent_(&parent), value_(value) {
#if VERBOSE_MEM > 1
		std::cout << "FTVNode(" << &parent << "," << value << ") " << this << std::endl;
#endif
	}
	
	~FTVNode();
	
	virtual inline FTANode* getChild(unsigned int value) const {
		assert(value > 0);
		FTNodeMap::const_iterator child = getChildren().find(value);
		if (child == getChildren().end()) return NULL;
		else return child->second;
	}
	
	virtual inline FTANode* getChild(unsigned int value) {
		assert(value > 0);
		FTNodeMap::const_iterator child = getChildren().find(value);
		if (child == getChildren().end()) return NULL;
		else return child->second;
	}
	
	virtual void removeChild(unsigned int value) {
		FTNodeMap::iterator child = getChildren().find(value);
		if (child != getChildren().end()) {
			getChildren().erase(child);
			if (empty()) getParent().removeChild(getValue());
		}
	}
	
	virtual inline FTNodeMap& getChildren() { return children_; }
	virtual inline const FTNodeMap& getChildren() const { return children_; }
	
	virtual inline bool hasParent() { return parent_ != NULL; }
	virtual inline FTANode& getParent() { assert(parent_ != NULL); return *parent_; }
	virtual inline const FTANode& getParent() const { return *parent_; }
	virtual inline void setParent(FTANode* parent) { parent_ = parent; } 
		
	virtual inline bool getValue() const { return value_; }
	
	virtual void clear();
	virtual inline bool empty() const { return getChildren().empty(); }
	
	virtual const RuleNode* next(int value) const;
	virtual RuleNode* next(int value);
	
	virtual void addChild(FTANode* child);
	virtual FTANode* addChild(unsigned int value);
	
	virtual const RuleNode* findRule(const int* rule,unsigned int length) const;
	virtual RuleNode* findRule(const int* rule,unsigned int length);
	
	virtual RuleNode& addRule(const int* rule, unsigned int length);
	
	virtual void getRule(std::vector<int>& rule,unsigned int split=0) const;
	
	virtual const FTRuleTrie& getRoot() const;
	virtual FTRuleTrie& getRoot();

	virtual bool findSubrules(const int* rule, unsigned int length, RuleList* rules) const;
	
	virtual bool findSubrule(const int* rule, unsigned int length, const int* mc, unsigned int mcl, RuleList* rules) const;
	
	virtual bool findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const;
	
	virtual bool findFailing(const int* rule, unsigned int length, RuleList* rules, int maxl,unsigned int split) const;
	
	virtual void listRules(RuleList* rules) const;
	
	virtual int countFailing(const int* rule, unsigned int length, int maxl) const;
	
	virtual bool removeRule(const int* rule, unsigned int length);
		
	virtual const RuleNode* firstRule() const;

	virtual RuleNode* firstRule();
	
	virtual unsigned int length() const;
		
	virtual void insertNode(const int* rule, unsigned int length, FTVNode* node);

	virtual const RuleNode* nextRule(const int* rule, unsigned int length) const; 
	
	virtual bool prune(unsigned int length);
	
	virtual bool prune(const int* rule, unsigned int length) ;
	
};
	
} 

#endif /*VNODE_H_*/
