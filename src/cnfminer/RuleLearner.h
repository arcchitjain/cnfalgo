/*!  
   \file NRuleLearner.h 

   $Id$
   $URL$
   
   \version   $Rev$
   \author    Anton Dries
   \date      18/03/08
   \namespace cnfalgo
*/
#ifndef cnfalgoNRuleLearner
#define cnfalgoNRuleLearner

#include "../ruletrie/RuleTrie.h"

#include "../language/Language.h"
#include "../filter/Filter.h"
#include "LearnerListener.h"
#include "ExampleFilter.h"
#include "../metric/Metric.h"
#include <cassert>

#include <vector>

#include "../lib/SmartPointer.h"

namespace cnf {

class RuleLearner : public LanguageListener {

  private:
  
	int k_;
  
	smart_ptr<RuleTrie> rules_;
	
	smart_ptr<Language> language_;
	
	smart_ptr<Filter> lfilter_;
	
	smart_ptr<ExampleFilter> efilter_;
	
	smart_ptr<LearnerListener> listener_;
	
	int excount_;
	
	smart_ptr<Metric> metric_;

  public:

	virtual void printInfo() {
		std::cout << "# Minimal Rules: " << getRules().size() << std::endl;
		{		
			RuleList rules; getRules().listRules(&rules);
			for (unsigned int i=0; i<rules.size() ; ++i) {
				std::cout << "   * " << rules[i]->toString() << std::endl;
			}
		}

	}

	RuleLearner(int k) : k_(k), rules_(NULL), language_(NULL), lfilter_(NULL), efilter_(NULL), listener_(NULL), excount_(0), metric_(NULL) {}

	~RuleLearner() {
		getLanguage().removeListener(this);
	}
	
	virtual void initialize();
	
// GET AND SET METHODS
	
	RuleTrie& getRules() const {
		assert(rules_ != NULL);
		return *rules_;
	}
	
	virtual bool hasSecondaryRules() const { return false; }
	
	virtual RuleTrie& getSecondaryRules() const { assert(false); }
	
	void setRules(RuleTrie& rules, bool persist=true) {
		assert(rules_ == NULL);
		rules_ = &rules;
		rules_.set_destruct(!persist);
	}
	
	Language& getLanguage() const {
		assert(language_ != NULL);
		return *language_;
	}
	
	void setLanguage(Language& language, bool persist=true) {
		assert(language_ == NULL);
		language_ = &language;
		language_.set_destruct(!persist);
	}
	
	bool hasMetric() const {
		return metric_ != NULL;
	}
	
	Metric& getMetric() const {
		assert(metric_ != NULL);
		return *metric_;
	}
	
	void setMetric(Metric& metric, bool persist=true) {
		metric_ = &metric;
		metric_.set_destruct(!persist);
	}
	
	Filter& getFilter() const {
		assert(lfilter_ != NULL);
		return *lfilter_;
	}
	
	void setFilter(Filter& filter, bool persist=true) {
		assert(lfilter_ == NULL);
		lfilter_ = &filter;
		lfilter_.set_destruct(!persist);
	}
	
	ExampleFilter& getExampleFilter() const {
		assert(efilter_ != NULL);
		return *efilter_;
	}
	
	void setExampleFilter(ExampleFilter& filter, bool persist=true) {
		assert(efilter_ == NULL);
		efilter_ = &filter;
		efilter_.set_destruct(!persist);
	}
	
	inline bool hasListener() const {
		return listener_ != NULL;
	}
	
	inline LearnerListener& getListener() const {
		assert(listener_ != NULL);
		return *listener_;
	}
	
	void setListener(LearnerListener& listener, bool persist=true) {
		listener_ = &listener;
		listener_.set_destruct(!persist);
	}
	
	void clearListener() {
		listener_ = NULL;
	}
	
	inline int getK() { return k_; }
	
	virtual double process_example(const std::vector<int>& example, int ex_id);
	
	virtual void languageElementAdded(int element, int ex_id) {
		if (getRules().findRule(NULL,0) == NULL) { // empty rule not there
			int r = -element;
			getRules().addRule(&r,1,ex_id);
			//	std::cout << "element added: new L = " << getLanguage().getSize() << std::endl;
		}
	}
	
	virtual void languageElementRemoved(int element, int ex_id) {
		int r = -element;
		getRules().removeRule(&r, 1);
	}
	
	int getExampleCount() {
		return excount_;
	}
	
	inline void notifyInit() const {
		if (hasListener()) getListener().learnerInit(*this);
	}
	
	inline void notifyFailing(const RuleList& rules, int ex_id) const {
		if (hasListener()) getListener().learnerFailingRules(*this,rules,ex_id);
	}
	
	double getScore(const std::vector<int>& example) {
		RuleList rules;
		getRules().findFailing(&example[0], static_cast<int>(example.size()), &rules);
		if (hasMetric()) {
			return getMetric().calculate(rules,this->getRules());
		} else {
			return rules.size();
		}
	}
	
	double getScore(const std::vector<int>& example, Metric& metric) {
		RuleList rules;
		getRules().findFailing(&example[0], static_cast<int>(example.size()), &rules);
		return metric.calculate(rules,this->getRules());
	}
	
	virtual int getCacheSize() { return 0; }
	

};

}  // end namespace cnf

#endif // cnfalgoNRuleLearner
