/*!  
   \file RuleListIterator.h 

   $Id$
   $URL$
   
   \version   $Rev$
   \author    Anton Dries
   \date      14/02/08
   \namespace cnfalgo
*/
#ifndef cnfalgoRuleListIterator
#define cnfalgoRuleListIterator

#include "../RuleTrie.h"
#include <cassert>

namespace cnf
{

class RuleListIterator : public RuleIterator
{

  private:
	const RuleList& parent_;
	
	Rule* current_;
	
	RuleList::const_iterator current_iter_;
	
	bool hasCurrent() const { return current_ != NULL; }
	
	virtual Rule& current() {
		assert(hasCurrent());
		return *current_;
	}
	
	void initCurrent() { current_ = new Rule(); }

  public:

	RuleListIterator(const RuleList& parent) : parent_(parent), current_(NULL), current_iter_(parent.begin()) { reset(); }

	~RuleListIterator() { delete current_; }
	
	virtual void reset() {
		delete current_;
		current_ = NULL;
		current_iter_ = parent_.begin();
		if (current_iter_ != parent_.end()) {
			initCurrent();
			(*current_iter_)->getRule(current());
		}
	}
	
	virtual bool moveNext() {
		if (!hasCurrent()) {
			return false;
		} else {
			++current_iter_;
		}
		
		if (current_iter_ != parent_.end()) {
			initCurrent();
			(*current_iter_)->getRule(current());
			return true;
		} else {
			delete current_;
			current_ = NULL;
			return false;
		}
	} 
	
	virtual unsigned int size() const {
		return static_cast<int>(parent_.size());
	}
	
	virtual const Rule& current() const {
		assert(hasCurrent());
		return *current_;
	}

};

}  // end namespace cnf

#endif // cnfalgoRuleListIterator
