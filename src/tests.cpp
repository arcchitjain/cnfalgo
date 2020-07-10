#include "tests.h"

#include "lib/command.h"
#include "lib/functions.h"
#include "lib/message.h"
#include "filereader/ExampleReader.h"
#include "filereader/BufferedExampleReader.h"
#include "cnfminer/RuleLearner.h"
#include "cnfminer/WindowedRuleLearner.h"
//#include "cnfminer/AttributedExtensionFilter.h"
#include "filter/AttributeFilter.h"
#include "ruletrie/RuleTrie.h"

#include "filter/Filter.h"
#include "lib/profile.h"
//#include "ruletrie/RuleTrieProfiler.h"
#include "filereader/RuleFileReader.h"
//#include "ruletrie/VNode.h"
//#include "ruletrie/ANode.h"
//#include "ruletrie/Rule.h"
#include "cnfminer/DistBasedDriftFilter.h"
#include "ruletrie/iterator/RuleTrieIterator.h"

#include "metric/CountMetric.h"
#include "metric/LengthMetric.h"
#include "metric/DetailedMetric.h"

#include "metric/KEqMetric.h"

#include "lib/Log.h"
#include "lib/QueueBuffer.h"

#include "math/gammaf.h"
#include "drift.h"

//#include "ruletrie/DynamicContainer.h"

#include "filereader/GenericReader.h"

#include <deque>
#include <set>

cnf::Filter& createFilter(bool bin, bool fil) {
	if (fil) 
		return *new cnf::AttributeFilter();
	else 
		return *new cnf::Filter();
}

cnf::Metric* createMetric(const std::string& metric) {
	
	if (metric == "length") 
		return new cnf::LengthMetric(); 
	else if (metric == "length_abs") 
		return new cnf::LengthMetric(true); 
	else if (metric == "count")
		return new cnf::CountMetric();
	else if (metric == "count_abs")
		return new cnf::CountMetric(true);
	else { 
		return NULL;
	}
	
}


int test_test(int argc, char* argv[]) {
	/*std::vector<unsigned int> a;
	a.push_back(5);
	a.push_back(10);
	QueueBuffer<int> qb(&a[0], a.size());
	
	for (int i=0; i < 25; i++) {
		int s = qb.push(i);
		std::cout << "[";
		std::cout << s << " ";
		std::cout << "] ==> [" ;
		
		for (std::deque<int>::const_iterator i = qb.get_buffer(0).begin(); i != qb.get_buffer(0).end(); ++i) {
			std::cout << *i << " ";
		}
		std::cout << "] -- [" ;
		for (std::deque<int>::const_iterator i = qb.get_buffer(1).begin(); i != qb.get_buffer(1).end(); ++i) {
			std::cout << *i << " ";
		}
		std::cout << "]" << std::endl; 
//		std::cout << std::endl;
	}
	
	return 0;
	*/
	
	
	
	return 0;
}

int main_test(int argc, char* argv[]) {
	try {
		if (argc > 1) {
			std::string select = argv[1];
			shift_array(argc,argv);
			
			if (select == "reader") {
				return test_reader(argc,argv);
			} else if (select == "learner") {
				return test_learner(argc,argv);
			} else if (select == "cd") {
				return run_drift(argc,argv);
			} else if (select == "insert") {
				return test_insert();
			} else if (select == "metric") {
				return test_metric(argc,argv);
			} else if (select == "rulereader") {
				return test_rulereader(argc,argv);
			} else if (select == "ruletrie") {
				return test_ruletrie(argc,argv);	
			} else if (select == "test") {
				return test_test(argc,argv);	

	//		} else if (select == "redundancy") {
	//			return test_redundancy(argc,argv);	
	//		} else if (select == "combination") {
	//			return test_combination(argc,argv);	
			}
		} 
		
		return -1;
	} catch (int e) {
		if (e == 5) std::cerr << "Process aborted : memory limit exceeded !" << std::endl;
		return -e;	
	}	
	
	
}

