#ifndef ROOTNODE_H_
#define ROOTNODE_H_

#include "NFTTreeNode.h"

namespace cnf {


class NFTANode;
class NFTVNode;

class RuleList : public std::vector<NFTTreeNode*> {};

class NFTRuleTrie : public NFTTreeNode 
{
	friend class NFTRuleTrieProfiler;
	
public:
	typedef std::vector<int> Rule;
	
private:
	NFTVNode* child_;
	
#ifdef NFTANode_DLL		
	NodeMap first_;
	
	NodeMap last_;
	
	
	
	inline NodeMap& getFirstMap() { return first_; }
	inline NodeMap& getLastMap() { return last_; }
	
	inline const NodeMap& getFirstMap() const { return first_; }
	inline const NodeMap& getLastMap() const { return last_; }
#endif
private:
	
	std::vector<unsigned int> rulelength;
			
	void addRuleLength(unsigned int length) {
	//	if (length == 0) cout << " adding rule of length 0 " << endl;
		while (rulelength.size() <= length) {
			rulelength.push_back(0);	
		}
		rulelength[length]++;	
	}
	
	void removeRuleLength(unsigned int length) {
	//	if (length == 0) cout << " removing rule of length 0 " << endl;
		
		if (rulelength.size() <= length || rulelength[length] == 0) {
			print_warning("Removing rule that doesn't exist");
		} else {
			rulelength[length]--;
		}	
	}
	
	bool removeRulesLength(unsigned int length) {
		bool result = false;
		while (rulelength.size()>length+1) {
			if (rulelength.back() > 0) result = true;
			rulelength.pop_back();	
		};
		return result;
	}

	
protected: 
	
	inline bool hasChild() const { return child_ != NULL; }
	
	inline NFTVNode& getChild() const { return *child_; }  

	void deleteChild();
	
	void createChild();

	const NFTTreeNode* next(bool value) const { return NULL; }

	
public:
	NFTRuleTrie() : child_(NULL) { /* blank */ std::cout << "USING NFT RuleTrie" <<std::endl;}
	virtual ~NFTRuleTrie();
#ifdef NFTANode_DLL		
	NFTTreeNode& getFirstNode(unsigned int value) const;
	
	NFTTreeNode& getLastNode(unsigned int value) const;
	
	void addNode(NFTANode& node);
#endif
public:
	
	virtual NFTTreeNode* findRule(const int* rule,unsigned int length) const;
	
	virtual NFTTreeNode& addRule(const int* rule, unsigned int length, int ex_id);
	
	virtual bool extendRule(const std::vector<int>& rule, int extension, int ex_id);
	
	virtual bool removeRule(const int* rule, unsigned int length);
//#ifdef NFTANode_DLL		
	virtual const NFTRuleTrie& getRoot() const;
//#endif
	virtual bool findSubrule(const int* rule, unsigned int length, int extension) const;
	
	virtual bool findSubrules(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules) const;
	
	virtual bool findSubrules(const int* rule, unsigned int length, int mc) const { 
		warning_not_implemented("RootNode::findSubrules(const int*, unsigned int, int mc)",__FILE__,__LINE__);
		return false; 
	}
	
	virtual bool findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const;
	
	virtual bool findFailing(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules, int maxl=-1,unsigned int split=0) const;
	
	virtual int countFailing(const int* rule, unsigned int length, int maxl=-1) const;
	
	virtual void getRule(std::vector<int>& rule,unsigned int split=0) const {}
	
	virtual void getRuleInner(std::vector<int>& rule,unsigned int split=0) const;
	
	virtual void listRules(std::vector<NFTTreeNode*>* rules) const;
	
	virtual const NFTTreeNode* firstRule() const;
	
	virtual const NFTTreeNode* nextRule(const int* rule, unsigned int length) const;
	
	const NFTTreeNode* next() const { return NULL; }
	
	void clear();
	
//	void getRedundant(std::set<NFTTreeNode*>& rules)  const;
	
//	int countRedundant() const;
#ifdef NFTANode_DLL	
	virtual NFTTreeNode& getPrev(unsigned int value) const { return getLastNode(value); }
	virtual void setPrev(unsigned int value, NFTTreeNode& prev) { 
		if (!getLastMap().empty()) getLastMap().erase(getLastMap().begin());
	}

	virtual void setPrev(unsigned int value, NFTANode& prev) { 
		if (!getLastMap().empty()) getLastMap().erase(getLastMap().begin());
		getLastMap().insert(std::make_pair(value,&prev));
	}

	
	virtual NFTTreeNode& getNext(unsigned int value) const { return getFirstNode(value); }
	virtual void setNext(unsigned int value, NFTTreeNode& next) { 
		if (!getFirstMap().empty()) getFirstMap().erase(getFirstMap().begin());
	}

	virtual void setNext(unsigned int value, NFTANode& next) { 
		if (!getFirstMap().empty()) getFirstMap().erase(getFirstMap().begin());
		getFirstMap().insert(std::make_pair(value,&next));
	}
#endif
	virtual inline unsigned int numberOfRules()  const {
		int sum=0;
		for (unsigned int i=0; i<rulelength.size(); i++) {
			sum += rulelength[i];
		}
						
		return sum;
	}
	
	inline unsigned int size() const { return numberOfRules(); }
	
	virtual inline unsigned int numberOfRules(unsigned int length) const {
		if (rulelength.size() <= length) return 0; else return rulelength[length];
	}
	
	
	
	inline unsigned int maxlength() const { 
		if (rulelength.size() == 0) return 0;
		for (unsigned int i=static_cast<int>(rulelength.size())-1; i>0; i--) {
			if (rulelength[i] > 0) return i;
		}
		return 0;
	}
	
	/* 
	 * Removes rules with length equal or longer than the given length.
	*/
	virtual bool prune(unsigned int length);
	
	virtual bool prune(const int* rule, unsigned int length) ;
	
public:
	virtual inline unsigned int length() const { return 0; }
	
public:
	
	class const_iterator {
	
	  private: 
		
		const NFTTreeNode* cn_;
		
		const NFTTreeNode* current() const { return cn_; }
		
	  public:
		const_iterator(const NFTTreeNode* current) : cn_(current) {}
		
		inline void operator++() {
			if (cn_ != NULL) cn_ = cn_->next();
		}
		
		inline void operator++(int) {
			if (cn_ != NULL) cn_ = cn_->next();
		}
		
		inline bool operator==(const_iterator& other) {
			return other.current() == cn_;
		}
		
		inline bool operator!=(const_iterator& other) {
			return other.current() != cn_;
		}
		
		inline const NFTTreeNode* operator->() {
			return cn_;
		}
		
		inline const NFTTreeNode* operator*() {
			return cn_;
		}
			
	};
	
	
	const_iterator begin() const { return const_iterator(firstRule()); } 
	
	const_iterator end() const { return const_iterator(NULL); }
	
};



} 

#endif /*ROOTNODE_H_*/
