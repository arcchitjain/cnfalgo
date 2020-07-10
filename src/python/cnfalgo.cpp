//#define REDUNDANTFREE

#include <vector>

#include "../cnfminer/RuleLearner.h"
#include "../cnfminer/RFreeRuleLearner.h"
#include "../cnfminer/FaultTolerantRuleLearner.h"
#include "../filter/AttributeFilter.h"
//#include "../ruletrie/TreeNode.h"
#include "../ruletrie/RuleTrie.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "container_conversions.h"
#include <iostream>
#include <cassert>

/*
class RuleIterator {

  public:
	virtual std::vector<int> next() {
		PyErr_SetString(PyExc_StopIteration, "No more data.");
		boost::python::throw_error_already_set();
		
		std::vector<int> vec;
		return vec;
	}
	
	

};
*/
class RuleListIterator {

private :
	cnf::RuleList& rules;
	cnf::RuleList::iterator iter;
	int split;
	
public :
	
	RuleListIterator(cnf::RuleList& rules, int split=0) : rules(rules), split(split) {
		iter = rules.begin();
	}
	
	
	std::vector<int> next() {
		if (iter == rules.end()) {
			PyErr_SetString(PyExc_StopIteration, "No more data.");
			boost::python::throw_error_already_set();
		}
		std::vector<int> vec;
		(*iter)->getRule(vec,split);
		++iter;
		return vec;
	}
	
	RuleListIterator iterator() {
		return *this;
	}
	
};

class RuleTrieIterator  {
	
private :
	const cnf::TreeNode* current;
	
public:
	RuleTrieIterator() : current(NULL) {}

	RuleTrieIterator(const cnf::RuleTrie& learner) : current(NULL) {
		current = learner.firstRule();
	}
	
	std::vector<int> next() {
		if (current == NULL) {
			PyErr_SetString(PyExc_StopIteration, "No more data.");
			boost::python::throw_error_already_set();
		}
		
		std::vector<int> vec;
		current->getRule(vec);
		current = current->next();
		return vec;
	} 

	RuleTrieIterator iterator() {
		return *this;
	}
	
	
};

class CNFAlgo {

	private:
		
		cnf::RuleLearner* learner;
		
		int tolerance;
		
		inline cnf::RuleLearner& getLearner() { 
			assert(learner!=NULL); 
			return *learner; 
		}
		
	public:
	
		CNFAlgo(int k) : learner(NULL), tolerance(0) {
			learner = new cnf::RuleLearner(k);
			learner->initialize();
		}
		
		CNFAlgo(int k, bool filter) : learner(NULL), tolerance(0) {
			learner = new cnf::RuleLearner(k);
			if (filter)
				learner->setFilter(*(new cnf::AttributeFilter()), false);
			learner->initialize();
		}
		
		CNFAlgo(int k, bool filter, int tolerance) : learner(NULL), tolerance(tolerance) {
			learner = new cnf::FaultTolerantRuleLearner(k,tolerance);
			if (filter)
				learner->setFilter(*(new cnf::AttributeFilter()), false);
			learner->initialize();
		}
		
#ifdef REDUNDANTFREE
		CNFAlgo(int k, bool filter, bool rfree) : learner(NULL) {
			if (rfree) {
				learner = new cnf::RFreeRuleLearner(k);
			} else {
				learner = new cnf::RuleLearner(k);
			}
			if (filter)
				learner->setFilter(*(new cnf::AttributeFilter()), false);
			learner->initialize();			
		}
#endif
		
		~CNFAlgo() {
			delete learner;
			//std::cerr << "clean up" << std::endl;
		}
		
		double train_example(const std::vector<int>& example, int example_id) {
			return getLearner().process_example(example,example_id);
		}
		
		double train_example2(const std::vector<int>& example) {
			return train_example(example,2);
		}
		
		double test_example(const std::vector<int>& example) {
			return getLearner().getRules().countFailing(&example[0],example.size());
		}
		
		boost::python::list test_set(const std::vector<std::vector<int> >& example) {
			boost::python::list result;
			for (unsigned int i = 0; i<example.size(); ++i) {
				result.append(test_example(example[i]));
			}
			return result;
		}
		
		RuleListIterator findFailing(const std::vector<int>& example) {
			return findPartialFailing(example,0);
		}
	
		
		RuleListIterator findPartialFailing(const std::vector<int>& example, int split) {
			return findPartialFailingFull(example,split,false);
		}
		
		RuleListIterator findPartialFailingFull(const std::vector<int>& example, int split, bool reportfull) {
		//	getLearner().getLanguage().add(example,2);
			cnf::RuleList* frules = new cnf::RuleList;
			getLearner().getRules().findFailing(&example[0],example.size(),frules,-1,split);
			if (reportfull) 
				return RuleListIterator(*frules);
			else
				return RuleListIterator(*frules,split);
		}
			
