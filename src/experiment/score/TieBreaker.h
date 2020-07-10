#ifndef TIEBREAKER_H_
#define TIEBREAKER_H_

namespace cnf
{

struct score_result {
	double standard;
	double yesno;
	double noyes;
	
	score_result(double s) {
		standard = s;
		yesno = (s > 0);
		noyes = (s == 1); 
	}
	
	score_result(double s, double y, double n) {
		standard = s;
		yesno = y;
		noyes = n;
	}
	
	score_result& operator=(double s) {
		standard = s;
		yesno = (s > 0);
		noyes = (s == 1);
		return *this;
	}
	
	score_result& operator+=(score_result other) {
		standard += other.standard;
		yesno += other.yesno;
		noyes += other.noyes;
		return *this;	
	}
	
	score_result& operator/=(int div) {
		standard /= div;
		yesno /= div;
		noyes /= div;
		return *this;
	}
};

class TieBreaker
{

public:
	TieBreaker() {}
	virtual ~TieBreaker() {}
	
	//virtual score_result calculate_score(const double* scores, int count, int correct, const int* distribution) const = 0;
	
	/*
	 * PRECONDITION: correct value is first in vectors 
	 */
	virtual score_result calculate_score(std::vector<double>& scores, std::vector<int>& distributions) const = 0;
	
	virtual std::string getName() const = 0;
};

typedef TieBreaker Scorer;

}

#endif /*TIEBREAKER_H_*/
