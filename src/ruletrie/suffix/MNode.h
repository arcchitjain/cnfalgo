#ifndef MNODE_H_
#define MNODE_H_

#include <vector>

namespace cnf
{

namespace ruletrie_map
{

class MNode
{

private:
	
	std::vector<int> atoms;
	
public:
	MNode();
	virtual ~MNode();
};

}

}

#endif /*MNODE_H_*/
