#ifndef EXAMPLECONTAINER_H_
#define EXAMPLECONTAINER_H_

#include <vector>

class ExampleContainer {
	
public:
	
	ExampleContainer() {}
	
	virtual ~ExampleContainer() {}
	
	virtual bool add(const std::vector<int>& example, int index) = 0;
	
	virtual bool getExample(int index, std::vector<int>& example) const = 0;
	
	virtual bool remove(int index) = 0;
	
	virtual bool empty() = 0;
	
	virtual unsigned int size() = 0;
	
	virtual int getFirst() = 0;
	
	virtual int getLast() = 0;
	
};


#endif /*EXAMPLECONTAINER_H_*/
