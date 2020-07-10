#ifndef SIMPLETIEBREAKER_H_
#define SIMPLETIEBREAKER_H_

#include "TieBreaker.h"

namespace cnf
{

/*
 * IdealTieBreaker always selects the correct class if there is a measurable difference between classes.
 */
class IdealTieBreaker : public cnf::TieBreaker
{
public:
	IdealTieBreaker() {}
	virtual ~IdealTieBreaker() {}
	
	virtual score_result calculate_score(map<int,double>& scores, int correct_class, map<int,int>& class_distributions) const {
		double n = 0;
		for (map<int,double>::iterator t=scores.begin(); t != scores.end(); t++) {
			if (t->second == 0) n++;
		}
		
		if (n == 0) return 1;
		else if (scores[correct_class] == 0) return 1/n;
		else return 0; 	
	}
	
	virtual inline string getName() const { return "ideal"; }
	
};

}

#endif /*SIMPLETIEBREAKER_H_*/
