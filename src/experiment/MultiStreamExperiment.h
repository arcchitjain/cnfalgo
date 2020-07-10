#ifndef MULTISTREAM_H_
#define MULTISTREAM_H_

#include "../cnfminer/RuleLearner.h"
#include "../metric/Metric.h"
#include "score/TieBreaker.h"

#include "Experiment.h"

#include <map>

namespace cnf {

	typedef std::map<int,RuleLearner*> theorymap;

	class MultiStreamExperiment : public Experiment {

		private:
	
			theorymap theory_;
			
			// common language for the different theories
			Language* language_;
			
		protected:
		
			virtual score_result train_example(const std::vector<int>& example, int s);
	
			virtual score_result test_example(const std::vector<int>& example, int s);
			
			inline const theorymap& getTheory() const { return theory_; }	
			
			
			Language& createLanguage() const {
				if (language_ == NULL) {
					return Experiment::createLanguage();
				} else {			
					return *language_;
				}
			}
			
			inline RuleLearner& getTheory(int classlabel) {
					theorymap::const_iterator t = getTheory().find(classlabel);
					
					if (t == getTheory().end()) {
						theory_.insert(std::make_pair(classlabel,createLearner()));
						t = getTheory().find(classlabel);
					}
				
					return *(t->second);
			}
			
			virtual bool reduce_to(int k);
			
			virtual int getMaxK() const {
				int max = 0;
				for (theorymap::const_iterator i = getTheory().begin(); i != getTheory().end(); i++) {
					int tk = i->second->getRules().maxlength();
					if (tk > max) max = tk;
				}
				return max;
			}
			
			virtual int getTheorySize() const {
				int total = 0;
				for (theorymap::const_iterator i = getTheory().begin(); i != getTheory().end(); i++) {
					total += i->second->getRules().numberOfRules();
				}
				return total;
			}
			
		protected :
		
			virtual void prepare_command_map(std::map<std::string,std::string>& cmd) {
				Experiment::prepare_command_map(cmd);
				cmd.insert(std::make_pair("-separatelanguage","NULL"));	
			}
			
			virtual bool initialize(std::map<std::string,std::string>& cmd) {
				if (Experiment::initialize(cmd)) {				
					if (read_param("-separatelanguage",cmd)) {
						language_ = NULL;
					} else {
						language_ = &Experiment::createLanguage();
					}
					return true;
				} else {
					return false;
				}
			}
					
		public:
		
			MultiStreamExperiment() : Experiment() {}	
			
			virtual ~MultiStreamExperiment() {
				theorymap::iterator t = theory_.begin();
				
				while (t != theory_.end()) {
					delete (*t).second;
					theory_.erase(t++);
				}
			}

			
			static int run_exp(int argc, char* argv[]) {
				MultiStreamExperiment exp;
				if (!exp.init(argc,argv)) { print_usage(); return -1; }
				return exp.run();
			}
			
			static void print_usage() {
				std::cout << "Usage: cnfalgo ms trainfile [options]" << std::endl;
				std::cout << "Options: (options with * require value)" << std::endl;
				Experiment::print_options();
				std::cout << "   --separatelanguage" << std::endl;
				std::cout << "                different theories use different language (default off)" << std::endl;	
			}
							
	};
}

#endif /*MULTISTREAM_H_*/
