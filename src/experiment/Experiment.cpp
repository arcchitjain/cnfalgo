#include "Experiment.h"

//#include "../cnfminer/AttributedExtensionFilter.h"
#include <map>

#include "../metric/CountMetric.h"
#include "../metric/LengthMetric.h"
#include "../metric/DetailedMetric.h"

#include "score/SimpleTieBreaker.h"
#include "score/MajorityTieBreaker.h"

#include "../language/SimpleCountedLanguage.h"
#include "../filter/AttributeFilter.h"

#include "../filereader/ExampleReader.h"

#include "../lib/profile.h"


using namespace cnf;

Language& Experiment::createLanguage() const {
	return *new SimpleCountedLanguage();	
}

RuleLearner* Experiment::createLearner() const {
	RuleLearner* result = new RuleLearner(getK());
	
	result->setLanguage(createLanguage());
	result->setFilter(getFilter());
	
	result->initialize();
	return result;
}

ExampleReader* Experiment::createReader(const std::string& filename) const {
	return new ExampleReader(filename,getBFilter());
}

score_result Experiment::train() {
	ExampleReader& data = *createReader(getTrainfile());

	std::string outfile = "";
	
	data.open();
	
	std::vector<int> example;
	
	score_result result = 0;
	int s=0;
	while ((getN()==0 || s<getN()) && data.readline(example)) {
		//for (int j = 0; j < example.size(); j++) std::cout << example[j] + " "; std::cout << std::endl;
		score_result test_score = 0;
		train_example(example,s);
		example.clear();
		s++;
		
		result += test_score;
	};
	
	data.close();
	delete &data;
	result /= s;
	return result;
}

score_result Experiment::test() {
	ExampleReader& data = *createReader(getTestfile());

	data.open();
	
	std::vector<int> example;
	
	score_result result = 0;
	
	int s=0;
	while (data.readline(example)) {
		result += test_example(example,s);
		example.clear();
		s++;
	};
	
	data.close();
	delete &data;
	
	result /= s; 
	return result;	
}

bool Experiment::initialize(std::map<std::string,std::string>& cmd) {
	
	// First create and prepare a command map for the parameters.
	// map<string,string> cmd;
	// prepare_command_map(cmd);
	
	// command line succesfully parsed
	
	read_param("-maxmem",cmd,LIMIT_MEMORY);
	
	std::string trainfile = "";
	if (!read_param("arg_0",cmd,trainfile)) return false; // no training file given
	setTrainfile(trainfile);
	
	setK(read_param_default("k",cmd,DEFAULT_K));
	
	setN(read_param_default("n","-number",cmd,0));
		
	if (read_param("f","-afilter",cmd)) {
		setFilter(new AttributeFilter());
	} else {
		setFilter(new Filter());
	}
	
	setTestfile(read_param_default("t","-testdata",cmd,""));	
	
	{
		Metric* m = createMetric(read_param_default("m","-metric",cmd,DEFAULT_METRIC),read_param("-detailed",cmd));
		if (m == NULL) return false;
		setMetric(m);
	}
	
	{
		std::string scorer = "major";	// default scorer
		Scorer* tb = createScorer(read_param_default("s","-score",cmd,DEFAULT_SCORER));
		if (tb == NULL) return false;
		setScorer(tb);
	}
		
	setBFilter(readBFilter(cmd));
	
	setMinK(read_param_default("r","-reduce",cmd,-1));
	
	//bool tryall = read_param("a","-all",cmd);
	
	return true;
}
			
void Experiment::prepare_command_map(std::map<std::string,std::string>& pars) {
	pars.insert(std::make_pair("k",""));				// k = max length of clauses
	pars.insert(std::make_pair("t",""));				// test file
	pars.insert(std::make_pair("-testdata",""));
	pars.insert(std::make_pair("b",""));				// binary filter range 
	pars.insert(std::make_pair("-bfilter",""));
	pars.insert(std::make_pair("f","NULL"));			// use attribute filter
	pars.insert(std::make_pair("-filter","NULL"));
	//pars.insert(std::make_pair("v",""));			// verbosity				
	//pars.insert(std::make_pair("-verbose",""));
	pars.insert(std::make_pair("m",""));				// specify metric
	pars.insert(std::make_pair("-metric",""));		
	pars.insert(std::make_pair("s",""));				// specify tiebreaker
	pars.insert(std::make_pair("-score",""));		
	pars.insert(std::make_pair("n",""));				// max number of training examples
	pars.insert(std::make_pair("-number",""));
	//pars.insert(std::make_pair("l","NULL"));			// create log
	//pars.insert(std::make_pair("-log","NULL"));
	pars.insert(std::make_pair("r",""));				// also calculate theory down to k=r
	pars.insert(std::make_pair("-reduce",""));
	//pars.insert(std::make_pair("a","NULL"));			// try out all metrics and tiebreakers
	//pars.insert(std::make_pair("-all","NULL"));
	pars.insert(std::make_pair("-detailed","NULL")); // output detailed scoring information
	pars.insert(std::make_pair("-maxmem",""));
}

