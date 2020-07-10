/*

#include "RuleTrieProfiler.h"

//#include "NodeMap.h"
//#include "VNode.h"
//#include "ANode.h"
#include "RuleTrie.h"
#include <cassert>

namespace cnf
{

RuleTrieProfiler::RuleTrieProfiler(const RuleTrie& target) : target_(target) 
{
}

RuleTrieProfiler::~RuleTrieProfiler()
{
}

int RuleTrieProfiler::countANodes() const {
	if (getTarget().hasChild())	return countANodes(getTarget().getChild()); return 0;	
}

int RuleTrieProfiler::countVNodes(int level) const {
	if (getTarget().hasChild())	return countVNodes(getTarget().getChild(),0,level); return 0;
}

int RuleTrieProfiler::countSingleChild() const {
	if (getTarget().hasChild()) return countSingleChild(getTarget().getChild()); return 0;
}

int RuleTrieProfiler::countSingleBranchNodes() const {
	if (getTarget().hasChild()) return countSingleBranchNodes(getTarget().getChild()); return 0;
}

int RuleTrieProfiler::maximalDepth() const {
	if (getTarget().hasChild())	return maximalDepth(getTarget().getChild()); return 0;
}

int RuleTrieProfiler::maximalWidth() const {
	if (getTarget().hasChild())	return maximalWidth(getTarget().getChild()); return 0;	
}


int RuleTrieProfiler::countANodes(const VNode& current) const {
	int result = 0;
	
	NodeMap::const_iterator stop = current.getChildren().end();
	NodeMap::const_iterator node;
	for (node = current.getChildren().begin(); node != stop; ++node) {
		result += 1 + (node->second->hasPosChild() ? countANodes(node->second->getPosChild()) : 0) + 
						(node->second->hasNegChild() ? countANodes(node->second->getNegChild()) : 0);
	} 
	return result;
}

int RuleTrieProfiler::countVNodes(const VNode& current, int currlevel, int level) const {
	int result = (level < 0 || currlevel == level) ? 1 : 0;
	
	if (level < 0 | currlevel < level) {
		NodeMap::const_iterator stop = current.getChildren().end();
		NodeMap::const_iterator node;
			for (node = current.getChildren().begin(); node != stop; ++node) {
			result += (node->second->hasPosChild() ? countVNodes(node->second->getPosChild(), currlevel+1,level) : 0) + 
				(node->second->hasNegChild() ? countVNodes(node->second->getNegChild(), currlevel+1,level) : 0);	
		}
	} 
	return result;
}


int RuleTrieProfiler::countSingleBranchNodes(const VNode& current) const {
	int result = 0;
	NodeMap::const_iterator stop = current.getChildren().end();
	NodeMap::const_iterator node;
	bool isneg = false;
	for (node = current.getChildren().begin(); node != stop; ++node) {
		int cc = countSingleBranchNodes(*(node->second));
		result += abs(cc);
		if (cc < 0) isneg = true;
	}
	if (current.getChildren().size() > 1 || isneg) result = -result;
	//std::cout << "returning " << result << std::endl;
	return result;
}

int RuleTrieProfiler::countSingleBranchNodes(const ANode& current) const {
	if (current.isEndOfRule()) {
		return 0;
	} else if (!current.hasPosChild() && !current.hasNegChild()) {
		assert(false);
		return 0; // dangling node
	} else if (!current.hasNegChild()) { // hasPosChild()
		int cc = countSingleBranchNodes(current.getPosChild());
		if (current.getPosChild().getChildren().size() == 1 && cc >= 0) { // current has one child
			return cc + 1;
		} else {
			return cc;
		}
	} else if (!current.hasPosChild()) { // hasNegChild()
		int cc = countSingleBranchNodes(current.getNegChild());
		if (current.getNegChild().getChildren().size() == 1 && cc >= 0) { // current has one child
			return cc + 1;
		} else {
			return cc;
		}
	} else {
		return -(abs(countSingleBranchNodes(current.getNegChild())) + abs(countSingleBranchNodes(current.getPosChild())));
	}
}

int RuleTrieProfiler::countSingleChild(const ANode& current) const {
	int result = 0;
	if (!current.hasPosChild() && !current.hasNegChild()) {
		return 0;
	} else if (!current.hasNegChild()) { // hasPosChild()
		if (current.getPosChild().getChildren().size() == 1) result = 1;
		return result + countSingleChild(current.getPosChild());
	} else if (!current.hasPosChild()) { // hasPosChild()
		if (current.getNegChild().getChildren().size() == 1) result = 1;
		return result + countSingleChild(current.getNegChild());
	} else {
		return countSingleChild(current.getNegChild()) + countSingleChild(current.getPosChild());
	}
}
int RuleTrieProfiler::countSingleChild(const VNode& current) const {
	int result = 0;
	NodeMap::const_iterator stop = current.getChildren().end();
	NodeMap::const_iterator node;
		for (node = current.getChildren().begin(); node != stop; ++node) {	
			result += countSingleChild(*(node->second));
		}
	return result;
}

int RuleTrieProfiler::maximalDepth(const VNode& current) const {
	int result = 0;
	NodeMap::const_iterator stop = current.getChildren().end();
	NodeMap::const_iterator node;
		for (node = current.getChildren().begin(); node != stop; ++node) {
		result = std::max(result,std::max(node->second->hasPosChild() ? maximalDepth(node->second->getPosChild()) : 0, 
				node->second->hasNegChild() ? maximalDepth(node->second->getNegChild()) : 0));	
	} 
	return 1 + result;
}

int RuleTrieProfiler::maximalWidth(const VNode& current) const {
	int result = current.getChildren().size();
	NodeMap::const_iterator stop = current.getChildren().end();
	NodeMap::const_iterator node;
		for (node = current.getChildren().begin(); node != stop; ++node) {
		result = std::max(result,std::max(node->second->hasPosChild() ? maximalWidth(node->second->getPosChild()) : 0, 
				node->second->hasNegChild() ? maximalWidth(node->second->getNegChild()) : 0));
	} 
	return result;
}

void RuleTrieProfiler::printRuleTrieProfile(const RuleTrie& rules) {
	RuleTrieProfiler profiler(rules);
	
	int mdep = profiler.maximalDepth();
	int ands = profiler.countANodes();
	int vnds = profiler.countVNodes();
	int cmem = (sizeof(VNode)*vnds + sizeof(ANode)*ands);
	
	std::cout << "number of rules " << rules.numberOfRules() << std::endl;
	std::cout << "number of rules - levelwise" << std::endl;
	for (int i=0; i<=mdep; ++i) {
		std::cout << i << " : " << rules.numberOfRules(i) << std::endl;	
	}
	
	std::cout << "maximal depth " << mdep << std::endl;
	std::cout << "maximal width " << profiler.maximalWidth() << std::endl;
	std::cout << "number of single child nodes " << profiler.countSingleChild() << std::endl;
	std::cout << "number of single branch nodes " << profiler.countSingleBranchNodes() << std::endl;
	std::cout << "number of ANode " << ands << std::endl;
	std::cout << "number of VNode " << vnds << std::endl;
	
	std::cout << "number of VNode - levelwise" << std::endl;
	for (int i=0; i<mdep; ++i) {
		std::cout << i << " : " << profiler.countVNodes(i) << std::endl;	
	}
	
	std::cout << "calculated memory usage " << cmem/1024 << std::endl;
	std::cout << "real memory usage " << memoryUsage() << std::endl;
	
}

std::string RuleTrieProfiler::createDotString(const RuleTrie& rules) {
	RuleTrieProfiler profiler(rules);
	return profiler.toDot();
}

void RuleTrieProfiler::print_sizes() {
	std::cout << "size of VNode = " << sizeof(VNode) << " bytes" << std::endl;
	std::cout << "size of VNode* = " << sizeof(VNode*) << " bytes" << std::endl;	
	std::cout << "size of ANode = " << sizeof(ANode) << " bytes" << std::endl;
	std::cout << "size of ANode* = " << sizeof(ANode*) << " bytes" << std::endl;
	std::cout << "size of NodeMap = " << sizeof(std::map<int,ANode*>) << " bytes" << std::endl;

}

std::string RuleTrieProfiler::toDot(ANode& node, std::string& id) const {
	std::stringstream ss;
	
	std::string nid = id + str(node.getValue());
	
	std::string s;
	
	s = id + " -> " + nid + "; \n";
	s += nid + " [ label=\"" + str(node.getValue()) + "\", shape=box ]; \n";
	
	if (node.hasPosChild()) {
		s+=toDot(node.getPosChild(),nid);
	} else if (node.isEndOfRule(true)) {
//		std::cout << "dotting ANodeP" << std::endl;
		s += nid + " -> " + nid + "p; \n"; 
		s+= nid + "p  [ label = \"+\", shape=box, height=0.15, width=0.15, fixedsize=true]; \n";
	}
	
	if (node.hasNegChild()) {
		s+=toDot(node.getNegChild(),nid);
	} else if (node.isEndOfRule(false)) {
//		std::cout << "dotting ANodeN" << std::endl;
		s += nid + " -> " + nid + "n; \n"; 
		s += nid + "n  [ label = \"-\", shape=box, height=0.15, width=0.15, fixedsize=true]; \n";
	}
	
	return s;
}

std::string RuleTrieProfiler::toDot(VNode& node, std::string& id) const {
	//std::cout << "dotting VNode" << std::endl;
	std::string nid;
	std::string s;
	if (node.getValue()) {
		nid = id + "p";
		s = nid + " [ label=\"+\"";
	} else {
		nid = id + "n";
		s = nid + " [ label=\"-\"";
	}
	
	s += ", height=0.3, width=0.3, fixedsize=true]; \n";
	
	if (id != "") s += id + " -> " + nid + "; \n"; 
	
	NodeMap::const_iterator iter = node.getChildren().begin();
	NodeMap::const_iterator stop = node.getChildren().end();
   	while (iter != stop) {
		s += toDot(*iter->second, nid);
		++iter;
	};
	
	return s;
}

std::string RuleTrieProfiler::toDot() const {
	std::string result = "digraph trie {\n"; 
	if (getTarget().hasChild()) {
		std::string id = "";
		result += toDot(getTarget().getChild(), id);
	}
	return result + "\n}\n";	
}


}


*/