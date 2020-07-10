#ifndef NORMALDISTRIBUTION_H_
#define NORMALDISTRIBUTION_H_

#include "Distribution.h"
#include "math/normaldistr.h"

namespace cnf {

class NormalDistribution : public Distribution {

private:
	
	double sum_x_;
	double var_x_;
	
protected :
	
	virtual void update_dist(double newdata) {
		sum_x_ += newdata;
		var_x_ += pow(getMean() - newdata,2);
	}
	
	virtual void update_dist(double olddata, double newdata) {
		var_x_ -= pow(getMean() - olddata,2);
		sum_x_ += newdata - olddata;
		var_x_ += pow(getMean() - newdata,2);
	}
	
	virtual void reset_dist() {
		var_x_ = 0;
		sum_x_ = 0;
	}
	
	inline double getMean() { if (count() == 0) return 0; else return sum_x_ / count(); }
	
	inline double getStd() {
		if (var_x_ < 0) return 0;
		if (count() <= 1) return 1;
		else return sqrt(var_x_) / (double)(count()-1); }
	
public :
	
	NormalDistribution(int capacity) : Distribution(capacity), sum_x_(0), var_x_(0) {}
	
	virtual double probability(double value) {
		if (count() <= 1) return 1;
		else return normaldistribution((value-getMean())/getStd());
	}
	
	virtual double inverse_probability(double value) { return 1-probability(value); }
	
	virtual std::string toString() { return "N(" + strd(getMean()) + "," + strd(getStd()) + ")"; }
};

}


#endif /*NORMALDISTRIBUTION_H_*/
