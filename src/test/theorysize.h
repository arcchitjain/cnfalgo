#ifndef THEORYSIZE_H_
#define THEORYSIZE_H_

#include <vector>
#include <math.h>
#include "../ruletrie/RuleTrie.h"
//#include "../ruletrie/TreeNode.h"
#include "../ruletrie/iterator/RuleIterator.h"
#include <iostream>
#include "../lib/profile.h"
#include <list>


inline int combination(int n, int k) {
	profile::start_timer("99 combination");
	// use direct factorial calculation (assume k is small, n is big)
	// C(n,k) = C(n-1,k) + C(n-1,k-1)
	// C(n,k) = n! / [ (n-k)! k!]
	//        = n(n-1)...(n-k+1) / k(k-1)...1
    //	if (k == 1) return n;
	int res = 1;
	for (int x=n; x>n-k; --x) res *= x;
	for (int x=2; x<=k; ++x) res /= x;
	profile::stop_timer("99 combination");
	return res;
}

inline bool calc_union(const std::vector<int>* ap, const std::vector<int>& b, std::vector<int>& result, unsigned int k) {
	
	if (ap == NULL) {
		result.assign(b.begin(),b.end());
		return true;
	} else {
		if (ap->size() == k && b.size() == k) return false;
		profile::start_timer("99 calc union");
		const std::vector<int>& a = *ap;
		unsigned int i=0;
		unsigned int j=0;
	
		while (i < a.size() || j < b.size()) {
			if (i >= a.size()) {
				result.push_back(b[j]); ++j;
			} else if (j >= b.size()) {
				result.push_back(a[i]); ++i;
			} else if (a[i] == b[j]) {
				result.push_back(a[i]); ++i; ++j;
			} else if (a[i] == -b[j]) {
				return false;
			} else if (abs(a[i]) < abs(b[j])) {
				result.push_back(a[i]); ++i;
			} else {
				result.push_back(b[j]); ++j;
			}
			if (result.size() > k) {
				profile::stop_timer("99 calc union");
				return false;
			}
		}
		profile::stop_timer("99 calc union");
		return true;
	}
}


inline bool is_subrule(const std::vector<int>& a, const std::vector<int>&b) {
	unsigned int i=0;
	unsigned int j=0;
	
	while (i < a.size()) {
		if (j >= b.size()) {
			return false;
		} else if (a[i] == b[j]) {
			++i; ++j;
		} else if (abs(a[i]) > abs(b[j])) {
			++j;
		} else {
			return false; // current element of a not in b
		}
	}	
	return true;
}


inline int calc_rule_space_size(int U, int k, int L, bool failing=false) {
	if (U < 0 || U > k) return 0;
	if (U == k) return 1;
	int result = combination(L-U,k-U);
	if (failing) {
		return result;
	} else {
		return (int)pow(2.0,k-U)*result;
	}	
}

template <typename RuleContainer>
inline int calc_theory_size(const RuleContainer& rulestore, const cnf::RuleTrie::Rule* base, typename RuleContainer::const_iterator start, int k, int L,bool failing) {
	int result;
	if (base == NULL) result = 0;
	else result = calc_rule_space_size(static_cast<int>(base->size()),k,L,failing);
	
	typename RuleContainer::const_iterator stop = rulestore.end();
	
	while (start != stop) {
		cnf::RuleTrie::Rule current;
		profile::start_timer("99 getrule");
		(*start)->getRule(current);
		profile::stop_timer("99 getrule");
		profile::start_timer("99 nextrule");
		++start;
		profile::stop_timer("99 nextrule");
		cnf::RuleTrie::Rule unionrule;
		if (calc_union(base,current,unionrule,k)) {
			result -= calc_theory_size(rulestore,&unionrule,start,k,L,failing);
		}
		
	//	std::cout << "start = " << (*start) << std::endl;
	}
	return result;
}


inline int calc_theory_size(const std::vector<std::vector<int> >& rulestore, const std::vector<int>* base, std::vector<std::vector<int> >::const_iterator start, int k, int L,bool failing) {
	
	int baseval = 0;
	if (base != NULL) baseval = calc_rule_space_size(static_cast<int>(base->size()),k,L,failing);
	int result = baseval;
	
	
		
	//if (base == NULL || base->size() < k) {
		std::vector<std::vector<int> >::const_iterator stop = rulestore.end();
		while (start != stop) {
			const std::vector<int>& current = (*start);
			profile::start_timer("99 nextrule");
			++start;
			profile::stop_timer("99 nextrule");
			std::vector<int> unionrule;
			if (calc_union(base,current,unionrule,k)) {
				result -= calc_theory_size(rulestore,&unionrule,start,k,L,failing);
			}
		
			//	std::cout << "start = " << (*start) << std::endl;
		}
	//}
	
	
	return result;
}

inline bool find_subrule(const std::list<std::vector<int> >& rulestore, std::list<std::vector<int> >::const_iterator start, const std::vector<int>& base) {
	std::list<std::vector<int> >::const_iterator stop = rulestore.end();
	while (start != stop) {
		if (is_subrule(*start,base)) return true;
		++start;
	}
	return false;
}

