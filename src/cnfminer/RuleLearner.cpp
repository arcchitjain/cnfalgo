/*
 *  NRuleLearner.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 18/03/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "RuleLearner.h"

#include "../language/SimpleLanguage.h"
#include "../exampletrie/ExampleTrie.h"
#include "../lib/profile.h"
#include "../lib/defaults.h"
#include "ExampleFilter.h"

using namespace cnf;

void RuleLearner::initialize() {
	if (!rules_) { rules_ = new RuleTrie(); rules_.set_destruct(true); }
	if (!language_) { language_ = new SimpleLanguage(); language_.set_destruct(true); }
	if (!lfilter_) { lfilter_ = new Filter(); lfilter_.set_destruct(true); }
	if (!efilter_) { efilter_ = new ExampleFilter(*new ExampleTrie()); efilter_.set_destruct(true); }
	
	getRules().addRule(NULL,0,1);

	getLanguage().addListener(this);
	notifyInit();
}

double RuleLearner::process_example(const std::vector<int>& example, int ex_id) {
#ifdef VERBOSE 
	std::cout << "\nRuleLearner::process_example([" << str(example) << "])" << std::endl;
#endif
	
	profile::start_timer(PROF_TOTAL);
	
//	examplecount++;
	excount_++;
	
	// First add the new example to the language.
	// For each new symbol, this will cause the adding of a rule -symbol.
	getLanguage().add(example, ex_id-1);
	
	RuleList frules;
	
	// Find the rules that fail for the current example.
	// Load them into the list of rules.
	profile::start_timer(PROF_FINDFAILING);
	getRules().findFailing(&example[0],static_cast<int>(example.size()),&frules);
	profile::stop_timer(PROF_FINDFAILING);

#if VERBOSE > 2
	std::cout << "Found " << frules.size() << " failing rules" << std::endl;
	
	for (unsigned int i=0; i<frules.size(); ++i) {
		std::vector<int> rr; frules[i]->getRule(rr);
		std::cout << " * " << str(rr) << std::endl;
	}
#endif
	
	double result = -1;
	
	//profile::start_timer(PROF_FILTEREXA);
	if (getExampleFilter().process_example(*this,example,frules,getRules(),ex_id)) {
		//	profile::stop_timer(PROF_FILTEREXA);
		//	std::cout << "learning example " << ex_id << std::endl;
		
		if (hasMetric()) {
			result = getMetric().calculate(frules,this->getRules());
		} else {
			result = frules.size();
		}
		
		notifyFailing(frules, ex_id);
		
		// Quick-remove all the failing rules.
		// This usually just set the end-of-rule flag to false.
		// This is done to prevent these rules from causing the subrule test to fail.
		// We can't remove the rules completely because we still need to query them later.
		RuleList::iterator iter;
		
		
		for (iter = frules.begin(); iter != frules.end(); iter++) {						
			profile::start_timer(PROF_QUICKREMOVE);
			(*iter)->unlink();
			profile::stop_timer(PROF_QUICKREMOVE);
		}
		
		std::vector<int> extensions;
		// Get the extensions based on the current example (and the current language).
		// Adding any of these extensions to the failing rule will make it valid again.
		profile::start_timer(PROF_GENERATEEXT);
		getLanguage().extensions(example,extensions);
		profile::stop_timer(PROF_GENERATEEXT);
		
		
		// Loop through the failing rules and extend them one-by-one.
		for (iter = frules.begin(); iter != frules.end(); iter++) {
			
			if (cnf::LIMIT_MEMORY > 0) {
				// Check the memory threshold.
				// Prevent failure of the experiments due to thrashing.
				if (memoryUsage() > cnf::LIMIT_MEMORY*1000) {
					//delete[] c;
					std::cerr << "Memory usage exceeds limit (" << cnf::LIMIT_MEMORY << "MB) on handling example " << ex_id << ". Execution Terminated." << std::endl;
					throw SIGNAL_OUT_OF_MEMORY;
				}
			}
			
			std::vector<int> rule;
			
			// Get the elements of the current failing rule.
			(*iter)->getRule(rule);
			
			
			//std::cout << " - failing rule " << str(rule) << " for example " << ex_id+1 << " : " << str(example) << std::endl;
			
			int* r = (rule.size() == 0 ? NULL : &rule[0]); 
			// Check if extending the current failing rule will not take us beyond k.  		
			if (getK() == 0 || (int)rule.size() < getK()) {
				// Remove the elements in the current rule from the list of extensions.
				std::vector<int> ruleext;
				subtract(extensions,rule,ruleext);
				
				// Apply a filter to the extensions
				//  (maybe we should combine this with the previous step)
				
				//cout << "ruleext "; for (int i=0; i<ruleext.size(); i++) cout << ruleext[i] << " "; cout << endl;
				//cout << "example "; for (int i=0; i<example.size(); i++) cout << example[i] << " "; cout << endl;			
				profile::start_timer(PROF_FILTEREXT);
				getFilter().filterExtensions(rule,example,ruleext);
				profile::stop_timer(PROF_FILTEREXT);
				//cout << "rule    "; for (int i=0; i<rule.size(); i++) cout << rule[i] << " "; cout << endl;
				//cout << "ruleex2 "; for (int i=0; i<ruleext.size(); i++) cout << ruleext[i] << " "; cout << endl;
				
				// Check if there exists a subrule of the current rule + extension exists.
				// This is done in batch, i.e. all the extensions are checked at the same time.
				// This returns true if for all extensions there exists a subrule.
				
				//#define NEWSRDISCOVERY
#ifndef NEWSRDISCOVERY
				profile::start_timer(PROF_FINDSUBRULES);
				
				
				if (!getRules().findSubrules(r,static_cast<int>(rule.size()),&ruleext[0],static_cast<int>(ruleext.size()))) {
					profile::stop_timer(PROF_FINDSUBRULES);
					// For some extensions, a subrule was found.
					// These extensions have been set to 0 in the ext-array.
					
					profile::start_timer(PROF_ADDRULE);
					
#ifdef VERBOSE 
					std::cout << "Updating Rule " << str(rule) << " with extensions: ";
#endif
					
					for (unsigned int i=0; i<ruleext.size(); i++) {
						if (ruleext[i] != 0) {
							// Create the new rule by inserting the extension.
							//  (maybe we shouldn't do this explicitly)
							std::vector<int> newrule;
							insert(ruleext[i],rule,newrule);
							
#ifdef VERBOSE 
							std::cout << ruleext[i] << " ";
#endif
							
							
							// Add the new rule to the ruletrie (and set the example-number)
							//	std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << std::endl;
							getRules().addRule(&newrule[0],static_cast<int>(newrule.size()),ex_id);
						}
					}
#ifdef VERBOSE 					
					std::cout << std::endl;
#endif
					profile::stop_timer(PROF_ADDRULE);
				}
				profile::stop_timer(PROF_FINDSUBRULES);
				
#else
				for (unsigned int i=0; i<ruleext.size(); i++) {
					getRules().extendRule(rule,ruleext[i],ex_id+1);
				}
#endif		
				
			}
			
			// Now remove the original rule from the ruletrie.
			// If the same nodes have been used in any extension of the rule,
			//  then this will have no effect.
			profile::start_timer(PROF_REMOVERULE);
			delete (*iter);
			//getRules().removeRule(r,rule.size());
			profile::stop_timer(PROF_REMOVERULE);
		}
	}
	//profile::stop_timer(PROF_FILTEREXA);
	profile::stop_timer(PROF_TOTAL);
	getExampleFilter().stream_buffer(ex_id, *this);
	
	return result;
}