Metric* Experiment::createMetric(const std::string& metric,bool detailed) {
	Metric* result;
	
	if (metric == "length") 
		result = new LengthMetric(); 
	else if (metric == "length_abs") 
		result = new LengthMetric(true); 
	else if (metric == "count")
		result = new CountMetric();
	else if (metric == "count_abs")
		result = new CountMetric(true);
	else { 
		std::cout << "Invalid metric: choose count, count_abs, length or length_abs" << std::endl;
		return NULL;
	}
	
	if (result != NULL && detailed) 
		return new DetailedMetric(result);
	else
		return result;
		
}

TieBreaker* Experiment::createScorer(const std::string& tiebreak) {
	if (tiebreak == "random")
		return new SimpleTieBreaker(); 
	else if (tiebreak == "major")
		return new MajorityTieBreaker();
	else { 
		std::cout << "Invalid scoring mechanism: choose random or major" << std::endl;
		return NULL;
	}
}

int Experiment::run() {
	// run training phase
	score_result tr_err = train();
	
	std::cout << "\nTime Usage: " << timeUsage() << std::endl;
    std::cout << "MaxK: " << getMaxK() << std::endl;
    std::cout << "Training Result: Standard " << tr_err.standard << ", NoYes " << tr_err.noyes << ", YesNo " << tr_err.yesno << std::endl;
    std::cout << "Theory Size: " << getTheorySize() << std::endl;
	
	if (hasTestfile()) {
		
		// if mink_ == -1 -> getMinK() == getMaxK() and loop execute once
		for (int kv=getMaxK(); kv >= getMinK(); kv--) {
			reduce_to(kv);	// remove all rules of length > kv
			score_result ts_err = test();
			std::cout << "RESULT: " << kv <<  " " << getMetric().getName() << " " << getScorer().getName() << ", Standard " << ts_err.standard << ", NoYes " << ts_err.noyes << ", YesNo " << ts_err.yesno << std::endl;
		}
	}
	
	print_info();
	
	
	profile::print_profile();
	
	return 0;
}


void Experiment::print_options() {
	std::cout << "-k            * maximal length of rules, 0 for no limit (default " << DEFAULT_K << ")" << std::endl;
	std::cout << "-n --number   * use only first n examples from training set (default 0 = all)" << std::endl;
	std::cout << "-t --testdata * filename of the test dataset (default none)" << std::endl;
	std::cout << "-f --afilter    use an attribute filter (values " << ATTR_WIDTH << "*att + value) (default off)" << std::endl;
	std::cout << "-b --bfilter  * use a filter for binary values, remove even values < threshold" << std::endl;
	std::cout << "        values: off     ==> threshold = -1 (no filtering)" << std::endl;
	std::cout << "                default ==> threshold = " << BINARY_WIDTH << " (default)" << std::endl;
	std::cout << "                class   ==> threshold = " << CLASS_WIDTH * CLASS_OFFSET << " (preserve class)" << std::endl;
	std::cout << "                all     ==> threshold = " << INT_MAX << " (filter all even values)" << std::endl;	     
	std::cout << "-m --metric   * define the metric to use" << std::endl;
	std::cout << "        values: length  ==> use length metric (default)" << std::endl;
	std::cout << "                count   ==> use count metric" << std::endl;
	std::cout << "        both metrics can be used with the suffix _abs for absolute version" << std::endl;
	std::cout << "   --detailed   make metrics output detailed information" << std::endl;
	std::cout << "-s --score    * define scoring mechanism" << std::endl;
	std::cout << "        values: major   ==> use majority value (default)" << std::endl;
	std::cout << "                random  ==> split ties evenly over values" << std::endl;
//	std::cout << "        both scorers can be use with suffix _yn and _ny for 0/1 output" << std::endl;
	std::cout << "-r --reduce   * also output results for theories with lower k (default -1 = no)" << std::endl;
	std::cout << "        e.g.: -k 10 -r 5 outputs results for k = 10, 9, 8, 7, 6, 5" << std::endl; 
}
