#ifndef WINDOWEDRULELEARNER_H_
#define WINDOWEDRULELEARNER_H_

#include "RuleLearner.h"

#include "../exampletrie/ExampleTrie.h"

namespace cnf
{

class WindowedRuleLearner : public cnf::RuleLearner
{
public:
	WindowedRuleLearner(int k, int w);
	virtual ~WindowedRuleLearner();
	
	virtual inline int getWindowSize() { return windowsize_; }
	
	virtual inline int getLowerBound(int n) { 
		if (getWindowSize() < 0 || getWindowSize() > n) return -1;
		else return n - getWindowSize();
	} 
	
	virtual double process_example(const std::vector<int>& example, int ex_id=0);
	
protected :
	
	virtual inline ExampleTrie& getExamples() const { return (ExampleTrie&)getLanguage(); }
	
	virtual double add_example(const std::vector<int>& example, int ex_id);
	
	virtual double remove_example(int ex_id);
	
private:

	int windowsize_;

};

}

#endif /*WINDOWEDRULELEARNER_H_*/
