/*
 *  FaultTolerantRuleLearner.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 25/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#include "FaultTolerantRuleLearner.h"

/*
 *  FTRuleLearner.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 24/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#include "FaultTolerantRuleLearner.h"
#include "../lib/profile.h"
#include "../lib/defaults.h"

namespace cnf {
	
	FaultTolerantRuleLearner::~FaultTolerantRuleLearner() {
		//delete(&examples_);
	}
	
	
	void FaultTolerantRuleLearner::filterFailing(const std::vector<int>& example, RuleList& failing)  {
#ifdef FTRULETRIE
		Example* new_example = &getExamples().add(&example[0], static_cast<int>(example.size()));
		
		unsigned int i=0;
		while (i < failing.size()) {
			if (!failing[i]->hasExamples()) failing[i]->initExamples();
			ExampleList& examples = failing[i]->getExamples();
			new_example->increaseCount();
			examples.push_back(new_example);
			if (examples.size() <= getTolerance()) {
				failing.erase(failing.begin()+i);
			} else {
				++i;
			}
		}	
		
		
		/*	
		 RuleList::iterator frule = failing.begin();
		 while (frule != failing.end()) {
		 //		std::cout << " getting FExamples ... "<< " " << std::endl;
		 std::vector<int>& examples = getFExamples(*frule);
		 examples.push_back(ex_id);
		 //		std::cout << " success "<< examples.size() << std::endl;
		 if (examples.size() <= getTolerance()) {
		 failing.erase(frule);
		 } else {
		 ++frule;
		 }
		 }
		 */
		#endif
	}
	
	double FaultTolerantRuleLearner::process_example(const std::vector<int>& example, int ex_id) {
#ifdef FTRULETRIE		
		profile::start_timer(PROF_TOTAL);
				
		// First add the new example to the language.
		// For each new symbol, this will cause the adding of a rule -symbol.
		getLanguage().add(example, ex_id-1);
		
		RuleList frules;
		
		// Find the rules that fail for the current example.
		// Load them into the list frules.
		profile::start_timer(PROF_FINDFAILING);
		getRules().findFailing(&example[0],static_cast<int>(example.size()),&frules);
		profile::stop_timer(PROF_FINDFAILING);
		
		
		double result = -1;
		if (getExampleFilter().process_example(*this,example,frules,getRules(),ex_id)) {
			
			if (hasMetric()) {
				result = getMetric().calculate(frules,this->getRules());
			} else {
				result = frules.size();
			}
			
			if (frules.size() > 0) {
				filterFailing(example,frules);
			}
			
			notifyFailing(frules, ex_id);
			
			// Quick-remove all the failing rules.
			// This usually just set the end-of-rule flag to false.
			// This is done to prevent these rules from causing the subrule test to fail.
			// We can't remove the rules completely because we still need to query them later.
			RuleList::iterator iter;
			
			
			for (iter = frules.begin(); iter != frules.end(); ++iter) {						
				profile::start_timer(PROF_QUICKREMOVE);
				(*iter)->unlink();
				profile::stop_timer(PROF_QUICKREMOVE);
			}
			
			sort_failing(frules);
			
			for (iter = frules.begin(); iter != frules.end(); ++iter) {
				std::vector<int> rule;
				
				
				// Get the elements of the current failing rule.
				(*iter)->getRule(rule);
				
			//	std::cout << "extending rule " << str(rule) << std::endl;
				
				int* r = (rule.size() == 0 ? NULL : &rule[0]); 
				
				ExampleList& example_ids = (*iter)->getExamples();
				
				
				// Check if extending the current failing rule will not take us beyond k.  		
				if (getK() == 0 || (int)rule.size() < getK()) {
					// Loop through the failing rules and extend them one-by-one.
					
					std::vector< std::pair<int,ExampleList > > extensions;
					for (unsigned int i=0; i<example_ids.size(); ++i) {
						std::vector<int> ex;
					//	std::cout << "retrieving example " << example_ids[i] << std::endl;
						example_ids[i]->getContent(ex);
					//	std::cout << "   " << str(ex) << std::endl;
						std::vector<int> ext;
						getLanguage().extensions(ex,ext);
						std::vector<int> ruleext;
						subtract(ext,rule,ruleext);
						profile::start_timer(PROF_FILTEREXT);
						getFilter().filterExtensions(rule,ex,ruleext);
						profile::stop_timer(PROF_FILTEREXT);
						merge_extensions(example_ids[i],extensions,ruleext);
					}
					
					if (cnf::LIMIT_MEMORY > 0) {
						// Check the memory threshold.
						// Prevent failure of the experiments due to thrashing.
						if (memoryUsage() > cnf::LIMIT_MEMORY*1000) {
							//delete[] c;
							std::cerr << "Memory usage exceeds limit (" << cnf::LIMIT_MEMORY << "MB) on handling example " << ex_id << ". Execution Terminated." << std::endl;
							throw SIGNAL_OUT_OF_MEMORY;
						}
					}
					
					
					profile::start_timer(PROF_FINDSUBRULES);
					
					std::vector<int> ruleexts;
					for (unsigned int i=0; i<extensions.size(); ++i) { 
						//std::cout << extensions[i].first << " "; 
						ruleexts.push_back(extensions[i].first); 
					}
					//	std::cout<< "   -> rule extensions " << str(ruleexts) << std::endl;				
					if (!getRules().findSubrules(r,static_cast<int>(rule.size()),&ruleexts[0],static_cast<int>(ruleexts.size()))) {
						profile::stop_timer(PROF_FINDSUBRULES);
						// For some extensions, a subrule was found.
						// These extensions have been set to 0 in the ext-array.
						//	std::cout<< "   -> rule extensions " << str(ruleexts) << std::endl;
						profile::start_timer(PROF_ADDRULE);
						for (unsigned int i=0; i<ruleexts.size(); i++) {
							if (ruleexts[i] != 0) {
								
								// Create the new rule by inserting the extension.
								//  (maybe we shouldn't do this explicitly)
								std::vector<int> newrule;
								insert(ruleexts[i],rule,newrule);
								ExampleList fexamples;
								subtract_examplelists(example_ids,extensions[i].second,fexamples);
								//			std::cout << "failing examples after update " << str(fexamples) << std::endl;	
								bool ok = true;
								for (unsigned int j = 1; j < newrule.size(); j++) {
									if (newrule[j] == newrule[j-1]) ok = false;
								}
								if (!ok) {
									std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << " " << ruleexts[i] << std::endl;
									throw SIGNAL_OUT_OF_MEMORY;
								}
																
								// Add the new rule to the ruletrie (and set the example-number)
								//		std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << " " << ruleexts[i] << std::endl;
								
								
								//if (newrule.size() == 1) {
								//	std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << " " << ruleexts[i] << std::endl;
								//}
								

								TreeNode* nrule = &getRules().addRule(&newrule[0],static_cast<int>(newrule.size()),ex_id);
								nrule->initExamples();
								for (unsigned int k=0; k<fexamples.size(); ++k) {
									fexamples[k]->increaseCount();
									nrule->getExamples().push_back(fexamples[k]);
								}
							}
						}
						profile::stop_timer(PROF_ADDRULE);
					}
					profile::stop_timer(PROF_FINDSUBRULES);
					
				}
				
				// Now remove the original rule from the ruletrie.
				// If the same nodes have been used in any extension of the rule,
				//  then this will have no effect.
				profile::start_timer(PROF_REMOVERULE);
				getRules().removeRule(r,static_cast<int>(rule.size()));
				for (unsigned int j = 0; j < example_ids.size(); j++) {
					example_ids[j]->decreaseCount();
				}
				
				profile::stop_timer(PROF_REMOVERULE);
			}
		}
		
		//			std::cout << "   nr of rules " << getRules().size()  << std::endl;
		//profile::stop_timer(PROF_FILTEREXA);
		profile::stop_timer(PROF_TOTAL);
		getExampleFilter().stream_buffer(ex_id, *this);
		
		return result;
#else
		return 0;
#endif
	}
