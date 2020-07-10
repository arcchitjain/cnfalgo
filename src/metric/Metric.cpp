#include "Metric.h"
#include "../ruletrie/RuleTrie.h"

namespace cnf {

double Metric::calculate(const std::vector<int>& example, RuleTrie& theory) const {
	RuleList rules;
	theory.findFailing(&example[0],static_cast<int>(example.size()),&rules);
	return calculate(rules,theory);
}


} // end namespace
