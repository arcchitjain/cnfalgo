#ifndef NFTANode_H_
#define NFTANode_H_

#include <map>
#include <vector>
#include <deque>
#include <cassert>
#include "../../lib/profile.h"
#include <set>
#include "NFTTreeNode.h"

namespace cnf { 

class NFTRuleTrie;
class NFTVNode;
class NFTRuleTrieProfiler;

class NFTANode : public NFTTreeNode {
	friend class NFTRuleTrieProfiler;
	
public:
#ifdef NFTANode_DLL
	NFTANode(int val, NFTVNode& p, NFTTreeNode& prev, NFTTreeNode& next);
#else 
	NFTANode(int val, NFTVNode& p);
#endif
	
	virtual ~NFTANode();
	
private:
	
	int flagged_value_;	// the atom for this node 
									// first bit is used to indicate end of rule position
	
	NFTVNode& parent_;	// the parent node for this node
	
#ifdef NFTANode_DLL	
	NFTTreeNode& previous_; // the previous node for the same atom

	NFTTreeNode& next_; // the next node for the same atom
#endif
	NFTVNode* poschild_;	// positive subbranch
	
	NFTVNode* negchild_; 	// negative subbranch
	
	inline static unsigned int uabs(int val) { return val>0 ? val : -val; }
	
	friend class NFTVNode;

	void clear();
	
	bool calc_union(const int* a, int al, const int* b, const int bl, std::vector<int>& result, unsigned int k) const;
	
protected:
	
	inline virtual NFTVNode& getParent() { return parent_; }
	inline virtual const NFTVNode& getParent() const { return parent_; }
	
	inline virtual unsigned int getValue() const { return uabs(flagged_value_); }
	inline virtual void setValue(int value) { flagged_value_ = (flagged_value_ > 0) ? value : -value; }
	
	inline virtual bool getEndOfRulePosition() const { return flagged_value_ > 0; }
	inline virtual void setEndOfRulePosition(bool flag) { flagged_value_ = flag ? getValue() : -getValue(); }
	inline virtual int getEndOfRuleValue() const { return flagged_value_; }
	
	inline virtual bool isEndOfRule() const { return NFTTreeNode::isEndOfRule(); }
	inline virtual bool isEndOfRule(bool branch) const { return (isEndOfRule() && getEndOfRulePosition() == branch); }

	inline virtual bool hasPosChild() const { return poschild_ != NULL; }
	inline virtual bool hasNegChild() const { return negchild_ != NULL; }
	inline virtual bool hasChild(bool cval) const { return (cval ? hasPosChild() : hasNegChild()); }
	
	inline virtual NFTVNode& getPosChild() const { return *poschild_; }
	inline virtual NFTVNode& getNegChild() const { return *negchild_; }
	inline virtual NFTVNode& getChild(bool cval) const { assert(hasChild(cval)); return (cval ? getPosChild() : getNegChild()); }
	 
	inline virtual void removeChild(bool cval) { if (cval) removePosChild(); else removeNegChild(); }
	virtual void removePosChild();
	virtual void removeNegChild();
	
	virtual void addChild(bool value);
	virtual inline void addChild(bool cval, NFTVNode* child) { if (cval) addPosChild(child); else addNegChild(child); }
	virtual inline void addPosChild(NFTVNode* child) { poschild_ = child; }
	virtual inline void addNegChild(NFTVNode* child) { negchild_ = child; }
#ifdef NFTANode_DLL		
	inline virtual NFTTreeNode& getPrev(unsigned int value) const { return previous_; }
	inline virtual void setPrev(unsigned int value, NFTTreeNode& prev) { previous_ = prev; }

	inline virtual NFTTreeNode& getNext(unsigned int value) const { return next_; }
	inline virtual void setNext(unsigned int value, NFTTreeNode& next) { next_ = next; } 
#endif	
	virtual inline bool empty() { return (!isEndOfRule() && !hasPosChild() && !hasNegChild()); }
	
	const NFTTreeNode* next(bool value) const;
		
public:
	const NFTTreeNode* next() const;	
protected:
	virtual NFTTreeNode* findRule(const int* rule,unsigned int length) const;
	
	virtual NFTTreeNode& addRule(const int* rule, unsigned int length, int ex_id);
	
	virtual bool removeRule(const int* rule, unsigned int length);
	
//#ifdef NFTANode_DLL
	virtual const NFTRuleTrie& getRoot() const;
//#endif
	
	virtual bool findSubrule(const int* rule, unsigned int length, int extension) const;
	
	virtual bool findSubrules(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules) const;
	
	virtual bool findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const;
	
	virtual bool findFailing(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules, int maxl,unsigned int split) const;
	
	virtual int countFailing(const int* rule, unsigned int length, int maxl) const;
	
	virtual void getRuleInner(std::vector<int>& rule, unsigned int split=0) const;
	
	virtual void getRule(std::vector<int>& rule, unsigned int split=0) const;
	
	virtual void listRules(std::vector<NFTTreeNode*>* rules) const;
	
	virtual const NFTTreeNode* firstRule() const;
	
	virtual const NFTTreeNode* nextRule(const int* rule, unsigned int length) const; 
	
	virtual bool prune(unsigned int length);
	
	virtual bool prune(const int* rule, unsigned int length);
	
//	virtual void getRedundant(std::set<NFTTreeNode*> rules) const;
	
public:
	virtual unsigned int length() const;

// memory allocation stuff
	
//#define NFTANode_MEMALLOC
#ifdef NFTANode_MEMALLOC
	static NFTANode* freelist;
	NFTANode* nextfree;

	inline void* NFTANode::operator new(size_t sz) {
		
		if (freelist) {
		//	count_operation("Reusing NFTANode memory");
			//stop_profile_timer("Reusing NFTANode memory");
			NFTANode* p = freelist;
			freelist = freelist->nextfree;
			return p;
		} //else {
		//	count_operation("Not reusing NFTANode memory");
			//stop_profile_timer("Not reusing NFTANode memory");
			return malloc(sz);
		//}
	}

	inline void NFTANode::operator delete(void* vp) {
		NFTANode* p = (NFTANode*)vp;
		p->nextfree = freelist;
		freelist = p;
	}
#endif
};



}

#endif /*NFTANode_H_*/
