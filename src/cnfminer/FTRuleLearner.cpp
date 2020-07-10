/*
 *  FTRuleLearner.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 24/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#include "FTRuleLearner.h"
#include "../lib/profile.h"
#include "../lib/defaults.h"

namespace cnf {

	void FTRuleLearner::initialize() {
		RuleLearner::initialize();
	//	std::cout << " initialize " << getTolerance() << " " << std::endl;
		TreeNode* first = const_cast<TreeNode*>(getRules().firstRule());
		getFRules().insert(std::pair< TreeNode*, std::vector<int> >(first,std::vector<int>()));
	}
	

	FTRuleLearner::FTRuleLearner(int k, unsigned int tolerance) :
		RuleLearner(k), tolerance_(tolerance), 
		examples_(*new ExampleTrie()), rules_(*new std::map<TreeNode*, std::vector<int> >()),
		example_count_(*new std::map<int,int>()) {}
	
	FTRuleLearner::~FTRuleLearner() {
		delete(&examples_);
		delete(&rules_);
	}

	std::vector<int>& FTRuleLearner::getFExamples(TreeNode* clause) {
		std::map<TreeNode*, std::vector<int> >::iterator iter = getFRules().find(clause);
	//	std::cout << clause << std::endl;
		if (iter == getFRules().end()) {
			getFRules().insert(std::pair<TreeNode*, std::vector<int> >(clause,std::vector<int>()));
			iter = getFRules().find(clause); 
		}
		return iter->second;
	}

	void FTRuleLearner::filterFailing(const std::vector<int>& example, RuleList& failing)  {
		int ex_id = getExamples().getLast()+2;
		
		getExamples().add(example, ex_id);
		
		unsigned int i=0;
		
		while (i < failing.size()) {
			std::vector<int>& examples = getFExamples(failing[i]);
			examples.push_back(ex_id);
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
	}
	
	double FTRuleLearner::process_example(const std::vector<int>& example, int ex_id) {
			//examples->add(example,example_length);
	//	std::cout << "----------------- processing example " << (ex_id) << " -------------------" << std::endl;
			
		profile::start_timer(PROF_TOTAL);
			
			//	examplecount++;
		//excount_++;
			
			// First add the new example to the language.
			// For each new symbol, this will cause the adding of a rule -symbol.
		getLanguage().add(example, ex_id-1);
			
		RuleList frules;
			
			// Find the rules that fail for the current example.
			// Load them into the list frules.
		profile::start_timer(PROF_FINDFAILING);
		getRules().findFailing(&example[0],static_cast<int>(example.size()),&frules);
		profile::stop_timer(PROF_FINDFAILING);

	//	std::cout << " found failing " << frules.size() << " " << std::endl;
			
		double result = -1;
			
		if (getExampleFilter().process_example(*this,example,frules,getRules(),ex_id)) {
				
			if (hasMetric()) {
				result = getMetric().calculate(frules,this->getRules());
			} else {
				result = frules.size();
			}
			
			if (frules.size() > 0) {
				std::cout << "Found " << frules.size() << " failing ";
				filterFailing(example,frules);
				std::cout << "filtered: " << frules.size() << " " << std::endl;
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
				
			for (iter = frules.begin(); iter != frules.end(); ++iter) {
				std::vector<int> rule;
				
				// Get the elements of the current failing rule.
				(*iter)->getRule(rule);
				
				int* r = (rule.size() == 0 ? NULL : &rule[0]); 
				// Check if extending the current failing rule will not take us beyond k.  		
				std::vector<int> example_ids = getFExamples(*iter);
				
				
				
				if (getK() == 0 || (int)rule.size() < getK()) {
					// Loop through the failing rules and extend them one-by-one.
					
					std::cout << "failing examples " << str(rule) << " -> " << str(example_ids) << std::endl;	
	//				std::cout << " example ids " << example_ids.size() << " " << std::endl;
					
					std::vector< std::pair<int,std::vector<int> > > extensions;
					for (unsigned int i=0; i<example_ids.size(); ++i) {
						std::vector<int> ex;
						bool xxx = getExamples().getExample(example_ids[i],ex);
						
						std::cout << "LOADING EXAMPLE " << xxx << std::endl;
						std::vector<int> ext;
						getLanguage().extensions(ex,ext);
						std::vector<int> ruleext;
						subtract(ext,rule,ruleext);
				//		std::cout << str(ext) << " - " << str(rule) << " = " << str(ruleext) << std::endl;
						profile::start_timer(PROF_FILTEREXT);
						getFilter().filterExtensions(rule,ex,ruleext);
						profile::stop_timer(PROF_FILTEREXT);
	//					std::cout << " calling merge extensions " << std::endl;
						merge_extensions(example_ids[i],extensions,ruleext);
	//					std::cout << " SUCCESS " << std::endl;
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
									std::vector<int> fexamples;
									subtract(example_ids,extensions[i].second,fexamples);
						//			std::cout << "failing examples after update " << str(fexamples) << std::endl;	
									bool ok = true;
									for (unsigned int j = 1; j < newrule.size(); j++) {
										if (newrule[j] == newrule[j-1]) ok = false;
									}
									if (!ok) {
										std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << " " << ruleexts[i] << std::endl;
										throw SIGNAL_OUT_OF_MEMORY;
									}
									
									for (unsigned int j = 0; j < fexamples.size(); j++) {
										increaseExampleCount(fexamples[j]);
									}
									
									// Add the new rule to the ruletrie (and set the example-number)
							//		std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << " " << ruleexts[i] << std::endl;
									
									
									//if (newrule.size() == 1) {
									//	std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << " " << ruleexts[i] << std::endl;
									//}
									
									TreeNode* nrule = &getRules().addRule(&newrule[0],static_cast<int>(newrule.size()),ex_id);
									getFRules().insert(std::pair< TreeNode*, std::vector<int> >(nrule,fexamples));
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
				getFRules().erase(*iter);
				for (unsigned int j = 0; j < example_ids.size(); j++) {
//					decreaseExampleCount(example_ids[j]);
				}
				
				profile::stop_timer(PROF_REMOVERULE);
			}
			}
			
//			std::cout << "   nr of rules " << getRules().size()  << std::endl;
			//profile::stop_timer(PROF_FILTEREXA);
			profile::stop_timer(PROF_TOTAL);
			getExampleFilter().stream_buffer(ex_id, *this);
			
			return result;
		}
		
	void FTRuleLearner::merge_extensions(int ex_id, std::vector<std::pair<int, std::vector<int> > >& a, const std::vector<int>& b) {
		unsigned int i=0;
		unsigned int j=0;

//		std::cout << "merging extensions " << std::endl;
//		for (unsigned int i=0; i<a.size(); ++i) std::cout << a[i].first << " "; std::cout << std::endl;
		
//		for (unsigned int i=0; i<b.size(); ++i) std::cout << b[i] << " "; std::cout << std::endl;

		while (i < a.size() || j < b.size()) {
			if (i >= a.size()) {
				a.push_back(std::pair<int, std::vector<int> >(b[j],std::vector<int>()));
				a.back().second.push_back(ex_id);
				++j; ++i;
			} else if (j >= b.size() || abs(a[i].first) < abs(b[j])) {
				++i;
			} else if (abs(a[i].first) > abs(b[j])) {
				a.insert(a.begin()+i,std::pair<int, std::vector<int> >(b[j],std::vector<int>()));
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
					a.insert(a.begin()+i,std::pair<int, std::vector<int> >(b[j],std::vector<int>()));
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
	
	
	void FTRuleLearner::increaseExampleCount(int ex_id) {
		std::map<int,int>::iterator record = example_count_.find(ex_id);
		if (example_count_.end() == record) {
			example_count_.insert(std::pair<int,int>(ex_id,1));
		} else {
			record->second += 1;
		}
	}
	
	void FTRuleLearner::decreaseExampleCount(int ex_id) {
		std::map<int,int>::iterator record = example_count_.find(ex_id);
		if (example_count_.end() == record) {
			
		} else {
			record->second -= 1;
			if (record->second == 0) {
				example_count_.erase(record);
				getExamples().remove(ex_id);
			}
		}		
	}
	
	void FTRuleLearner::printInfo() {
	
		int* tolcnt = new int[getTolerance()+1];
		for (unsigned int i=0; i<=getTolerance(); ++i) {
			tolcnt[i] = 0;
		}
	
		for (std::map<TreeNode*, std::vector<int> >::iterator r = rules_.begin(); r != rules_.end(); r++) {
			tolcnt[r->second.size()]++;
		}
		
		delete[] tolcnt;
		std::cout << "tolerance: ";
		for (unsigned int i=0; i<=getTolerance(); ++i) {
			std::cout << i << ":" << tolcnt[i] << " ";
		}
		std::cout << std::endl;
		
	
	
	}
	

}
