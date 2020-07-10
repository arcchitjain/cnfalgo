#ifndef DISTRIBUTION_H_
#define DISTRIBUTION_H_

#include <deque>
#include <iostream>

namespace cnf {

class Distribution {
	
private:
	
	std::deque<double> data_;
	
	unsigned int capacity_;
	
protected :
	inline std::deque<double>& getData() { return data_; }

	virtual void update_dist(double newdata) = 0;
	
	virtual void update_dist(double olddata, double newdata) = 0;
	
	virtual void reset_dist() = 0;
	
	inline unsigned int count() { return static_cast<int>(getData().size()); }
	
	inline unsigned int capacity() { return capacity_; } 
	
public :
	inline void update_data(double newdata) {
		if (getData().size() >= capacity()) {
			double front = getData().front();
			getData().pop_front();
			getData().push_back(newdata);
			update_dist(front,newdata);
		} else {
			getData().push_back(newdata);
			update_dist(newdata);
		}
		
	//	std::cout << "dist updated : value " << newdata << "/" << count() << " -> " << toString() << std::endl;
	}
	
	virtual void reset() {
		getData().clear();
		reset_dist();
	}
	
	virtual double probability(double value) = 0;
	
	virtual double inverse_probability(double value) = 0;
	
	Distribution(unsigned int capacity) : capacity_(capacity) {}
	
	virtual ~Distribution() {}
	
	virtual std::string toString() = 0;
};

} // end namespace cnf

#endif /*DISTRIBUTION_H_*/
