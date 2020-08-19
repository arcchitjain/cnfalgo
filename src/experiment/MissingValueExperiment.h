#ifndef MISSINGVALUEEXPERIMENT_H_
#define MISSINGVALUEEXPERIMENT_H_

#include <string>
#include "CompletionExperiment.h"


namespace cnf
{

class MissingValueExperiment : public CompletionExperiment
{
	
private:
	
	std::string delfilename_;
	std::string outfilename_;
	int binattvalue(int att) { return BINARY_START + 2*att + 1; }
		
protected:

	virtual score_result test_example(const std::vector<int>& example, int s, const std::vector<int>& deletion, std::string& prediction);
	
	virtual const std::string& getDelfile() const { return delfilename_; }
	virtual void setDelfile(const std::string& delfile) { delfilename_ = delfile; }
	virtual inline bool hasDelfile() const { return delfilename_ != ""; }
			
	virtual const std::string& getOutfile() const { return outfilename_; }
    virtual void setOutfile(const std::string& outfile) { outfilename_ = outfile; }
    virtual inline bool hasOutfile() const { return outfilename_ != ""; }
    
	virtual void prepare_command_map(std::map<std::string,std::string>& cmd) {
		CompletionExperiment::prepare_command_map(cmd);
		cmd.insert(std::make_pair("d",""));
        cmd.insert(std::make_pair("-deletions",""));
        cmd.insert(std::make_pair("o",""));
        cmd.insert(std::make_pair("-output",""));
	}
			
	virtual bool initialize(std::map<std::string,std::string>& cmd) {
		if (CompletionExperiment::initialize(cmd)) {
			setDelfile(read_param_default("d","-deletions",cmd,""));
            setOutfile(read_param_default("o","-output",cmd,""));
			return true;
		} else {
			return false;
		}
	}
	
	virtual bool findAttributeValues(int att,std::vector<int>& values);
	
	virtual int findValue(const std::vector<int>& example, std::vector<int>& values);
	
	virtual int findValue(const std::vector<int>& example, int value);
	
	virtual bool scoreExample(std::vector<int>& example, const int* del, int dell, std::vector<double>& scores, int dist, std::vector<int>& dists, double correct, double& best_score, std::string& prediction);

public:
	MissingValueExperiment() : CompletionExperiment() {}

	virtual ~MissingValueExperiment() {}
	
	virtual score_result test();
		
	static int run_exp(int argc, char* argv[]) {
		MissingValueExperiment exp;
		if (!exp.init(argc,argv)) { print_usage(); return -1; }
		return exp.run();
		
	}
	
	static void print_usage() {
		std::cout << "Usage: cnfalgo mv trainfile [options]" << std::endl;
		std::cout << "Options: (options with * require value)" << std::endl;
		Experiment::print_options();
        std::cout << "-d --deletions * file containing the deletions" << std::endl;
        std::cout << "-o --output    * output file containing the predictions" << std::endl;
	}
	

};



}

#endif /*MISSINGVALUEEXPERIMENT_H_*/