int test_rulereader(int argc, char* argv[]) {
	std::map<std::string,std::string> pars;
	if (read_command_line(argc,argv,pars)) {
		std::string filename = "";
		if (!read_param("arg_0",pars,filename)) {
			return -1;
		}
		
		cnf::RuleFileReader rf(filename);
		
		rf.open();
		
		std::vector<int> rule;
		while (rf.readline(rule)) {
			std::cout << str(&rule[0],static_cast<int>(rule.size())) << std::endl;
			rule.clear();	
		}
				
		rf.close();
		return 0;
				
	} else {
		return -1;
	}
}

int test_reader(int argc, char* argv[]) {
	std::map<std::string,std::string> pars;
	
	pars.insert(std::make_pair("b","NULL"));
	pars.insert(std::make_pair("-binary","NULL"));
	pars.insert(std::make_pair("f","NULL"));
	pars.insert(std::make_pair("-filter","NULL"));
	pars.insert(std::make_pair("o",""));
	pars.insert(std::make_pair("-offset",""));
	pars.insert(std::make_pair("n",""));
	
	
	if (read_command_line(argc,argv,pars)) {
		bool binary = read_param("b","-binary",pars);
		bool filter = read_param("f","-filter",pars);
		
		
		cnf::Filter *f;
		if (binary)
			if (filter) f = new cnf::AttributeFilter();
			else f = new cnf::Filter();
		else f = new cnf::Filter();
		
		int offset = 0;
		read_param("o","-offset",pars,offset);
		
		int n=0;
		read_param("n",pars,n);
		
		std::string filename = "";
		if (!read_param("arg_0",pars,filename)) {
			return -1;
		}
		
		cnf::ExampleReader data(filename);
		
		data.open();
		
		std::vector<int> example;
		
		int s=0;
		while ((n==0 || s<n) && data.readline(example)) {
			s++;
			
			//if (binary) filter_even(ex,exl);	
			
			std::cout << "data:"; for (unsigned int i=0; i<example.size(); i++) std::cout << " " << example[i]; std::cout << std::endl;
			
			example.clear();
			
		}
		
		data.close();
		
	}
	
	return -1;
}

