#include "NFTRuleTrie.h"
#include "NFTANode.h"
#include "NFTVNode.h"
#include "../../lib/profile.h"
#include "../../lib/functions.h"


namespace cnf
{

NFTRuleTrie::~NFTRuleTrie()
{
	delete child_;
}
#ifdef ANODE_DLL
TreeNode& NFTRuleTrie::getFirstNode(unsigned int value) const { 
	NodeMap::const_iterator first = getFirstMap().find(value);
	if (first == getFirstMap().end())  return const_cast<NFTRuleTrie&>(*this); else return (*(first->second));
}
	
TreeNode& NFTRuleTrie::getLastNode(unsigned int value) const { 
	NodeMap::const_iterator last = getLastMap().find(value);
	if (last == getLastMap().end())  return const_cast<NFTRuleTrie&>(*this); else return (*(last->second));
}
#endif

void NFTRuleTrie::clear() {
	deleteChild();
	rulelength.clear();
	addRule(NULL,0,1);
}

NFTTreeNode* NFTRuleTrie::findRule(const int* rule,unsigned int length) const {
	if (length == 0) {
		if (isEndOfRule()) {
			return const_cast<NFTRuleTrie*>(this);
		} else {
			return NULL;
		}
	} else {
		if (hasChild()) {
			return getChild().findRule(rule,length); 
		} else {
			return NULL;
		}
	}
}
#ifdef ANODE_DLL
void NFTRuleTrie::addNode(ANode& node) {
	// What this does:
	//	insert this ANode in the double-linked-list of atomnodes.
	
	node.getPrev(node.getValue()).setNext(node.getValue(),node);
	node.getNext(node.getValue()).setPrev(node.getValue(),node);
}
#endif
NFTTreeNode& NFTRuleTrie::addRule(const int* rule, unsigned int length, int ex_id) {
	addRuleLength(length);
	if (length == 0) {
		setExampleId(ex_id);
		return *this;
	} else { 
		if (!hasChild()) createChild();
		return getChild().addRule(rule,length,ex_id); 
	}
}

bool NFTRuleTrie::extendRule(const std::vector<int>& rule, int extension, int ex_id) {
	profile::start_timer(PROF_FINDSUBRULES);	
	if (!findSubrules(&rule[0],static_cast<int>(rule.size()),&extension,1)) {
		profile::stop_timer(PROF_FINDSUBRULES);
		// For some extensions, a subrule was found.
		// These extensions have been set to 0 in the ext-array.
		
		profile::start_timer(PROF_ADDRULE);
		// Create the new rule by inserting the extension.
		//  (maybe we shouldn't do this explicitly)
		std::vector<int> newrule;
		insert(extension,rule,newrule);
				
		// Add the new rule to the NFTRuleTrie (and set the example-number)
		//	std::cout << "   -> adding rule " << str(newrule) << " for example " << ex_id+1 << std::endl;
		addRule(&newrule[0],static_cast<int>(newrule.size()),ex_id);
		profile::stop_timer(PROF_ADDRULE);
		return true;
	} else {
		profile::stop_timer(PROF_FINDSUBRULES);
		return false;
	}
}

bool NFTRuleTrie::removeRule(const int* rule, unsigned int length) {
	removeRuleLength(length);
	if (length == 0) clearEndOfRule();
	else if (getChild().removeRule(rule,length)) {
		deleteChild();
	}
	return false;
}

void NFTRuleTrie::deleteChild() { delete child_; child_ = NULL; }

void NFTRuleTrie::createChild() { child_ = new NFTVNode(*this,true); }

//#ifdef ANODE_DLL
const NFTRuleTrie& NFTRuleTrie::getRoot() const { return *this; }
//#endif

bool NFTRuleTrie::findSubrules(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules) const {
	if (isEndOfRule()) {
		if (rules == NULL) { 
			return true;
		} else { 
			rules->push_back(const_cast<NFTRuleTrie*>(this));
			if (hasChild()) getChild().findSubrules(rule,length,rules);
			return true;
		}
	} else {
		if (hasChild()) return getChild().findSubrules(rule,length,rules);
		else return false;
	}
}

bool NFTRuleTrie::findSubrule(const int* rule, unsigned int length, int extension) const {
	if (hasChild()) return getChild().findSubrule(rule,length,extension);
	else return false;
}

bool NFTRuleTrie::findSubrules(const int* rule, unsigned int length, int* mc, unsigned int mcl) const {
	// current empty rule cannot match because it does not contain mc value
	if (mcl == 0) return true;
	if (hasChild()) return getChild().findSubrules(rule,length,mc,mcl);
	else return false;
}

bool NFTRuleTrie::findFailing(const int* rule, unsigned int length, std::vector<NFTTreeNode*>* rules, int maxl,unsigned int split) const {
	if (isEndOfRule()) {
		if (rules == NULL) return true;
		else {
			rules->push_back(const_cast<NFTRuleTrie*>(this));
			if (hasChild()) getChild().findFailing(rule,length,rules,maxl,split);
			return true;
		}
	}
	if (hasChild()) return getChild().findFailing(rule,length,rules,maxl,split);
	else return false;	
}

void NFTRuleTrie::listRules(std::vector<NFTTreeNode*>* rules) const {
	if (isEndOfRule()) {
		rules->push_back(const_cast<NFTRuleTrie*>(this));
	} else {
		if (hasChild()) return getChild().listRules(rules);
	}
}


int NFTRuleTrie::countFailing(const int* rule, unsigned int length, int maxl) const {
	if (isEndOfRule()) {
		if (hasChild()) return 1 + getChild().countFailing(rule,length,maxl);
		return 1;
	}
	if (hasChild()) return getChild().countFailing(rule,length,maxl);
	else return 0;	
}

void NFTRuleTrie::getRuleInner(std::vector<int>& rule,unsigned int split) const { /* left blank intentionally */ }

const NFTTreeNode* NFTRuleTrie::firstRule() const {
	if (isEndOfRule()) {
		return this;
	} else if (hasChild()) {
		return getChild().firstRule();
	} else {
		return NULL;
	}
}

const NFTTreeNode* NFTRuleTrie::nextRule(const int* rule, unsigned int length) const {
	if (hasChild()) {
		if (length == 0) return getChild().firstRule();
		return getChild().nextRule(rule,length);
	} else {
		return NULL;
	}
}

bool NFTRuleTrie::prune(unsigned int length) {
	if (removeRulesLength(length)) {
		if (hasChild()) 
			if (getChild().prune(length)) 
				deleteChild();
		return false;
	} else {
		return false;
	}
}

bool NFTRuleTrie::prune(const int* rule, unsigned int length) { 
	if (length == 0) {
		deleteChild();
	} else if (hasChild()) {
		if (getChild().prune(rule,length)) {
			deleteChild();
		}
	}
	return false;
}

/*
void NFTRuleTrie::getRedundant(std::set<TreeNode*>& rules) const {
	if (hasChild()) {
		getChild().getRedundant(rules);
	}
}

int NFTRuleTrie::countRedundant() const {
	std::set<TreeNode*> rules;
	getRedundant(rules);
	return rules.size();
}
*/
}
