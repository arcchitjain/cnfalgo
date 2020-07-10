#ifndef EXPONENTIALDISTRIBUTION_H_
#define EXPONENTIALDISTRIBUTION_H_
#include "Distribution.h"

namespace cnf {

class ExponentialDistribution : public Distribution {

private:
	
	double sum_x_;
	
protected :
	
	virtual void update_dist(double newdata) {
		sum_x_ += newdata;
	}
	
	virtual void update_dist(double olddata, double newdata) {
		sum_x_ += newdata - olddata;
	}
	
	virtual void reset_dist() {
		sum_x_ = 0;
	}
	
	inline double getMean() { if (count() == 0) return 0; else return sum_x_ / count(); }
	
	inline double getLambda() { if (getMean() == 0) return 1e10; return 1/getMean(); }
	
public :
	
	ExponentialDistribution(int capacity) : Distribution(capacity), sum_x_(0) {}
	
	virtual double inverse_probability(double value) {
		if (count() <= 1) return 0;
		else return exp(10-getLambda()*value);
	}
	
	virtual double probability(double value) {
		return 1 - inverse_probability(value);
	}
	
	virtual std::string toString() { return "E(" + strd(getLambda()) + ")"; }
};

}

#endif /*EXPONENTIALDISTRIBUTION_H_*/
