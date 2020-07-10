#ifndef NFTVNode_H_
#define NFTVNode_H_

#include <vector>
#include <cassert>
#include "../../lib/profile.h"
#include <set>
#include "NFTTreeNode.h"

namespace cnf
{ 

class NFTRuleTrie;
//class NFTTreeNode;
class NFTRuleTrieProfiler;
class NFTANode;

typedef std::map<unsigned int, NFTANode*> NFTNodeMap;

class NFTVNode
{
	
	friend class NFTRuleTrieProfiler;
	
private:
	
	NFTTreeNode* parent_;
	
	const bool value_;
	
	NFTNodeMap children_;

protected:
	
	inline NFTANode* getChild(unsigned int value) const {
		assert(value > 0);
		NFTNodeMap::const_iterator child = getChildren().find(value);
		if (child == getChildren().end()) return NULL;
		else return child->second;
	}
	
	inline NFTANode* getChild(unsigned int value) {
		assert(value > 0);
		NFTNodeMap::const_iterator child = getChildren().find(value);
		if (child == getChildren().end()) return NULL;
		else return child->second;
	}
		
	void addChild(NFTANode* child);
	
	NFTANode* addChild(unsigned int value);
	
	inline NFTNodeMap& getChildren() { return children_; }
	inline const NFTNodeMap& getChildren() const { return children_; }
	
	inline NFTTreeNode& getParent() { return *parent_; }
	inline const NFTTreeNode& getParent() const { return *parent_; }
	
	inline bool getValue() const { return value_; }
	
	void clear();
	
	inline bool empty() const { return getChildren().empty(); }
	
	unsigned int length() const;
	
	const NFTTreeNode* next(int value) const;
		
	friend class NFTANode;
	
public:
	NFTVNode(NFTTreeNode& parent, bool value) : parent_(&parent), value_(value) {
		//get_profile().start("NFTVNode CONSTRUCTION");
		//get_profile().stop("NFTVNode CONSTRUCTION");
		
	}
	~NFTVNode();
	
	NFTTreeNode* findRule(const int* rule,unsigned int length) const;
	
	NFTTreeNode& addRule(const int* rule, unsigned int length, int exid);
	
	void getRuleInner(std::vector<int>& rule,unsigned int split=0) const;
//#ifdef NFTANode_DLL	
	const NFTRuleTrie& getRoot() const;
//#endif
	bool findSubrules(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules) const;
	
	bool findSubrule(const int* rule, unsigned int length, int extension) const;
	
	bool findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const;
	
	bool findFailing(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules, int maxl,unsigned int split) const;
	
	void listRules(std::vector<NFTTreeNode*>* rules) const;
	
	int countFailing(const int* rule, unsigned int length, int maxl) const;
	
	bool removeRule(const int* rule, unsigned int length);

	const NFTTreeNode* firstRule() const;
	
	const NFTTreeNode* nextRule(const int* rule, unsigned int length) const; 
	
	bool prune(unsigned int length);
	
	bool prune(const int* rule, unsigned int length);
	
//	void getRedundant(std::set<NFTTreeNode*>& rules) const;


	
			
//#define NFTVNode_MEMALLOC
#ifdef NFTVNode_MEMALLOC
	static NFTVNode* freelist;
	NFTVNode* nextfree;

	inline void* NFTVNode::operator new(size_t sz) {
		//std::cout << "new NFTVNode" << std::endl;
		
		if (freelist) {
			NFTVNode* p = freelist;
			freelist = freelist->nextfree;
			return p;
		} 
		return malloc(sz);
	}

	inline void NFTVNode::operator delete(void* vp) {
		//std::cout << "old NFTVNode" << std::endl;
		NFTVNode* p = (NFTVNode*)vp;
		p->nextfree = freelist;
		freelist = p;
	}
#endif
	
};

} 

#endif /*NFTVNode_H_*/
