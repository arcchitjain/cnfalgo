/*!  
   \file RuleVectorIterator.h 

   $Id$
   $URL$
   
   \version   $Rev$
   \author    Anton Dries
   \date      14/02/08
   \namespace cnfalgo
*/
#ifndef cnfalgoRuleVectorIterator
#define cnfalgoRuleVectorIterator

#include "RuleIterator.h"
#include <cassert>
namespace cnf
{

class RuleVectorIterator : public RuleIterator {

  private:
	const std::vector<Rule>& parent_;
	
	std::vector<Rule>::const_iterator current_iter_;

  public:

	RuleVectorIterator(const std::vector<Rule>& parent) : parent_(parent), current_iter_(parent.begin()) {}

	~RuleVectorIterator() {}
	
	virtual void reset() {
		current_iter_ = parent_.begin();
	}
	
	virtual bool moveNext() {
		if (current_iter_ != parent_.end()) {
			++current_iter_;
		}
		return (current_iter_ != parent_.end());
	}
	
	virtual const Rule& current() const {
		assert(current_iter_ != parent_.end());
		return *current_iter_;
	}
	
	virtual unsigned int size() const {
		return static_cast<int>(parent_.size());
	}

};

}  // end namespace cnf

#endif // cnfalgoRuleVectorIterator