int test_learner(int argc, char* argv[]) {
	std::map<std::string,std::string> pars;
	
	pars.insert(std::make_pair("n",""));
	pars.insert(std::make_pair("k",""));
	pars.insert(std::make_pair("f","NULL"));
	pars.insert(std::make_pair("-filter","NULL"));
	pars.insert(std::make_pair("t",""));
	pars.insert(std::make_pair("-triefile",""));
	pars.insert(std::make_pair("r",""));
	pars.insert(std::make_pair("-rulefile",""));
	pars.insert(std::make_pair("l",""));
	pars.insert(std::make_pair("-logfile",""));
	pars.insert(std::make_pair("-maxmem",""));
	pars.insert(std::make_pair("W",""));
	pars.insert(std::make_pair("L",""));
	pars.insert(std::make_pair("-lang-size",""));
	
	
	
	if (read_command_line(argc,argv,pars)) {
		bool binary = read_param("b","-binary",pars);
		
		bool filter = read_param("f","-filter",pars);
		
		int n = read_param_default("n","-number",pars,0);
		
		int Ls = read_param_default("L","-lang-size",pars,0);
		
		int k= read_param_default("k",pars,5);
		
		int W = read_param_default("W",pars,-1);
		
		std::string filename = "";
		if (!read_param("arg_0",pars,filename)) {
			return -1;
		}
		
		read_param("-maxmem",pars,cnf::LIMIT_MEMORY);
		
		std::string triefile = "";
		read_param("t","-triefile",pars,triefile);
		
		std::string rulefile = "";
		read_param("r","-rulefile",pars,rulefile);
		
		std::string outfile = read_param_default("l","-logfile",pars,"");
		
		std::ofstream report;
		if (outfile != "") report.open(outfile.c_str(),std::ofstream::out);
		
		cnf::ExampleReader data(filename);
		
		cnf::RuleLearner* theory;
		
		if (W >= 0) 
			theory = new cnf::WindowedRuleLearner(k,W);
		else
			theory = new cnf::RuleLearner(k);
		
		std::cout << "theory created" << std::endl;
		theory->setFilter(createFilter(binary,filter),false);
		std::cout << "filter set" << std::endl;
		
		
		if (Ls > 0) theory->setListener(* new cnf::KEqMetric(k,Ls));
		std::cout << "listener set " << k << ", " << Ls <<  std::endl;
		theory->initialize();
		std::cout << "theory initialized" << std::endl;
		
		data.open();
		
		std::vector<int> ex;
		
		int s=0;
		while ((n==0 || s<n) && data.readline(ex)) {
			s++;
			
			if (!report.is_open()) {
				if (s % 100 == 0) std::cout << "training example " << s << std::endl; 
			}
			
			//cout << "data:"; for (unsigned int i=0; i<ex.size(); i++) cout << " " << ex[i]; cout << endl;
						
			theory->process_example(ex,s);
			
			
			if (report.is_open()) { 
				report << s << " " << theory->getRules().numberOfRules() << " " << timeUsage() << std::endl;
//				report << s << " " << theory->getRules().maxlength() << " "<< theory->getRules().numberOfRules() 
//					<<  " " << timeUsage() << " " << profile::get_count("", PROF_QUICKREMOVE) << " " <<
//					profile::get_count("", PROF_ADDRULE) << " " << memoryUsage() << " " << theory->getLanguage().getSize() << std::endl;
			}
			
	
			ex.clear();
		}
		
		data.close();
		
		double time = timeUsage();
		double mem = memoryUsage();
		
		if (rulefile != "") {
			write(rulefile,theory->getRules(),false);
	//		ofstream f; 
	//		f.open(rulefile.c_str(),ofstream::out);
		
	//		const cnf::TreeNode* r = theory->getRules().firstRule();
		//	int s = 0;
	//		while (r) {
	//			vector<int> rule;
		//		std::cout << "writing rule " << s++ << std::endl;
	//			r->getRule(rule);
	//			f << rule[0]; 
	//			for (unsigned int i=1; i<rule.size();++i) f << ", " << rule[i]; 
	//			f << "! (" << r->getExampleId() << ")" << std::endl;
	//			r = theory->getRules().nextRule(&rule[0], rule.size());
				//rule.clear();
	//		}
			
			//	cnf::rulelist rules;
		//	theory->getRules().findRules(rules);
		//	for (cnf::rulelist::iterator i = rules.begin(); i != rules.end(); i++) {
		//		f << (*i)->toString() << endl;	
		//	}
		//	f.close();
		}


		if (triefile != "") { 
			std::ofstream f; 
			f.open(triefile.c_str(),std::ofstream::out);
			
	//		f << cnf::RuleTrieProfiler::createDotString(theory->getRules());
		//	f << theory->getRules().toDot();	
			
			f.close();
		}
		
		// cout << "number of nodes : " << theory->getRules().countNodes() << endl;
		
		
		
		
		std::cout << "calculating theorysize"  << std::endl;
		
		int L = theory->getLanguage().getSize();
		
	//	std::cout << "theorysize = " << calc_theory_size(theory->getRules(),k,L) <<  std::endl;
		std::cout << "max theorysize given theory = " << max_theory_size(theory->getRules(),k,L) <<  std::endl;
		std::cout << "max theorysize = " << total_theory_size(k,L) <<  std::endl;
				
		std::cout << "langsize = " << theory->getLanguage().getSize() << std::endl;

//		cnf::RuleTrieProfiler::printRuleTrieProfile(theory->getRules());
		
		delete theory;
		
		if (report.is_open()) report.close();
	
	
		profile::print_profile();
		
		std::cout << "time = " << time << std::endl;
		std::cout << "mem = " << mem << std::endl;
		
	//	int test;
	//	std::cin >> test;
		
	}
	
	
	
	return -1;
}


int test_insert() {
	
	int rule[] = { 1, -3, -7, 9 };
	int rulel = 4;
	
	int item = 11;
	
	int* nrule = NULL;
	int nrulel = 0;
	
	insert(item, rule,rulel,nrule,nrulel);
	
	for (int i=0; i<nrulel; i++) std::cout << nrule[i] << " "; std::cout << std::endl;
	
	return 0;
}

int test_metric(int argc, char* argv[]) {
	return 0;
}

