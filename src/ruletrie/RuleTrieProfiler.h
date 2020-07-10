#ifndef RULETRIEPROFILER_H_
#define RULETRIEPROFILER_H_
/*
#include <string>
#include <sstream>

namespace cnf
{ 

#include "../ruletrie/RuleTrie.h"
// class VNode;
// class ANode;

class RuleTrieProfiler
{
	
private:

	const RuleTrie& target_;
	
protected:

	virtual inline const RuleTrie& getTarget() const { return target_; }
	
	virtual inline const RuleTrie* getTargetPtr() const { return &target_; }
	
	virtual int countANodes(const VNode& currentNode) const;
	
	virtual int countVNodes(const VNode& currentNode, int currlevel, int level) const;
	
	virtual int countSingleChild(const VNode& current) const;
	
	virtual int countSingleChild(const ANode& current) const;
	
	virtual int maximalDepth(const VNode& currentNode) const;
	
	virtual int maximalWidth(const VNode& currentNode) const;
	
	int countSingleBranchNodes(const ANode& current) const;
	
	int countSingleBranchNodes(const VNode& current) const;
	
	std::string toDot(ANode& node, std::string& id) const;
		
	std::string toDot(VNode& node, std::string& id) const;

		
	
public:
	RuleTrieProfiler(const RuleTrie& target);
	virtual ~RuleTrieProfiler();
	
	virtual int countANodes() const;
	
	virtual int countVNodes(int level = -1) const;
	
	virtual int maximalDepth() const;
	
	virtual int maximalWidth() const;
	
	virtual int countSingleChild() const;
	
	int countSingleBranchNodes() const;
	
	std::string toDot() const;

	
	static void printRuleTrieProfile(const RuleTrie& rules);
	
	static std::string createDotString(const RuleTrie& rules);
	
	static void print_sizes();	
};

} 

*/
#endif /*RULETRIEPROFILER_H_*/
