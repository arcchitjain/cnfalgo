#include "drift.h"

// include standard libraries
#include <map>
#include <string>
#include <cassert>

// include command line parsing
#include "lib/command.h"


// include CNF classes
#include "cnfminer/RuleLearner.h"
#include "filter/AttributeFilter.h"
#include "filereader/ExampleReader.h"
#include "lib/QueueBuffer.h"
#include "math/gammaf.h"
#include <map>

typedef QueueBuffer<std::vector<int> >::buffer_type ExampleBuffer;


double probability(int k1, int k2, int N1, int N2, double a = 1, double b = 1) {

	int l1 = N1 - k1;
	//int l2 = N2 - k2;
	
	double p = 0;
	for (int i=k2; i<=N2; i++) {
		double c = lnchoose(N2,i);
		double b1 =  lnbeta(a+k1+i,b+l1+N2-i);
		double b2 =  lnbeta(a+k1,b+l1);
		
		p += exp(c+b1-b2);
		
		//std::cout << "prob " << k1 << " " << k2 << " (" << i << ") " << c << " " << b1 << " " << b2 << " " << exp(c+b1-b2) << " " << p << std::endl;
	}
	
	if (p > 1.001) std::cerr << "\E[32mPROBABILITY EXCEEDS ONE: " << p << " " << k1 << " " << k2 <<"\E[0m" << std::endl;
//	assert(p <= 1);
	return p;	
}



double min(std::deque<double>& list) {
	double m = 1;
	int pos = -1;
	
	for (unsigned int i = 0; i < list.size(); ++i) 
		if (list[i] < m) {
			pos = i;
			m = list[i];
		}
	
	return m;
	
}

int test_set(const ExampleBuffer& examples, const cnf::RuleLearner& theory, std::map<int,int>& results) {
	int score1 = 0;
	for (ExampleBuffer::const_iterator i = examples.begin(); i != examples.end(); ++i) {
		std::vector<cnf::TreeNode*> rules;
		if (theory.getRules().findFailing(&(*i)[0],static_cast<int>((*i).size()), &rules)) {
			score1++;
		}
		results[static_cast<int>(rules.size())]++;
	}
	return score1;
}

int test_set(const ExampleBuffer& examples, const cnf::RuleLearner& theory, std::vector<int>& results) {
	int score1 = 0;
	for (ExampleBuffer::const_iterator i = examples.begin(); i != examples.end(); ++i) {
		std::vector<cnf::TreeNode*> rules;
		if (theory.getRules().findFailing(&(*i)[0],static_cast<int>((*i).size()), &rules)) {
			score1++;
		}
		results.push_back(static_cast<int>(rules.size()));
	}
	return score1;
}

int test_set(const ExampleBuffer& examples,const cnf::RuleLearner& theory) {
	int score1 = 0;
	for (ExampleBuffer::const_iterator i = examples.begin(); i != examples.end(); ++i) {
		if (theory.getRules().findFailing(&(*i)[0],static_cast<int>((*i).size()), NULL)) {
			score1++;
		}
	}
	return score1;
}

double calculate_probability(const cnf::RuleLearner& theory, const ExampleBuffer& testset1, const ExampleBuffer& testset2, double& mint, double a = 1, double b = 1) {
	// run testset 1
	int score1 = test_set(testset1,theory);
	
	// run testset 2
	int score2 = test_set(testset2,theory);			
	mint = 1;
	return probability(score1,score2,static_cast<int>(testset1.size()),static_cast<int>(testset2.size()));
}

double sum(std::map<int,int>::const_iterator start, std::map<int,int>::const_iterator end) {
	double s = 0;
	for (std::map<int,int>::const_iterator iter = start; start != end; ++start) {
		s += iter->second;
	}
	return s;
}

