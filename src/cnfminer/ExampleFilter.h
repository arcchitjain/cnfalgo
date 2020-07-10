#ifndef EXAMPLEFILTER_H_
#define EXAMPLEFILTER_H_

#include <vector>
#include "../exampletrie/ExampleContainer.h"
#include "../ruletrie/RuleTrie.h"

namespace cnf
{

class RuleLearner;
//class RuleList;
//class RuleTrie;
//class ExampleContainer;

class ExampleFilter
{
	
private:
	
	static int metric_value;
	
	ExampleContainer& buffer_;
	
public:	
	ExampleFilter(ExampleContainer& buffer) : buffer_(buffer) {}
	virtual ~ExampleFilter();
	
	virtual bool process_example(RuleLearner& learner, const std::vector<int>& example, const RuleList& rules, 
			RuleTrie& theory, int ex_id);
				
	virtual inline void stream_buffer(int ex_id, RuleLearner& learner) {}
	
protected:
	
	inline ExampleContainer& getBuffer() { return buffer_; }
	
	void bufferExample(const std::vector<int>& example, int exid);
	
};

}

#endif /*EXAMPLEFILTER_H_*/
