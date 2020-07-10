/*
 *  RFreeRuleLearner.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 06/08/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#include "RFreeRuleLearner.h"

#include "../lib/functions.h"

namespace cnf {
	
	void RFreeRuleLearner::initialize() {
		RuleLearner::initialize();
		if (!rrules_) { rrules_ = new RuleTrie(1); rrules_.set_destruct(true); }
	}
	
	RFreeRuleLearner::~RFreeRuleLearner() {
	}
	
	double RFreeRuleLearner::process_example(const std::vector<int>& example, int ex_id) {
#ifdef VERBOSE 
		std::cout << "RFreeRuleLearner::process_example([" << str(example) << "])" << std::endl;
#endif
		
	
		// 0. Update language
#ifdef VERBOSE 
		std::cout << "  --> updating language" << std::endl;
#endif		
		getLanguage().add(example, ex_id-1);	
	
		// 1. Find failing rules in regular rules.

#ifdef VERBOSE 
		std::cout << "  --> finding failing clauses" << std::endl;
#endif	
		
		RuleList frules;
		getRules().findFailing(&example[0],static_cast<int>(example.size()),&frules);
		
#ifdef VERBOSE 
		std::cout << "Found " << frules.size() << " failing rules" << std::endl;
		
		for (unsigned int i=0; i<frules.size(); ++i) {
			std::cout << " * " << frules[i]->toString() << std::endl;
		}
#endif
		
		
		
		notifyFailing(frules, ex_id);

#ifdef VERBOSE 
		std::cout << "  --> quick removing failing clauses" << std::endl;
#endif		
		
		// 2. Quickremove failing rules
		RuleList fcrules;

		for (RuleList::iterator iter = frules.begin(); iter != frules.end(); ++iter) {
			(*iter)->unlink();
			RuleList tcrules;
			(*iter)->getRRecord().listChildren(example,fcrules,tcrules);
			for (RuleList::iterator child = tcrules.begin(); child != tcrules.end(); ++child) {
				if ((*child)->getRRecord().unsetRedundant(*iter)) {
					// if rule is no longer redundant -> move it to regular rules
					(*child)->moveTo(getRules());
				}
			}
		}
		
#ifdef VERBOSE 
		std::cout << "Additionally found " << fcrules.size() << " redundant failing rules" << std::endl;
		
		for (unsigned int i=0; i<fcrules.size(); ++i) {
			std::cout << " * " << fcrules[i]->toString() << std::endl;
		}
#endif

#ifdef VERBOSE 
		std::cout << "  --> quick removing failing redundant clauses" << std::endl;
#endif		
		for (RuleList::iterator iter = fcrules.begin(); iter != fcrules.end(); ++iter) {
			(*iter)->unlink();
		}
		
		double result = frules.size();

		std::vector<int> extensions;
		// Get the extensions based on the current example (and the current language).
		// Adding any of these extensions to the failing rule will make it valid again.
		getLanguage().extensions(example,extensions);

#ifdef VERBOSE 
		std::cout << "  --> generate refinements" << std::endl;
#endif		
		
		// 3. Generate refinements for each of the failing rules.
		for (RuleList::iterator iter = frules.begin(); iter != frules.end(); ++iter) {
			// Add FRecords to the failing rules.
			refineRule(*(*iter),example,extensions);
		}
		
		// 4. Generate new rules and add them to a temporary ruletrie. 
#ifdef VERBOSE 
		std::cout << "  --> creating new rules" << std::endl;
#endif				
		RuleTrie newrules(2);
		for (RuleList::iterator iter = frules.begin(); iter != frules.end(); ++iter) {
			std::vector<int>& refs = (*iter)->getFRecord().refinements;
			std::vector<int> frule; (*iter)->getRule(frule);
			
		//	std::cout << "[" + str(frule) << "] + " << str(refs) << std::endl;
			for (unsigned int i=0; i<refs.size(); ++i) {
				std::vector<int> newrule;
				insert(refs[i],frule,newrule);
				// Add the new rule to the ruletrie (and set the example-number)
				//	std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << std::endl;
				(*iter)->getFRecord().rules.insert(std::pair<int,RuleNode*>(refs[i],&newrules.addRule(&newrule[0],static_cast<int>(newrule.size()),ex_id)));
			}
		}
		
		
#ifdef VERBOSE 
		std::cout << "    ==> " << newrules.size() << " new rules added" << std::endl;

		std::cout << "  --> updating redundancy records" << std::endl;
#endif		
		
		// 5. Update redundancy records of new rules.
		for (RuleList::iterator iter = frules.begin(); iter != frules.end(); ++iter) {
			std::vector<int>& refs = (*iter)->getFRecord().refinements;
			std::map<int,RuleNode*>& refrules = (*iter)->getFRecord().rules;
			std::vector<int> frule; (*iter)->getRule(frule);
			
			for (unsigned int i=0; i<refs.size(); ++i) {
				for (unsigned int j=0; j<refs.size(); ++j) {
					if (i != j) {
						// Find a clause that matches frule+i such that it makes frule+j redundant:
						// This can be in Rules or RRules or in rest of frules
						RuleList matching;
						int* r = new int[2];
						if (abs(refs[i]) < abs(refs[j])) { r[0] = -refs[i]; r[1] = refs[j]; } else { r[0] = refs[j]; r[1] = -refs[i]; }
						getRules().findSubrule(&frule[0],static_cast<int>(frule.size()),r,2,&matching);
						getRRules().findSubrule(&frule[0],static_cast<int>(frule.size()),r,2,&matching);
						newrules.findSubrule(&frule[0],static_cast<int>(frule.size()),r,2,&matching);
						
						// Each rule in matching, combined with F+refs[i] is a parent of F+refs[j]
						for (RuleList::iterator mr = matching.begin(); mr != matching.end(); ++mr) {
							refrules[refs[j]]->getRRecord().setRedundant(refrules[refs[i]],*mr);
							
							#ifdef VERBOSE
							std::cout << "Rule [" << refrules[refs[j]]->toString() << 
									"] is made redundant by rules [" << refrules[refs[i]]->toString() << "] and [" <<
									(*mr)->toString() << "]" << std::endl;
							#endif
						}
					}
				}
			}
		}
		
		// At this point all new rules have been generated and added to the temporary RuleTrie 'newrules'.
		// Each of these rules has its redundancy parents and children set correctly.
		// The only thing remaining is now putting each rule from 'newrules' into either getRules() or getRRules().
#ifdef VERBOSE 
		std::cout << "  --> moving rules (stage 1)" << std::endl;
#endif		
		
		// Find a rule in newrules that has no redundancy parents -> add it to not redundant
		// For each of its children that is still in `newrules'
		//		-> if the 'other' parent is in Rules or RRules -> add it to redundant
		//		-> if the 'other' parent is in 'newrules'	-> wait
		// Do this for all newrules without redundancy parents
		RuleList newruleslist; newrules.listRules(&newruleslist);
		for (RuleList::iterator iter = newruleslist.begin(); iter != newruleslist.end(); ++iter) {			
			if (!(*iter)->hasRRecord() || !(*iter)->getRRecord().hasParents()) {
				// Move this rule to non redundant rules
				(*iter)->moveTo(getRules());

				RRecord::RRecordList& rchildren = (*iter)->getRRecord().getChildren();
				for (RRecord::RRecordList::iterator citer = rchildren.begin(); citer != rchildren.end(); ++citer) {
					if ((*citer)->getRule()->getRoot().getIdentifier() == 2) { // child still in newrules
						RRecord* otherParent = (*citer)->getOtherParent(&((*iter)->getRRecord()));
						if (otherParent->getRule()->getRoot().getIdentifier() != 2) {
							// move child to redundant rules
							(*citer)->getRule()->moveTo(getRRules());
						}
						
					}
				}
			}
		}

#ifdef VERBOSE 
		std::cout << "    ==> " << newrules.size() << " new rules remaining" << std::endl;
		std::cout << "    ==> " << getRules().size() << " rules in current theory" << std::endl;

		std::cout << "  --> moving rules (stage 2)" << std::endl;
#endif		
		
		// At this point only loopy redundant rules should still exist;
		// So now do the previous again but now also for rules that do have RParents
		newruleslist.clear(); 
		newrules.listRules(&newruleslist);

		for (unsigned int i = 0; i < newruleslist.size(); ++i) {
			if (newruleslist[i]->getRoot().getIdentifier() == 2) { // verify that this rule is still in newrules
			// Move this rule to non redundant rules
				newruleslist[i]->moveTo(getRules());
				RRecord::RRecordList& rchildren = newruleslist[i]->getRRecord().getChildren();
				for (RRecord::RRecordList::iterator citer = rchildren.begin(); citer != rchildren.end(); ++citer) {
					if ((*citer)->getRule()->getRoot().getIdentifier() == 2) { // child still in newrules
						RRecord* otherParent = (*citer)->getOtherParent(&(newruleslist[i]->getRRecord()));
						if (otherParent->getRule()->getRoot().getIdentifier() != 2 ) {
						// !otherParent->getRule()->isFailing() && 
							// other parent not in newrules and it is not failing
							// move child to redundant rules
							std::vector<int> s; (*citer)->getRule()->getRule(s);
							(*citer)->getRule()->moveTo(getRRules());
						}
					}
				}
			}
		}

		// Now newrules should be empty.
		assert(newrules.size() == 0);

#ifdef VERBOSE 
		std::cout << "    ==> " << newrules.size() << " new rules remaining" << std::endl;
		std::cout << "    ==> " << getRules().size() << " rules in current theory" << std::endl;


		std::cout << "  --> adding additional redundant clauses" << std::endl;
#endif		
		
		// Last thing to do: extend failing child rules and add them to the set of redundant rules
		// These clause are guaranteed to be redundant.

		bool update = true;
		while (update) {
			update = false;
			for (RuleList::iterator iter = fcrules.begin(); iter != fcrules.end(); ++iter) {
				refineRuleRecursive(*(*iter),example,extensions);
			}
		}
		
		for (RuleList::iterator iter = frules.begin(); iter != frules.end(); ++iter) {
			// Add FRecords to the failing rules.
			delete (*iter);
		}

	//	std::cout << fcrules.size() << " fcrules " << std::endl;
		for (RuleList::iterator iter = fcrules.begin(); iter != fcrules.end(); ++iter) {
			// Add FRecords to the failing rules.
	//		std::cout << "   * " << (*iter) << std::endl;
			delete (*iter);
		}		

	
#ifdef VERBOSE
		std::cout << " --> finished processing example" << std::endl;

#endif

		RuleList rls; getRRules().listRules(&rls);
		for (int unsigned i=0; i<rls.size();++i) {
			if (rls[i]->getRRecord().getParents().empty()) {
				std::cout << " FOUND ORPHANED REDUNDANT RULE [" << rls[i]->toString() << "]" << std::endl;
		//		rls[i]->unlink();
		//		rls[i]->moveTo(getRules());
			}
	//		if (!rls[i]->getRRecord().hasNonRedundantParent()) {
	//			std::cout << " FOUND REDUNDANT RULE WITHOUT NR-PARENT [" << rls[i]->toString() << "]" << std::endl;
	//		}
		}
		
		return result;
	}
	
	/*
		Generates possible refinements of the rule such that the refined rule is minimal.
		Returns true is the set of refinements is non-empty. return !refinements.empty()
	*/
	bool RFreeRuleLearner::refineRule(RuleNode& rulenode, const std::vector<int>& example, const std::vector<int>& extensions) {
		std::vector<int> rule;
		rulenode.getRule(rule);
		
		FRecord* frecord = new FRecord(true);
		frecord->refined = 2;
		rulenode.setFRecord(frecord);
		const int* r = (rule.size() == 0 ? NULL : &rule[0]);	// make rule an int array
		if (getK() == 0 || rule.size() < (unsigned int)getK()) {
			// maximal length of rule not exceeded
			std::vector<int> ruleext;
			subtract(extensions,rule,ruleext);		// subtract the atoms in the rule from the set of extensions (equality through abs)
		
			getFilter().filterExtensions(rule,example,ruleext);
			
			if (!getRules().findSubrules(r,static_cast<int>(rule.size()),&ruleext[0],static_cast<int>(ruleext.size())) && !getRRules().findSubrules(r,static_cast<int>(rule.size()),&ruleext[0],static_cast<int>(ruleext.size()))) {
				for (unsigned int i=0; i<ruleext.size(); i++) {
					if (ruleext[i] != 0) {
						frecord->refinements.push_back(ruleext[i]);
					}
				}
				return true;
			}
		}
		return false;
	
	}
	
	
	void RFreeRuleLearner::refineRuleRecursive(RuleNode& rulenode, const std::vector<int>& example, 
												 const std::vector<int>& extensions) {


		if (rulenode.getFRecord().refined >= 2) {
//			std::cout << " rule [" << rulenode.toString() << "] was already refined" << std::endl;
		} else if (rulenode.getFRecord().refined == 1) {
			refineRule(rulenode,example,extensions);
			
			std::vector<int>& refs = rulenode.getFRecord().refinements;
			std::vector<int> frule; rulenode.getRule(frule);
			
			//	std::cout << "[" + str(frule) << "] + " << str(refs) << std::endl;
			for (unsigned int i=0; i<refs.size(); ++i) {
				std::vector<int> newrule;
				insert(refs[i],frule,newrule);
				// Add the new rule to the ruletrie (and set the example-number)
//				std::cout << "   -> ADDING rule " << str(newrule) << std::endl;
				rulenode.getFRecord().rules.insert(std::pair<int,RuleNode*>(refs[i],&getRRules().addRule(&newrule[0],static_cast<int>(newrule.size()))));
//				std::cout << str(rulenode.getFRecord().refinements) << std::endl;
			}
			rulenode.getFRecord().refined = 3;
		
		} else {
			rulenode.getFRecord().refined = 1;
			RRecord::RRecordPairList pps = rulenode.getRRecord().getParents();
			std::vector<int> parent_refs;
			std::map<int,std::vector<std::pair<RuleNode*,RuleNode*> > > parent_rules;
			// Each pair of parents of this rule has exactly one failing one.
			
//			std::cout << rulenode.toString() << std::endl;
			for (RRecord::RRecordPairList::iterator pp = pps.begin(); pp != pps.end(); ++pp) {
				// Find out which of the two parents is failing
				RRecord* fp; RRecord* nfp;
				
				if (pp->first->getRule()->isFailing()) {
					assert(!pp->second->getRule()->isFailing());
					fp = pp->first; nfp = pp->second;
				} else if (pp->second->getRule()->isFailing()) {
					assert(!pp->first->getRule()->isFailing());
					fp = pp->second; nfp = pp->first;
				} else {
					assert(false);
				}
				
//				std::cout << " refining rule [" << rulenode.toString() << "] requires [" << 
//							fp->getRule()->toString() << "]" << std::endl;
				
				refineRuleRecursive(*(fp->getRule()),example,extensions);				
				
				std::vector<int>& prefs = fp->getRule()->getFRecord().refinements;
	//			std::cout << " prefs: " << str(prefs) << " | " << fp->getRule()->getFRecord().rules.size() << std::endl;
				
				for (unsigned int i=0; i<prefs.size();++i) {
					parent_rules[prefs[i]].push_back(std::pair<RuleNode*,RuleNode*>(fp->getRule()->getFRecord().rules[prefs[i]],nfp->getRule()));
				}
				unionl(parent_refs, prefs);
			}
			
	//		std::cout << " parent_refs: " << str(parent_refs) << " XX " <<  parent_rules.size() << std::endl;
			
			
			if (rulenode.getFRecord().refined == 3) {
				std::vector<int>& refs = rulenode.getFRecord().refinements;
				// get contents of current rule
				std::vector<int> frule; rulenode.getRule(frule);
//				std::cout << " CASE 3 ENCOUNTERED " << rulenode.toString() << " | " << str(refs) << " | " 
//					<< rulenode.getFRecord().rules.size() << std::endl;	
				
				for (unsigned int i=0; i<refs.size(); ++i) {
					std::vector<int> newrule;
					insert(refs[i],frule,newrule);
					RuleNode* frulenode = getRRules().findRule(&newrule[0],static_cast<int>(newrule.size()));
					assert(frulenode != NULL);
//					if (frulenode == NULL) {
//						frulenode = &getRRules().addRule(&newrule[0],newrule.size());
//					}
//					rulenode.getFRecord().rules.insert(std::pair<int,RuleNode*>(refs[i],frulenode));
					
					std::vector<std::pair<RuleNode*,RuleNode*> > curr_parent_rules = parent_rules[refs[i]];
//					std::cout << curr_parent_rules.size() << std::endl;
					for (unsigned int j=0; j<curr_parent_rules.size(); ++j) {
						
//						std::cout << frulenode->toString() << " = " << curr_parent_rules[j].first->toString() << " + " <<
	//					curr_parent_rules[j].second->toString() << std::endl;
						
						frulenode->getRRecord().setRedundant(curr_parent_rules[j].first,curr_parent_rules[j].second);
					}
				}
				
			} else {
				refineRule(rulenode,example,parent_refs);

			
				std::vector<int>& refs = rulenode.getFRecord().refinements;
				// get contents of current rule
				std::vector<int> frule; rulenode.getRule(frule);
			
	#ifdef VERBOSE
				std::cout << " UUUUpdating failing redundant rule " << rulenode.toString() << " + " << str(refs) << " (" << str(parent_refs) << ")" << std::endl;
	#endif
			
				for (unsigned int i=0; i<refs.size(); ++i) {
					std::vector<int> newrule;
					insert(refs[i],frule,newrule);
					RuleNode* frulenode = getRRules().findRule(&newrule[0],static_cast<int>(newrule.size()));
					if (frulenode == NULL) {
						frulenode = &getRRules().addRule(&newrule[0],static_cast<int>(newrule.size()));
					}
					rulenode.getFRecord().rules.insert(std::pair<int,RuleNode*>(refs[i],frulenode));
							
					std::vector<std::pair<RuleNode*,RuleNode*> > curr_parent_rules = parent_rules[refs[i]];
					for (unsigned int j=0; j<curr_parent_rules.size(); ++j) {
					
//						std::cout << frulenode->toString() << " = " << curr_parent_rules[j].first->toString() << " + " <<
//							curr_parent_rules[j].second->toString() << std::endl;
					
						frulenode->getRRecord().setRedundant(curr_parent_rules[j].first,curr_parent_rules[j].second);
					}
				}
		}			
		}
	}
	
		/*	
			
		 for (unsigned int i=0; i<ruleext.size(); i++) {
		 if (ruleext[i] != 0) {
		 // Create the new rule by inserting the extension.
		 //  (maybe we shouldn't do this explicitly)
		 std::vector<int> newrule;
		 insert(ruleext[i],rule,newrule);
		 
		 // Add the new rule to the ruletrie (and set the example-number)
		 //	std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << std::endl;
		 getRules().addRule(&newrule[0],newrule.size(),ex_id);
		 }
		 
						}
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
				getRules().removeRule(r,rule.size());
				profile::stop_timer(PROF_REMOVERULE);
			}
		}
		//profile::stop_timer(PROF_FILTEREXA);
		profile::stop_timer(PROF_TOTAL);
		getExampleFilter().stream_buffer(ex_id, *this);
		
*/		
	
	void RFreeRuleLearner::printInfo() {
	
		int nrr = getRules().size();
		int rr = getRRules().size();

	
		std::cout << "non-red. rules " << nrr << std::endl;
		{		
			RuleList rules; getRules().listRules(&rules);
			for (unsigned int i=0; i<rules.size() ; ++i) {
				std::cout << "   * " << rules[i]->toString() << std::endl;
			}
		}
		

		std::cout << "red. rules " << rr << std::endl;
		{		
			RuleList rules; getRRules().listRules(&rules);
			for (unsigned int i=0; i<rules.size() ; ++i) {
				std::cout << "   * " << rules[i]->toString() << " R" << std::endl;
			}
		}
		
		
		std::cout << "all rules " << nrr+rr << std::endl;				
		
	}
	
	
	
	
}
