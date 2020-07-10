/*
 *  FTRuleTrie.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 24/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#ifndef FTRuleTrie_H_
#define FTRuleTrie_H_

#include "../../lib/message.h"
#include "FTVNode.h"
#include "FTANode.h"
#include "RuleNode.h"

namespace cnf {
	
class FTRuleTrie : virtual public FTANode {
		
  public:
		
	typedef std::vector<int> Rule;
		
  private:
		
	std::vector<unsigned int> rulelength;
	
  protected:
    friend class FTVNode;
	friend class RuleNode;
  
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
	
	const int identifier_;
	
  protected: 
		
	inline bool hasChild() const { return hasPosChild(); }
	
	inline FTVNode& getChild() const { return getPosChild(); }  
		
	virtual const RuleNode* next(bool value) const { return NULL; }
		
		
	public:
		FTRuleTrie(int identifier=0)  : FTANode(0), identifier_(identifier) { 
#if VERBOSE_MEM > 1
			std::cout << "FTRuleTrie(" << identifier << ") " << this << std::endl;
#endif			
			/* blank */ 
		}
		virtual ~FTRuleTrie();

	public:
		
	
		
		virtual RuleNode* findRule(const int* rule,unsigned int length) const;
		
		virtual RuleNode& addRule(const int* rule, unsigned int length, int ex_id) {
			return addRule(rule,length);
		}
		
		virtual RuleNode& addRule(const int* rule, unsigned int length);
		
		virtual bool extendRule(const std::vector<int>& rule, int extension);
		
		virtual bool removeRule(const int* rule, unsigned int length);

		virtual const FTRuleTrie& getRoot() const;
		virtual FTRuleTrie& getRoot();

		virtual bool findSubrule(const int* rule, unsigned int length, int extension,RuleList* rules=NULL) const;
		
		virtual bool findSubrule(const int* rule, unsigned int length, const int* mc, unsigned int mcl, RuleList* rules=NULL) const;
		
		virtual bool findSubrules(const int* rule, unsigned int length, RuleList* rules) const;
		
		virtual bool findSubrules(const int* rule, unsigned int length, int mc) const { 
			warning_not_implemented("RootNode::findSubrules(const int*, unsigned int, int mc)",__FILE__,__LINE__);
			return false; 
		}
		
		virtual bool findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const;
		
		virtual bool findFailing(const int* rule, unsigned int length, RuleList* rules, int maxl=-1,unsigned int split=0) const;
		
		virtual int countFailing(const int* rule, unsigned int length, int maxl=-1) const;
		
		virtual void getRule(std::vector<int>& rule, unsigned int split=0) const;
		
		virtual void listRules(RuleList* rules) const;
		
		virtual const RuleNode* firstRule() const;
		
		virtual RuleNode* firstRule();
		
		virtual const RuleNode* nextRule(const int* rule, unsigned int length) const; 

		void clear();
		
		//	void getRedundant(std::set<TreeNode*>& rules)  const;
		
		//	int countRedundant() const;

		virtual inline unsigned int numberOfRules()  const {
			int sum=0;
			for (unsigned int i=0; i<rulelength.size(); i++) {
				sum += rulelength[i];
			}
			
			return sum;
		}

		virtual inline unsigned int numberOfRules(unsigned int length) const {
			if (rulelength.size() <= length) return 0; else return rulelength[length];
		}
	
		inline unsigned int size() const { return numberOfRules(); }
		
		
		
		inline unsigned int maxlength() const { 
			if (rulelength.size() == 0) return 0;
			for (unsigned int i=static_cast<int>(rulelength.size()-1); i>0; i--) {
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
		
		virtual int getIdentifier() const { return identifier_; }
	
		virtual void insertNode(const int* rule, unsigned int length, FTVNode* node);
	
	
		
	public:
		
		class const_iterator {
			
		private: 
			
			const RuleNode* cn_;
			
			const RuleNode* current() const { return cn_; }
			
		public:
			const_iterator(const RuleNode* current) : cn_(current) {}
			
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
			
			inline const RuleNode* operator->() {
				return cn_;
			}
			
			inline const RuleNode* operator*() {
				return cn_;
			}
			
		};
		
		
		const_iterator begin() const { return const_iterator(firstRule()); } 
		
		const_iterator end() const { return const_iterator(NULL); }
		
	};
	
	
	
} 

#endif /*ROOTNODE_H_*/
