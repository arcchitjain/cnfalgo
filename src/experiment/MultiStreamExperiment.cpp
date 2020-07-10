#include "MultiStreamExperiment.h"

using namespace cnf;

score_result MultiStreamExperiment::train_example(const std::vector<int>& example, int s) {
	std::vector<int> unlabeled = example;
	int classlabel = unlabeled.front();
	unlabeled.erase(unlabeled.begin());
	return getTheory(classlabel).process_example(unlabeled,s);
}

score_result MultiStreamExperiment::test_example(const std::vector<int>& example, int s) {
	std::vector<int> unlabeled = example;
	int classlabel = unlabeled.front();
	unlabeled.erase(unlabeled.begin());
	
	std::vector<double> scores;
	std::vector<int> dist;
	
	scores.push_back(getMetric().calculate(unlabeled,getTheory(classlabel).getRules()));
	dist.push_back(getTheory(classlabel).getExampleCount());
	
	for (theorymap::const_iterator t=getTheory().begin(); t != getTheory().end(); ++t) {
		if (t->first != classlabel) {
			scores.push_back(getMetric().calculate(unlabeled,t->second->getRules()));
			dist.push_back(t->second->getExampleCount());
		}
	}
	
	return getScorer().calculate_score(scores, dist);	
}

bool MultiStreamExperiment::reduce_to(int k) {
	
	
	bool result=false;
	
	for (theorymap::const_iterator t = getTheory().begin(); t != getTheory().end(); ++t) {
		result = t->second->getRules().prune(k) || result;	
	}
	return result;
}