void readAndAdd(const std::string& filename, cnf::RuleTrie& rules) {
	cnf::RuleFileReader rf(filename);
	rf.open();
	std::vector<int> rule;
	while(rf.readline(rule)) {
		rules.addRule(&rule[0],static_cast<int>(rule.size()),1);
		rule.clear();
	}
	rf.close();
}

void readAndDelete(const std::string& filename, cnf::RuleTrie& rules) {
	cnf::RuleFileReader rf(filename);
	rf.open();
	std::vector<int> rule;
	while(rf.readline(rule)) {
		rules.removeRule(&rule[0],static_cast<int>(rule.size()));
		rule.clear();
	}
	rf.close();
}

void write(const std::string& filename, const cnf::RuleTrie& rules, bool noExId = true) {
	std::ofstream f; 
	f.open(filename.c_str(),std::ofstream::out);
	
	write(f,rules,noExId);
	
	f.close();
}


void write(std::ostream& f, const cnf::RuleTrie& rules, bool noExId = true) {
	const cnf::TreeNode* r = rules.firstRule();
	
	while (r) {
		std::vector<int> rule;
		r->getRule(rule);
		f << rule[0]; 
		for (unsigned int i=1; i<rule.size();++i) f << ", " << rule[i]; 
		//if (noExId) {
			f << "!" << std::endl;
		//} else {
			//f << "! (" << r->getExampleId() << ")" << std::endl;
		//}
		r = rules.nextRule(&rule[0], static_cast<int>(rule.size()));
	}
}


int test_ruletrie(int argc, char* argv[]) {
	std::map<std::string,std::string> cmd;
	
	// What this does:
	//	test the add, list and delete of rules in the rule structure
	//	1. read files from command line
	//			a) 1 file given
	//			b) 2 files given
	//	2a. read file and add all the rules to the ruletrie
	//	3a. list all the rules and write them to a file <filename>.out
	
	//  2b. read file 1 + 2 and add all the rule to the ruletrie
	//  3b. list all rules and write them to a file <filename2>.out
	//	4b. remove files from file 2 from ruletrie
	//	5b. list all rule in ruletrie and write then to file <filename1>.out 
	
	if (!read_command_line(argc,argv,cmd)) return -1;
		
	
	std::cout << "Testing RuleTrie implementation" << std::endl;
	std::string file1 = "";
	std::string file2 = "";
	if (!read_param("arg_0",cmd,file1)) {
		return -1; // should at least give one parameter
	} else if (!read_param("arg_1",cmd,file2)) {
		// case a
		cnf::RuleTrie rules;
		readAndAdd(file1,rules);
		write(file1 + ".out",rules);
		std::cout << "calculating theorysize " << std::endl;
		std::cout << "theorysize = " << calc_theory_size(rules,3,10) <<  std::endl;	
	} else {
		// case b
		cnf::RuleTrie rules;
		readAndAdd(file1,rules);
		readAndAdd(file2,rules);
		write(file2 + ".out",rules);
		readAndDelete(file2,rules);
		write(file1 + ".out",rules);
		//readAndDelete(file1,rules);
	}
	return 0;
}

void print_sizes() {
//	cnf::RuleTrieProfiler::print_sizes();
}

#include "cnfminer/RuleLearner.h"

// code for running the learner in pipe mode


// pipe in sends data with following form:

// TRAIN <ex_id> <example>		-> trains learner with given example: returns metric value
// TEST <ex_id> <example>		-> tests learner with given example: returns metric value