#ifdef FTRULETRIE	
	void FaultTolerantRuleLearner::sort_failing(RuleList& failing) {
		std::multimap<int, TreeNode*> sfailing;
		for (RuleList::const_iterator i=failing.begin(); i!=failing.end(); ++i) {
			sfailing.insert(std::pair<int,TreeNode*>((*i)->length(),*i));
		}
		
		failing.clear();
		for (std::multimap<int, TreeNode*>::const_iterator i=sfailing.begin(); i!=sfailing.end(); ++i) {
			failing.push_back(i->second);
		}
		
		
	}


	void FaultTolerantRuleLearner::subtract_examplelists(ExampleList& a, ExampleList& b, ExampleList&c) {
		
		for (unsigned int i=0; i<a.size(); ++i) {
			bool found = false;
			for (unsigned int j=0; j<b.size(); ++j) {
				if (a[i] == b[j]) {
					found = true; 
					break;
				}
			}
			if (!found) c.push_back(a[i]);
		}
	}
	
	void FaultTolerantRuleLearner::merge_extensions(Example* ex_id, std::vector<std::pair<int, ExampleList > >& a, const std::vector<int>& b) {
		unsigned int i=0;
		unsigned int j=0;
		
		//		std::cout << "merging extensions " << std::endl;
		//		for (unsigned int i=0; i<a.size(); ++i) std::cout << a[i].first << " "; std::cout << std::endl;
		
		//		for (unsigned int i=0; i<b.size(); ++i) std::cout << b[i] << " "; std::cout << std::endl;
		
		while (i < a.size() || j < b.size()) {
			if (i >= a.size()) {
				a.push_back(std::pair<int, ExampleList >(b[j],ExampleList()));
				a.back().second.push_back(ex_id);
				++j; ++i;
			} else if (j >= b.size() || abs(a[i].first) < abs(b[j])) {
				++i;
			} else if (abs(a[i].first) > abs(b[j])) {
				a.insert(a.begin()+i,std::pair<int, ExampleList >(b[j],ExampleList()));
				a[i].second.push_back(ex_id);
				++j; ++i;
			} else { // abs == abs
				if (a[i].first == b[j]) {
					a[i].second.push_back(ex_id);
					++i;
					++j;
				} else if (a[i].first < b[j]) {
					++i;
				} else {
					a.insert(a.begin()+i,std::pair<int, ExampleList >(b[j],ExampleList()));
					a[i].second.push_back(ex_id);
					++j; ++i;					
				}
			}
		}
		
		//		std::cout << "result "; 
		//		for (unsigned int i=0; i<a.size(); ++i) std::cout << a[i].first << " "; std::cout << "END" << std::endl;
		
		for (unsigned int i=1; i<a.size(); ++i) 
			if (abs(a[i].first) < abs(a[i-1].first) || a[i].first == a[i-1].first) 
				throw SIGNAL_OUT_OF_MEMORY;
		
	} 
	#endif
	
	void FaultTolerantRuleLearner::printInfo() {
		getExamples().show("");
		std::cout << "number of cached examples: " << getExamples().size() << std::endl;
	}
	
	
}
