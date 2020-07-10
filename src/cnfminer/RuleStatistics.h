
#ifndef RULESTATISTICS_H_
#define RULESTATISTICS_H_

#include <map>

namespace cnf
{



class RuleStatistics
{
	
	
protected :
	
	int k;
		
	std::map<int,int> rulesizes_add;
	
	std::map<int,int> rulesizes_remove;
	
	std::map<int,int> rulesizes_test;
	
	int language_inc;
	
	int language_dec;
	
	virtual void insert(std::map<int,int>& m, int length);
	
	virtual void clear();
			
public:
	RuleStatistics(int k) : 
		k(k), language_inc(0), language_dec(0) {}
	
	virtual ~RuleStatistics() { delete &rulesizes_add; delete &rulesizes_remove; delete &rulesizes_test; }
	
	virtual void addRule(int length);
	
	virtual void testRule(int length);
	
	virtual void removeRule(int length);
	
	virtual void nextExample() = 0;
	
	virtual void addItem() { language_inc++; }
	
	virtual void removeItem() { language_dec++; }
};

}

#endif /*RULESTATISTICS_H_*/