double calculate_probability_min(const cnf::RuleLearner& theory, const ExampleBuffer& testset1, const ExampleBuffer& testset2, double& mint, double& mink1, double& mink2, double a = 1, double b = 1) {
	std::map<int,int> errors1;
	std::map<int,int> errors2;
	
	//double maxerror1 = 
	test_set(testset1,theory,errors1);
	//double maxerror2 = 
	test_set(testset2,theory,errors2);


/*	for (std::map<int,int>::const_iterator i = errors1.begin(); i != errors1.end(); ++i) {
		std::cerr << " " << i->first << " - " << i->second << std::endl;
	}
	for (std::map<int,int>::const_iterator i = errors2.begin(); i != errors2.end(); ++i) {
		std::cerr << " " << i->first << " - " << i->second << std::endl;
	}
*/



	std::map<int,int>::const_reverse_iterator iter1 = errors1.rbegin();
	std::map<int,int>::const_reverse_iterator iter2 = errors2.rbegin();
	std::map<int,int>::const_reverse_iterator stop1 = errors1.rend();
	std::map<int,int>::const_reverse_iterator stop2 = errors2.rend();
	
	int k1 = 0;
	int k2 = 0;
	double minprob = 1;
	//double mint = -1;
	// run through errors1 and errors2 in order
	while (iter1 != stop1 || iter2 != stop2) {
		int t;
		if (iter1 != stop1 && iter2 != stop2 && iter1->first == iter2->first) {
			k1 += iter1->second;
			k2 += iter2->second;
			t = iter1->first;
			++iter1; ++iter2;
		} else if (iter1!=stop1 && (iter2 == stop2 || iter1->first > iter2->first)) {
			k1 += iter1->second;
			t = iter1->first;
			++iter1;			
		} else {
			k2 += iter2->second;
			t = iter2->first;
			++iter2;
		}
		double p = probability(k1,k2,static_cast<int>(testset1.size()),static_cast<int>(testset2.size()));
		
	//	std::cerr << " PROB " << t << " " << k1 << " " << k2 << " " << p << " " << minprob << std::endl;
		if (p < minprob) {
			minprob = p;
			mint = t;
			mink1 = k1;
			mink2 = k2;
		}
	}
	//std::cerr << " threshold for minimum probability: " << mint << " " << minprob << std::endl;
	return minprob;

}

int run_online(int argc, char* argv[]) {

	// read parameters from command line
	std::map<std::string,std::string> cmd;
	init_cmd(cmd,"k:t:");
	if (!read_command_line(argc,argv,cmd)) {
		std::cerr << "error parsing command line" << std::endl;
		return -1;
	}
	
	int k = read_param_default("k",cmd,5);				// max length of rules (5)
	unsigned int t = read_param_default("t",cmd,100);			// test set size (100)
	std::string filename = "";
	if (!read_param("arg_0",cmd,filename)) {
		std::cerr << "error reading filename from command line" << std::endl;
		return -1;
	}
	// command line handled
	
	
	// initialize learner
	cnf::RuleLearner theory(k);
	theory.setFilter(*(new cnf::AttributeFilter()),false);
	theory.initialize();
	
	// initialize buffer
	cnf::ExampleReader data(filename);
//	std::deque<std::vector<int> > buffer;
	ExampleBuffer buffer;
	std::vector<int> example;
	int s = 0; // example count (training examples)
	
	data.open();
	while(data.readline(example)) {
		// add the example to the buffer
		
		buffer.push_front(example);
		
		
		if (buffer.size() > t) {
			theory.process_example(buffer.back(),++s);
			buffer.pop_back();
			if (s % 500 == 0) std::cerr << " training example " << s << std::endl;
		
			std::vector<int> results;
			test_set(buffer,theory,results);
			
			std::cout << s << " |";		//	print training example number
			for (unsigned int i = 0; i<results.size(); ++i) {
				std::cout << " " << results[i]; 
			}
			std::cout << std::endl; 
			
		} else {
			std::cout << "0 | " << std::endl;
		}
		example.clear();
	}
	data.close();
	return 0; // success
}

