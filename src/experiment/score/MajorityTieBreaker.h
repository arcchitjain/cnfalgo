#ifndef MAJORITYTIEBREAKER_H_
#define MAJORITYTIEBREAKER_H_

#include "TieBreaker.h"

namespace cnf
{

class MajorityTieBreaker : public cnf::TieBreaker
{
public:
	MajorityTieBreaker() {}
	virtual ~MajorityTieBreaker() {}
	
	virtual score_result calculate_score(std::vector<double>& scores, std::vector<int>& dist) const {
		double n = 1;
		for (unsigned int i=1; i<scores.size(); i++) {
			if (scores[i] < scores[0]) {
				//cout << "better theory found" << endl;
				return 0;
			} else if (scores[i] == scores[0]) {
				if (dist.empty() || dist[i] == dist[0]) {
					n++;
				} else if (dist[i] > dist[0]) {
					return 0;
				}
			}
		} 	
		return 1/n;	
	}
			
	virtual inline std::string getName() const { return "major"; }
};

}


#endif /*MAJORITYTIEBREAKER_H_*/