inline int correction(const std::list<std::vector<int> >& rulestore, const std::vector<int>* base, std::list<std::vector<int> >::const_iterator start, int k, int L, bool failing) {
	if (base != NULL && (int)base->size() == k) {
		if (find_subrule(rulestore,start,*base)) {
			return 1;
		} else {
			return 0; // std::cout << "unionset of size k" << std::endl;
		}
	} else {
		int result = 0; // temporary value of result
		std::list<std::vector<int> >::const_iterator stop = rulestore.end();
		while (start != stop) {
			const std::vector<int>& current = (*start);
			profile::start_timer("99 nextrule");
			++start;
			profile::stop_timer("99 nextrule");	
			std::vector<int> unionrule;
			if (calc_union(base,current,unionrule,k)) { 
				// the union has a size <= k and the rules have no opposite literals
				if (base != NULL && base->size() == unionrule.size()) {
					return calc_rule_space_size(static_cast<int>(base->size()),k,L,failing);
				} else {
					// now we need to calculate the value of correction(base \union r)
					result += calc_rule_space_size(static_cast<int>(unionrule.size()),k,L,failing) - correction(rulestore,&unionrule,start,k,L,failing);
				}
			} else {
				// we can skip this because rule_space_size(S) = 0 if |S| > k
			}
		}
		return result;
	}
}

template <class RuleContainer>
inline int toVector(const RuleContainer& rules, std::list<std::vector<int> >& nrules, int k) {
    typename RuleContainer::const_iterator node = rules.begin();
    typename RuleContainer::const_iterator stop = rules.end();
    
    int n = 0;
    while (node != stop) {
        cnf::RuleTrie::Rule current;
        profile::start_timer("99 getrule");
        (*node)->getRule(current);
        profile::stop_timer("99 getrule");
        if ((int)current.size() < k) {
            nrules.push_back(current);
        } else {
            n++;
        }
        ++node;
    }
    
    return n;
}

template <class RuleContainer>
inline int calc_theory_size_indirect(const RuleContainer& rulestore, int k, int L, bool failing = false) {
	std::list<std::vector<int> > nrules;
	profile::start_timer("99 tovector");
	int K = toVector(rulestore,nrules,k);
	profile::stop_timer("99 tovector");
	//return -calc_theory_size(nrules,NULL,nrules.begin(),k,L, failing);
	//std::cout << K << "/" << nrules.size()+K << std::endl;
	return correction(nrules,NULL,nrules.begin(),k,L, failing) + K;
}

template <class RuleContainer>
inline int calc_theory_size_direct(const RuleContainer& rulestore, int k, int L, bool failing = false) {
    return -calc_theory_size(rulestore,NULL,rulestore.begin(),k,L, failing);
}

template <class RuleContainer>
inline int calc_theory_size(const RuleContainer& rulestore, int k, int L, bool failing = false) {
	return calc_theory_size_indirect(rulestore,k,L,failing);
}




/*
inline int calc_theory_size_corr(const cnf::RuleTrie& rulestore, const std::vector<int>& baserule,
		const std::vector<int>& lastrule, int k, int L) {
	
	int result = 0;
	// if (baserule.size() == 0) result = 0;
	// else result = calc_rule_space_size(baserule.size(),k,L);
	
	// int s = 0;
	// int n = rulestore.numberOfRules();
	
	const cnf::TreeNode* rulenode = rulestore.nextRule(&lastrule[0], lastrule.size());
	while (rulenode != NULL) {
		std::vector<int> currentrule;
		rulenode->getRule(currentrule);
		std::vector<int> union_rule;
		if (calc_union(baserule,currentrule,union_rule,k)) {
			// there is a union
			result -= calc_theory_size(rulestore,union_rule,currentrule,k,L);
		}
		
		rulenode = rulestore.nextRule(&currentrule[0], currentrule.size());
	}
	return result;
}
*/


inline int max_theory_size(const cnf::RuleTrie& rulestore, int k, int L) {
	int result = 0;
	for (unsigned int i=0; i<=rulestore.maxlength(); i++) {
		//std::cout << i << " | " << rulestore.numberOfRules(i) << " | " << calc_rule_space_size(i,k,L) << std::endl;
		result += rulestore.numberOfRules(i) * calc_rule_space_size(i,k,L);
	}
	return result;
}

inline int total_theory_size(int k, int L) {
	return calc_rule_space_size(0,k,L);
}


/*
inline int calc_theory_size_corr(const cnf::RuleTrie& rulestore, int k, int L) {
	
	int result = max_theory_size(rulestore, k, L);
	const cnf::TreeNode* rulenode = rulestore.firstRule();
	int s = 0;
	int n = rulestore.numberOfRules();
	while (rulenode != NULL) {
		std::vector<int> currentrule;
		rulenode->getRule(currentrule);
		result += calc_theory_size_corr(rulestore,currentrule,currentrule,k,L);
		std::cout << " current estimate : " << result << " (" << ++s << "/"  << n << ")" << endl;
			
		rulenode = rulestore.nextRule(&currentrule[0], currentrule.size());
	}
	return result;
}
*/

#endif /*THEORYSIZE_H_*/