int run_drift(int argc, char* argv[]) {
	std::map<std::string,std::string> cmd;
	
	// initialize command line parameters 
//	cmd.insert(std::make_pair<std::string,std::string>("k",""));
//	cmd.insert(std::make_pair<std::string,std::string>("1",""));
//	cmd.insert(std::make_pair<std::string,std::string>("2",""));
//	cmd.insert(std::make_pair<std::string,std::string>("P",""));
//	cmd.insert(std::make_pair<std::string,std::string>("M","NULL"));
//	cmd.insert(std::make_pair<std::string,std::string>("a",""));
//	cmd.insert(std::make_pair<std::string,std::string>("b",""));
	
	init_cmd(cmd,"k:1:2:P:Ma:b:");
	
	// read command line into 'cmd'
	if (!read_command_line(argc,argv,cmd)) {
		std::cerr << "error parsing command line" << std::endl;
		return -1;
	}
	
	// read parameters from 'cmd'
	int k = read_param_default("k",cmd,5);					// max length of rules	(def = 5)
	int testsize1 = read_param_default("1",cmd,100);		// size of test set 1	(def = 100)
	int testsize2 = read_param_default("2",cmd,100);		// size of test set 2	(def = 100)
	double threshold = read_param_default("P",cmd,0.005);	// threshold for drift detection (def = 0.5%)
	bool minimum = read_param("M",cmd);
	double a = read_param_default("a",cmd,1.0);
	double b = read_param_default("b",cmd,1.0);
	
	
	std::string filename = "";
	if (!read_param("arg_0",cmd,filename)) {
		std::cerr << "error reading filename from command line" << std::endl;
		return -1;
	}
	
	
	cnf::RuleLearner theory(k);
	theory.setFilter(*(new cnf::AttributeFilter()),false);
	theory.initialize();
	
	cnf::ExampleReader data(filename);
	std::vector<int> example;
	
	unsigned int sizes[2] =  {testsize1,testsize2};
	QueueBuffer<std::vector<int> > qb(sizes, 2);
	
	ExampleBuffer buffer;
	data.open();
	
	int cd_start = -1;
	int cd_end = -1;
	int cd_min = -1;
	double cd_mint = -1;
	double cd_start_P = 1;
	double cd_min_P = 1;
	double cd_end_P = 1;
	bool cddetect = false;
	std::deque<double> average;
	
	std::vector<int> detected;
	int s = 0;
	while(data.readline(example)) {
		
		// put example in buffer
		std::vector<int> training =qb.push(example);
		
		if (training.size() != 0) {
			// process example from buffer
			s++;
			theory.process_example(training,s);
			
			if (s % 500 == 0) std::cerr << " training example " << s << std::endl;
			
			
			// int k1 = score1;
			// int k2 = score2;
			
			// int l1 = testsize1 - score1;
			// int l2 = testsize2 - score2;
			
			// prob = 			
			if (average.size() >= 0.20 * testsize1) {
				average.pop_back();
			}
			
		//	std::cerr << s + testsize1 << std::endl;
			double prob; 
			double probo = 0; 
			double mint = -1;
			double score1 = 0;
			double score2 = 0;
			if (minimum) {
				prob = calculate_probability_min(theory,qb.get_buffer(1),qb.get_buffer(0),mint,score1,score2,a,b);
				double crap;
				probo = calculate_probability(theory,qb.get_buffer(1),qb.get_buffer(0),crap,a,b);
			} else {
				double crap;
				probo = calculate_probability_min(theory,qb.get_buffer(1),qb.get_buffer(0),crap,score1,score2,a,b);
				
				prob = calculate_probability(theory,qb.get_buffer(1),qb.get_buffer(0),mint,a,b);
			}	
			average.push_front(prob);
			
			
			// DETECT concept drift:
			// 1. P < threshold -> enter cdmode
			// 2. P > threshold (in entire window of length X) -> leave cdmode
			//		+ REPORT example with minimum P (+ start + end + P values) 
			
			if (prob < threshold) {
				if (!cddetect) {
					cddetect = true;
					cd_min_P = prob;
					cd_start_P = prob;
					cd_mint = mint;
					cd_min = s + testsize1;
					cd_start = s + testsize1;
				} else {
					if (prob < cd_min_P) {
						cd_min_P = prob;
						cd_mint = mint;
						cd_min = s + testsize1;
					}
				}
			} else if (cddetect && min(average) >= threshold) {
				cddetect = false;
				cd_end = s + testsize1;
				cd_end_P = prob;
				std::cerr << " ==> CONCEPT DRIFT @ " << cd_min << " (P=" << cd_min_P << ", t = " << cd_mint << ") [" << cd_start << "->" << cd_start_P << ", " << cd_end << "->" << cd_end_P << "]" << std::endl; 
				detected.push_back(cd_min);
			}
			
			double sum = 0;
			for (unsigned int i=0;i<average.size(); ++i) {
				sum += average[i];
			}
			
			//int score1 = 0;
			//int score2 = 0;
			std::cout << s + testsize1 << " " << (double)score1/testsize1 << " " << (double)score2/testsize2 << " " << prob << " " << sum/average.size() << " " << probo << " " << mint << std::endl;
		}
		example.clear();
	}
	if (cddetect) {
		cddetect = false;
		cd_end = s + testsize1;
		cd_end_P = 0.5;
		std::cerr << " ==> CONCEPT DRIFT @ " << cd_min << " (P=" << cd_min_P << ", t = " << cd_mint << ") [" << cd_start << "->" << cd_start_P << ", " << cd_end << "->" << cd_end_P << "]" << std::endl; 
		detected.push_back(cd_min);
	}
	
	for (std::deque<std::vector<int> >::const_iterator i = qb.get_buffer(1).begin(); i != qb.get_buffer(1).end(); ++i) {
		s++;
		theory.process_example(*i,s);
	}
	
	for (std::deque<std::vector<int> >::const_iterator i = qb.get_buffer(0).begin(); i != qb.get_buffer(0).end(); ++i) {
		s++;
		theory.process_example(*i,s);
	}
	
	std::cout << "! langsize " << theory.getLanguage().getSize() << std::endl;
	
	std::cout << "! ";
	for (unsigned int i=0; i<detected.size(); i++) {
		if (i > 0) std::cout << ",";
		std::cout << detected[i];
	}
	std::cout << std::endl;
	
	data.close();
	
	return 0;
}

