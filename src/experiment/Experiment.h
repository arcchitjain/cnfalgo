#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

#include "../filereader/ExampleReader.h"
#include "../cnfminer/RuleLearner.h"

#include "../metric/Metric.h"
#include "score/TieBreaker.h"

#include "../filter/Filter.h"
#include "../language/Language.h"

#include "../lib/command.h"

#include "../lib/defaults.h"

namespace cnf {
	
	class Experiment { 

		private:
			
			int maxk_;
			
			int mink_;
			
			int maxn_;
		
			int bfilter_;
			
			Metric* metric_;
			
			Scorer* scorer_;
			
			Filter* filter_;
			
			//Language* language_;
			
			std::string trainfile_;
			
			std::string testfile_;
			
            std::string outputfile_;
			
			virtual int readBFilter(std::map<std::string,std::string> pars) {
				std::string bfil;
				
				if (read_param("-b","-bfilter",pars,bfil)) {
				
					if (bfil == "off") { 
						return -1;
					} else if (bfil == "all") {
						 return INT_MAX;
					} else if (bfil == "default") {
						return BINARY_WIDTH;
					} else if (bfil == "class") {
						return CLASS_OFFSET*CLASS_WIDTH;
					} else {
						return atoi(bfil.c_str()); 
					}
				} else {
					return BINARY_WIDTH;
				}
			}
			
		protected:
			Experiment() {
				filter_ = NULL;
			}
					
			virtual ~Experiment() {
				delete filter_;
			}
			
			/* Initialization methods */
			virtual bool initialize(std::map<std::string,std::string>& pars);
			
			virtual void prepare_command_map(std::map<std::string,std::string>& pars);
        
			
			/* Get-Set methods */
			virtual inline int getK() const { return maxk_; }
			virtual inline void setK(int k) { maxk_ = k; }
			
			virtual inline int getMinK() const { return (mink_ < 0 ? getMaxK() : mink_ ); }
			virtual inline void setMinK(int k) { mink_ = k; }
						
			virtual inline int getN() const { return maxn_; }
			virtual inline void setN(int n) { maxn_ = n; }
			
			virtual inline Metric& getMetric() const { return *metric_; }
			virtual inline void setMetric(Metric* newmetric) { metric_ = newmetric; }
			
			virtual inline Scorer& getScorer() const { return *scorer_; }
			virtual inline void setScorer(Scorer* newscorer) { scorer_ = newscorer; }
			
			virtual inline Filter& getFilter() const { return *filter_; }
			virtual inline void setFilter(Filter* f) { filter_ = f; }
			
			virtual inline int getBFilter() const { return bfilter_; }
			virtual inline void setBFilter(int b) { bfilter_ = b; }
			
			virtual inline const std::string& getTestfile() const { return testfile_; }
			virtual inline void setTestfile(const std::string& testfile) { testfile_ = testfile; }
			virtual inline bool hasTestfile() const { return testfile_ != ""; }
			
			virtual inline const std::string& getTrainfile() const { return trainfile_; }
			virtual inline void setTrainfile(const std::string& trainfile) { trainfile_ = trainfile; }
        
            virtual const std::string& getOutfile() const { return outputfile_; }
            virtual void setOutfile(const std::string& outfile) { outputfile_ = outfile; }
            virtual inline bool hasOutfile() const { return outputfile_ != ""; }
					
			virtual int getMaxK() const = 0;		
					
			/* Creation methods */ 
			virtual RuleLearner* createLearner() const;
			
			virtual ExampleReader* createReader(const std::string& filename) const;
			
			virtual Language& createLanguage() const;
			
			virtual Metric* createMetric(const std::string& metric, bool detailed = false);
			
			static Scorer* createScorer(const std::string& tiebreak);
			
			
			
			virtual inline int getBinFilter() const { return bfilter_; }
			
			/* Virtual methods */
			
			virtual score_result train_example(const std::vector<int>& example, int s) = 0;
			
			virtual score_result test_example(const std::vector<int>& example, int s) = 0;
					
			virtual bool reduce_to(int k) = 0;
			
			virtual int getTheorySize() const = 0;
					
		public:
			
			virtual int run();
								
			virtual score_result test();
			
			virtual score_result train();
			
			static void print_options();
			
			bool init(int argc, char* argv[]) {
				std::map<std::string,std::string> cmd;
				prepare_command_map(cmd);
				return (read_command_line(argc,argv,cmd) && initialize(cmd)); 
			}
			
			virtual void print_info() {}
        
            virtual void print_theory_to_file(std::string outFile) {};
	};

}

#endif /*EXPERIMENT_H_*/
