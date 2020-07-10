#ifndef SIMPLETIEBREAKER_H_
#define SIMPLETIEBREAKER_H_

#include "TieBreaker.h"

namespace cnf
{

class SimpleTieBreaker : public cnf::TieBreaker
{
public:
	SimpleTieBreaker() {}
	virtual ~SimpleTieBreaker() {}
	
	virtual score_result calculate_score(std::vector<double>& scores, std::vector<int>& distributions) const {
		double n = 1;
		for (unsigned int i=1; i<scores.size(); i++) {
			if (scores[i] < scores[0]) return 0;
			else if (scores[i] == scores[0]) n++;
		} 	
			
		return 1/n;	
	}
	
	virtual inline std::string getName() const { return "random"; }
	
};

}

#endif /*SIMPLETIEBREAKER_H_*/
