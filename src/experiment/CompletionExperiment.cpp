#include "CompletionExperiment.h"

//#include "../ruletrie/RuleTrieProfiler.h"

using namespace cnf;

score_result cnf::CompletionExperiment::train_example(const std::vector<int>& example, int s) {
	return getTheory().process_example(example,s);
}
	

score_result cnf::CompletionExperiment::test_example(const std::vector<int>& example, int s) {
	
	std::vector<int> classes;
	
	getTheory().getLanguage().getElements(classes);
	
	for (std::vector<int>::iterator i = classes.begin(); i != classes.end();) {
		if (*i > CLASS_WIDTH) i = classes.erase(i); else ++i;
	}
			
	// classes now contains all the possible classes as found in the training data
	// class_dist contains the distribution of the training examples over these classes
	
	int classlabel = example.front();
	std::vector<int> unlabeled = example;
	
	std::vector<double> scores;
	std::vector<int> dist;
	scores.push_back(getMetric().calculate(example,getTheory().getRules()));
	dist.push_back(getTheory().getLanguage().getOccurence(classlabel));
	
	for (std::vector<int>::iterator c=classes.begin(); c != classes.end(); c++) {
		if (*c != classlabel) {
			unlabeled.front() = *c;
			scores.push_back(getMetric().calculate(unlabeled,getTheory().getRules()));
			dist.push_back(getTheory().getLanguage().getOccurence(*c));
		}
	}
	
	// class_scores now contains the score for each class for the current test example
	return getScorer().calculate_score(scores,dist);
}

int cnf::CompletionExperiment::getMaxK() const {
	return getTheory().getRules().maxlength();
}

bool cnf::CompletionExperiment::reduce_to(int k) {
	return getTheory().getRules().prune(k);
}

void cnf::CompletionExperiment::print_info() {
	std::cout << "\nPrinting Info:" << std::endl;
    getTheory().printInfo();
//	RuleTrieProfiler::printRuleTrieProfile(getTheory().getRules());
}

void cnf::CompletionExperiment::print_theory_to_file(std::string outfile) {
    
    
    std::ofstream out;
    out.open(outfile);
    
    std::cout << "Outputting Learned theory to output file\t: " << outfile << std::endl;
    
    // In the first line, enter number of rules in the theory
    out << getTheory().getRules().size() << std::endl;
    {
        RuleList rules; getTheory().getRules().listRules(&rules);
        for (unsigned int i=0; i<rules.size() ; ++i) {
            out << rules[i]->toString() << std::endl;
        }
    }
    
    out.close();
    
}

