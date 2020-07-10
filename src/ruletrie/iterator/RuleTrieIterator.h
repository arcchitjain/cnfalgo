/*!  
   \file RuleTrieIterator.h 

   $Id$
   $URL$
   
   \version   $Rev$
   \author    Anton Dries
   \date      14/02/08
   \namespace cnfalgo
*/
#ifndef cnfalgoRuleTrieIterator
#define cnfalgoRuleTrieIterator


#include "../RuleTrie.h"
//#include "../TreeNode.h"
#include <cassert>

namespace cnf
{


class RuleTrieIterator : public RuleIterator {

  private :
	  
	const RuleTrie& parent_;
	
	Rule* current_;
	
	bool hasCurrent() const { return current_ != NULL; }
	

	
	void initCurrent() { current_ = new Rule(); }

  public:

    RuleTrieIterator(const RuleTrie& parent) : parent_(parent), current_(NULL) {}

   ~RuleTrieIterator() { delete current_; current_ = NULL; }
   
	virtual unsigned int size() const { return parent_.numberOfRules(); }

	virtual void reset() { delete current_; current_ = NULL; }
		
	virtual bool moveNext() { 
		const TreeNode* node = NULL;
		if (!hasCurrent()) {
			node = parent_.firstRule();
		} else {
			node = parent_.nextRule(&current()[0], static_cast<int>(current().size()));
		}
		if (node == NULL) { 
			return false; 
		} else {
			if (hasCurrent()) {
				current().clear();
			} else {
				initCurrent();
			}
			node->getRule(current());
			return true;
		}
	}
		
	virtual const Rule& current() const {
		assert(hasCurrent());
		return *current_;
	}
		
	virtual Rule& current() {
		assert(hasCurrent());
		return *current_;
	}


};

}  // end namespace cnf

#endif // cnfalgoRuleTrieIterator
