#ifndef COMPLETIONEXPERIMENT_H_
#define COMPLETIONEXPERIMENT_H_

#include <string>
#include "Experiment.h"

namespace cnf
{

class CompletionExperiment : public Experiment
{
	
private:
	
	RuleLearner* theory_;
		
protected:

	virtual score_result train_example(const std::vector<int>& example, int s);
	
	virtual score_result test_example(const std::vector<int>& example, int s);
	
	virtual inline RuleLearner& getTheory() const { return *theory_; }
	virtual inline void setTheory(RuleLearner* learner) { theory_ = learner; }
	

	virtual bool reduce_to(int k);
			
	virtual int getMaxK() const;
	
	virtual inline int getTheorySize() const {
		return getTheory().getRules().numberOfRules();
	}
	
	virtual bool initialize(std::map<std::string,std::string>& cmd) {
		if (Experiment::initialize(cmd)) {
			setTheory(createLearner());
			return true;
		}
		return false;
	}
	
public:
	CompletionExperiment() : Experiment() {}

	virtual ~CompletionExperiment() { delete theory_; }
	
	
	static int run_exp(int argc, char* argv[]) {
		CompletionExperiment exp;
		if (!exp.init(argc,argv)) { print_usage(); return -1; }
		return exp.run();

	}
	
	static void print_usage() {
		std::cout << "Usage: cnfalgo cc trainfile [options]" << std::endl;
		std::cout << "Options: (options with * require value)" << std::endl;
		Experiment::print_options();
	}
	
	void print_info();
	
    void print_theory_to_file(std::string outfile);
};

}

#endif /*COMPLETIONEXPERIMENT_H_*/
