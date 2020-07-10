/* 
Filename: exampletrie.cpp
Author: anton dries (anton@cs.kuleuven.be)
Original version: 29-03-2007
*/


#include "SimpleTrie.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include "../lib/functions.h"

using namespace cnf;


bool SimpleTrie::add(int index, const int* rule, int length) {
	if (length == 0) {
		int oldindex = this->index;
		this->index = index;
		nodeUpdated(oldindex,index,this);
		return (oldindex == -1);
	} else {
		triemap::iterator iter = children->find(*rule);
		if (iter == children->end()) {
			SimpleTrie* child = new SimpleTrie(*rule,this);
			children->insert(std::make_pair(*rule,child));
			nodeAdded(*rule,-1,child);
			return child->add(index,++rule,--length);
		} else {
			return iter->second->add(index,++rule,--length);	
		}
	}

}



bool SimpleTrie::remove(const int* example, int length) {
	if (length == 0) {
		index = -1;
		if (empty()) return true; else return false;
	} else {
		triemap::iterator child = children->find(*example);
		if (child == children->end()) return false;
		else {
			if (child->second->remove(example+1,length-1)) {
				if (child->second->empty()) {
					delete child->second;
					children->erase(child);
					return true;
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
	}
}
			
void SimpleTrie::remove() {
	index = -1;
	if (empty()) {
		nodeRemoved(value,index);
		parent->removeChild(value);
		delete this;
	}
}
				
SimpleTrie* SimpleTrie::find(int* rule, int length) {
	if (length == 0)
		return this;
	else {
		triemap::iterator iter = children->find(*rule);
	
		if (iter == children->end()) {
			return NULL;
		} else {
			return iter->second->find(++rule,--length);	
		}
	}
}

void SimpleTrie::removeChild(int value) {
	children->erase(value);
	if (empty())
		remove();
}

std::string SimpleTrie::toDot(std::string id) {
	std::string result = "";
	
	std::string nid = id + str(value);
	if (id != "") result += id + " -> " + nid + ";\n";
	result += nid + " [label=\"" + str(value);
	if (index > -1) result += "\\n" + str(index) + "\" shape=box ]; \n";
	else result += "\"]";
	
	triemap::iterator iter = children->begin();
   	while (iter != children->end()) {
		result += iter->second->toDot(nid);
		iter++;
	};
	

	
	return result;
	
}

void SimpleTrie::nodeAdded(int value, int index, SimpleTrie* node) {
	parent->nodeAdded(value,index, node);
}

void SimpleTrie::nodeRemoved(int value,int index) {
//	cerr << "calling SimpleTrie::nodeRemoved(" << value << ") on " << parent << endl;
	parent->nodeRemoved(value,index);
}

void SimpleTrie::nodeUpdated(int index,int newindex, SimpleTrie* node) {
	parent->nodeUpdated(index,newindex,node);
}


SimpleTrie::~SimpleTrie() {
	value = 0;
	index = -1;
	//require(children->empty(), "children not empty upon destruction");
	delete children;
	parent = NULL;
}


void SimpleTrie::getExample(std::vector<int>& example) const {
	getParent().getExample(example);
	example.push_back(value);
}