		RuleListIterator iterator() {
			cnf::RuleList* rules = new cnf::RuleList; 
			getLearner().getRules().listRules(rules);
			return RuleListIterator(*rules);
//			return RuleTrieIterator(getLearner().getRules());
		}
		
		RuleListIterator otherRules() {
			cnf::RuleList* rules = new cnf::RuleList; 
			if (getLearner().hasSecondaryRules()) {
				getLearner().getSecondaryRules().listRules(rules);
			}
			return RuleListIterator(*rules);
			
		}
		
						
		int size() {
			return getLearner().getRules().size();
		}

		int size2() {
			if (getLearner().hasSecondaryRules()) 
				return getLearner().getSecondaryRules().size();
			else 
				return 0;
		}
	
		int numberOfRules2(int l) {
			if (getLearner().hasSecondaryRules()) 
				return getLearner().getSecondaryRules().numberOfRules(l);
			else 
				return 0;
		}
	
		
//		bool verify() {
//			return getLearner().getRules().verifyStructure();
//		}
		
		int numberOfRules(int l) {
			return getLearner().getRules().numberOfRules(l);
		}
		
		int getK() {
			return getLearner().getK();
		}
		
/*		int countRedundant() {
			return getLearner().getRules().countRedundant();
		}
*/		
		int getTolerance() {
			return tolerance;
		}
		
		void printInfo() {
			getLearner().printInfo();
		}
		
		int getCacheSize() {
			return getLearner().getCacheSize();
		}
		
};

class PyRuleTrie : public cnf::RuleTrie {
 
   public:
  
	PyRuleTrie() : cnf::FTANode(0), cnf::RuleTrie(0) {}

	inline bool findRule(const std::vector<int>& rule) { return cnf::RuleTrie::findRule(&rule[0], rule.size()) != NULL; }
	
	inline void addRule(const std::vector<int>& rule) { cnf::RuleTrie::addRule(&rule[0], rule.size());  }
	
	inline void removeRule(const std::vector<int>& rule) { cnf::RuleTrie::removeRule(&rule[0], rule.size());  }	

	RuleTrieIterator iterator() {
		return RuleTrieIterator(*this);
	}
};

std::string vec_repr(const std::vector<int>& vec) {
	std::stringstream ss;
	ss << "[";
	if (vec.size() > 0) {
		ss << vec[0];
		for (unsigned int i=1; i<vec.size(); ++i) {
			ss << ", " << vec[i];
		}
	}
	ss << "]";
	return ss.str();
}


using namespace boost::python;

void init_module() {	
	container_conversions::from_python_sequence<std::vector<int>,container_conversions::variable_capacity_policy>();
	container_conversions::from_python_sequence<std::vector<std::vector<int> >, container_conversions::variable_capacity_policy>();
}


BOOST_PYTHON_MODULE(cnfalgo) {
	init_module();
	
	class_<CNFAlgo>("CNFAlgo", init<int>())
		.def(init<int,bool>())
		.def(init<int,bool,int>())
#ifdef REDUNDANTFREE
		.def(init<int,bool,bool>())
#endif		
		.def("train", &CNFAlgo::train_example)
		.def("train", &CNFAlgo::train_example2)
		.def("test", &CNFAlgo::test_example)
		.def("failing", &CNFAlgo::findFailing)
		.def("failing", &CNFAlgo::findPartialFailing)
		.def("failing", &CNFAlgo::findPartialFailingFull)
		.def("test_set", &CNFAlgo::test_set)
		.def("__iter__", &CNFAlgo::iterator)
		.def("__len__", &CNFAlgo::size)
		.def("size", &CNFAlgo::numberOfRules)
		.def("size", &CNFAlgo::size)
		.def("size2", &CNFAlgo::size2)
		.def("size2", &CNFAlgo::numberOfRules2)
		.def("info", &CNFAlgo::printInfo)
//		.def("verify", &CNFAlgo::verify)
//		.def("count_redundant", &CNFAlgo::countRedundant)
		.def("cache_size", &CNFAlgo::getCacheSize)
		.def("rules2", &CNFAlgo::otherRules)
		.add_property("k", &CNFAlgo::getK)
	;
	
	class_<PyRuleTrie>("RuleTrie")
		.def("add", &PyRuleTrie::addRule)
		.def("remove", &PyRuleTrie::removeRule)
		.def("__contains__", &PyRuleTrie::findRule)
		.def("__len__", &PyRuleTrie::size)
		.def("__iter__", &PyRuleTrie::iterator)
	;
	
	
	class_<std::vector<int> >("Example")
		.def(vector_indexing_suite<std::vector<int> >())
		.def("__repr__",vec_repr)
	;
	

	class_<RuleListIterator>("RuleListIterator", no_init)
		.def("next",&RuleListIterator::next)
		.def("__iter__", &RuleListIterator::iterator)
	;

	class_<RuleTrieIterator>("RuleListIterator", no_init)
	.def("next",&RuleTrieIterator::next)
	.def("__iter__", &RuleTrieIterator::iterator)
	;
	
	
}
