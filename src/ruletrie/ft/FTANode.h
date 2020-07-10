/*
 *  FTANode.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 25/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#ifndef FTANODE_H_
#define FTANODE_H_

#include <map>
#include <vector>
#include <deque>
#include <cassert>
#include "../../lib/profile.h"
#include <set>
#include "RuleList.h"

namespace cnf { 
	
class FTRuleTrie;
class FTVNode;
class RuleNode;
	
class FTANode {
		
  protected:
	FTANode(int val) : value_(val), parent_(NULL), poschild_(NULL), negchild_(NULL) {
#if VERBOSE_MEM > 1
		std::cout << "FTANode(" << val << ") " << this << std::endl;
#endif
	}
		
  public:
	FTANode(int val, FTVNode* p);
	virtual ~FTANode();
		
  private:
		
	int value_;	// the atom for this node 
	
	FTVNode* parent_;	// the parent node for this node
	
	FTVNode* poschild_;	// positive subbranch
	
	FTVNode* negchild_; 	// negative subbranch
	
	inline static unsigned int uabs(int val) { return val>0 ? val : -val; }
		
	void clear();
	
  protected:
  
  	friend class FTVNode;

	friend class RuleNode;

	inline virtual bool hasParent() const { return parent_ != NULL; }	
		
	inline virtual FTVNode& getParent() { return *parent_; }
	inline virtual const FTVNode& getParent() const { return *parent_; }
	
	inline virtual unsigned int getValue() const { return value_; }
	inline virtual void setValue(int value) { value_ = value; }
		
	virtual bool hasPosChild() const { return poschild_ != NULL; }
	virtual bool hasNegChild() const { return negchild_ != NULL; }
	virtual bool hasChild(bool cval) const { return (cval ? hasPosChild() : hasNegChild()); }
	
	virtual FTVNode& getPosChild() const { return *poschild_; }
	virtual FTVNode& getNegChild() const { return *negchild_; }
	virtual FTVNode& getChild(bool cval) const { assert(hasChild(cval)); return (cval ? getPosChild() : getNegChild()); }
	
	inline virtual void removeChild(bool cval) { if (cval) removePosChild(); else removeNegChild(); }
	virtual void removePosChild();
	virtual void removeNegChild();
	
	virtual void unlinkChild(bool cval);
		
	virtual void addChild(bool value, bool is_rule);
	virtual void addChild(bool cval, FTVNode* child) { 
//#ifdef VERBOSE
//		std::cout << "FTANode::addChild(" << cval << ", " << child << ") " << this << std::endl;
//#endif	
		if (cval) addPosChild(child); else addNegChild(child); 
	
	}
	virtual void addPosChild(FTVNode* child) { poschild_ = child; }
	virtual void addNegChild(FTVNode* child) { negchild_ = child; }

	virtual inline bool empty() { return (!hasPosChild() && !hasNegChild()); }
		
	virtual const RuleNode* next(bool value) const;
	virtual RuleNode* next(bool value);
		
	virtual unsigned int length() const;

	virtual const RuleNode* findRule(const int* rule,unsigned int length) const;
	virtual RuleNode* findRule(const int* rule,unsigned int length);
		
	virtual RuleNode& addRule(const int* rule, unsigned int length);
		
	virtual bool removeRule(const int* rule, unsigned int length);
		
	virtual const FTRuleTrie& getRoot() const;
	virtual FTRuleTrie& getRoot();
		
	virtual bool findSubrule(const int* rule, unsigned int length, const int* mc, unsigned int mcl, RuleList* rules) const;
	
	virtual bool findSubrules(const int* rule, unsigned int length, RuleList* rules) const;
	
	virtual bool findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const;
	
	virtual bool findFailing(const int* rule, unsigned int length, RuleList* rules, int maxl,unsigned int split) const;
	
	virtual int countFailing(const int* rule, unsigned int length, int maxl) const;
	
	virtual void getRule(std::vector<int>& rule, unsigned int split) const;
	
	virtual void listRules(RuleList* rules) const;
	
	virtual const RuleNode* firstRule() const;
	virtual RuleNode* firstRule();
		
	virtual const RuleNode* nextRule(const int* rule, unsigned int length) const; 
	
	virtual bool prune(unsigned int length);
	
	virtual bool prune(const int* rule, unsigned int length);
	
	virtual void cleanUp();
	
	virtual void insertNode(const int* rule, unsigned int length, FTVNode* node);
	
		
};
	
	
	
}

#endif /*ANode_H_*/