void tokenize(const std::string& str,
			  std::vector<std::string>& tokens,
			  const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
	
    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


// return type:
//	0:	end of file
//	1:  train
//	2:	test

// data fields:
// [type exid] example		-> first two optional with read_type parameter
int read_pipe(std::vector<int>& example, std::istream& pipe, bool read_type = true) {
	
	std::string line;
	if (!pipe) {
		return -1; // file not open
	} else {
		if (pipe.eof()) {
			return 0; // end-of-file			
		} else {
			int type; // unknown type
			getline(pipe,line);
			
			while (line == "") {
				if (pipe.eof()) {
					return 0;
				}
				getline(pipe,line);
			}	
			
			std::vector<std::string> tokens;
			tokenize(line,tokens);
			
			int s=0;
			if (read_type) {
				s = 1;
				if (tokens[0] == "TRAIN") {
					type = 1;
				} else if (tokens[0] == "TEST") {
					type = 2;
				} else if (tokens[0] == "RULES") {
					type = 3;
				} else if (tokens[0] == "EXIT") {
					type = 0;
					return type;
				} else {
					type = 1;
					s = 0;
				}
				
			}
			
			for (unsigned int i=s; i<tokens.size(); ++i) {
				example.push_back(atoi(tokens[i].c_str()));
			}
			return type;
		}
	}
}


int run_pipe(int argc, char *argv[]) {
	std::map<std::string,std::string> cmd;	
	{
		cmd.insert(std::make_pair("k",""));				// k = max length of clauses
		
		cmd.insert(std::make_pair("f","NULL"));			// use attribute filter
		cmd.insert(std::make_pair("-filter","NULL"));
		
		cmd.insert(std::make_pair("m",""));				// specify metric
		cmd.insert(std::make_pair("-metric",""));		
	}
	
	if (!read_command_line(argc,argv,cmd)) return -1; // error reading command line
	
	
	bool filter = read_param("f","-filter",cmd);
		
	int Ls = read_param_default("L","-lang-size",cmd,0);
		
	int k= read_param_default("k",cmd,5);
	
	std::string metricN = read_param_default("m","-metric",cmd,"");
	
	cnf::Metric* metric = NULL;
	if (metricN != "") {
		metric = createMetric(metricN);
	}
	
				
	std::istream& pipe = std::cin;
	
	std::vector<int> example;
	int pipe_state = read_pipe(example,pipe);
	
	cnf::RuleLearner learner(k);
	
	learner.setFilter(createFilter(false,filter),false);
	if (Ls > 0) learner.setListener(* new cnf::KEqMetric(k,Ls));
	if (metric != NULL) learner.setMetric(*metric,false);
	learner.initialize();
		
	int tr = 0;
	int ts = 0;
	while (pipe_state > 0) {
		if (example.size() > 0) { 
			if (pipe_state == 1) {
				// training example
				tr++;
				double score = learner.process_example(example, tr);
				std::cout << "TRAINED " << tr << " " << score << std::endl;
				//			for (int i=0; i<example.size(); ++i) cout << " " << example[i]; cout << endl;
			} else if (pipe_state == 2) {
				ts++;
				double score = learner.getScore(example);
				std::cout << "TESTED " << ts << " " << score << std::endl;				
			}
		} else if (pipe_state == 3) {
			// output theory
			write(std::cout,learner.getRules(),false);
		}
		example.clear();
		pipe_state = read_pipe(example,pipe);
	}
	
	if (pipe_state != 0) {
		std::cerr << "Error occured while reading from input pipe! (" << (pipe_state) << ")" << std::endl;
	}
	
	return pipe_state;
	
}

int run_measure(int argc, char* argv[]) {
	std::map<std::string,std::string> pars;
	
	pars.insert(std::make_pair("n",""));
	pars.insert(std::make_pair("b",""));
	
	if (read_command_line(argc,argv,pars)) {
		int n = read_param_default("n","-number",pars,0);		
		
		int b = read_param_default("b",pars,cnf::CLASS_WIDTH);
		
		std::string filename = "";
		if (!read_param("arg_0",pars,filename)) {
			return -1;
		}
	
		std::set<int> language;
		cnf::ExampleReader rf(filename,b);
		rf.open();
		std::vector<int> ex;
		int s = 0;
		
		unsigned int max = 0;
		int sum = 0;
        while((rf.readline(ex) && (n == 0)) || s < n) {
			s++;
			
			if (ex.size() > max) max = static_cast<int>(ex.size());
			sum = static_cast<int>(ex.size());
			
			for (unsigned int i=0; i<ex.size(); ++i) {
			//	cout << " " << ex[i];
				language.insert(ex[i]);
			}
			//cout << endl;
			ex.clear();
		}
		rf.close();
		
		std::cout << "lsize = " << language.size() << std::endl;
		
		
		// for (set<int>::iterator i = language.begin(); i != language.end(); ++i) 
		// 	std::cout << *i << " "; 
		// std::cout << endl;

		return 0;
	}
	
	
	return -1;
}

