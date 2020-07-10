#ifndef NFTTreeNode_H_
#define NFTTreeNode_H_

#include <vector>
#include <string>
#include <map>
#include "../../lib/message.h"
#include <set>


namespace cnf
{ 

class NFTRuleTrie;



class NFTTreeNode
{
	
private:
	
	int example_; // the example that caused the adding of the rule 
				  // 0 if this node is not the end of a rule
	
protected :
	NFTTreeNode() : example_(0) {}
#ifdef NFTANode_DLL
	NFTVNode* child_;
	virtual NFTTreeNode& getPrev(unsigned int value) const = 0;
	virtual void setPrev(unsigned int value, NFTTreeNode& prev) = 0;
		
	virtual NFTTreeNode& getNext(unsigned int value) const = 0;
	virtual void setNext(unsigned int value, NFTTreeNode& next) = 0;
#endif
	friend class NFTANode;
	friend class NFTRuleTrie;
	friend class NFTVNode;
	
	
protected:
	
	virtual NFTTreeNode* findRule(const int* rule,unsigned int length) const = 0;
	
	virtual NFTTreeNode& addRule(const int* rule, unsigned int length, int ex_id) = 0;
	
	virtual bool removeRule(const int* rule, unsigned int length) = 0;
//#ifdef NFTANode_DLL	
	virtual const NFTRuleTrie& getRoot() const = 0;
//#endif
	virtual bool findSubrule(const int* rule, unsigned int length, int extension) const = 0;
	
	virtual bool findSubrules(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules) const = 0;
	
	virtual bool findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const = 0;
	
	virtual bool findFailing(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules, int maxl,unsigned int split) const = 0;
	
	virtual int countFailing(const int* rule, unsigned int length, int maxl) const = 0;
	
	virtual void getRuleInner(std::vector<int>& rule, unsigned int split=0) const = 0;
	
	virtual const NFTTreeNode* firstRule() const = 0;
	
	virtual const NFTTreeNode* nextRule(const int* rule, unsigned int length) const = 0; 
	
	virtual void listRules(std::vector<NFTTreeNode*>* rules) const = 0;
	
	inline bool isEndOfRule() const { return example_ != 0; }
	inline void clearEndOfRule() { example_ = 0; }
	
	inline void setExampleId(int exid) { example_ = exid; }

	virtual const NFTTreeNode* next(bool value) const = 0;	
		
	virtual bool prune(unsigned int length) = 0;
	
//	virtual void getRedundant(std::set<NFTTreeNode*>& rules) const = 0;
public:



	virtual const NFTTreeNode* next() const = 0;

	inline int getExampleId() const { return example_; }
	
	virtual void getRule(std::vector<int>& rule,unsigned int split=0) const = 0;
		
	inline void quickRemove() { clearEndOfRule(); }
	
	virtual unsigned int length() const = 0;
	
	virtual ~NFTTreeNode() {}
	
};

}

#endif /*NFTTreeNode_H_*/
