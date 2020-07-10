#include "ExampleTrie.h"

using namespace cnf;

bool ExampleTrie::getExample(int index, std::vector<int>& example) const {
	triemap::const_iterator iter = examples_index.find(index);
	if (iter == examples_index.end()) {
		return false;
	} else {
		iter->second->getExample(example);
		return true;
	}
}

ExampleTrie::~ExampleTrie() {
	delete &lang;
	delete &examples_index;
}

void ExampleTrie::nodeAdded(int value,int index, SimpleTrie* node) {
	langAdd(value);
	nodeUpdated(-1,index,node);
}
			
void ExampleTrie::nodeRemoved(int value,int index) {
//	cerr << "calling ExampleTrie::nodeRemoved(" << value << ") " << endl;
	langDel(value);
	nodeUpdated(index,-1,NULL);
}

void ExampleTrie::nodeUpdated(int index,int newindex,SimpleTrie* node) {
	if (index == -1 && newindex == -1) {
	} else if (index == -1) { // just add a new example to the index
		examples_index.insert(std::make_pair(newindex,node));
//		cout << " - adding example with index " << newindex << endl;
	} else if (newindex == -1) { // just erase an example
		examples_index.erase(index);
	} else { // update the example
		nodeUpdated(index,-1,NULL);
		nodeUpdated(-1,newindex,node);
	}
}
			
void ExampleTrie::langAdd(int value) {
	std::map<int,int>::iterator iter = lang.find(value);
	
	if (iter == lang.end()) {
		lang.insert(std::make_pair(value,1));
		notifyListenersAdd(value, 1);
	} else {
		iter->second++;
		//lang.erase(value);
		//lang.insert(make_pair(value,iter->second+1));
	}
}
	
void ExampleTrie::langDel(int value) {
	std::map<int,int>::iterator iter = lang.find(value);
	
	if (iter == lang.end()) {
		
	} else {
		if (iter->second > 1) {
			iter->second--;
//			lang.insert(make_pair(value,iter->second-1));
		} else {
			//iter->second = 0;
			lang.erase(value);
			notifyListenersRemove(value, 1);	
		}
	}
}


/*
list<int>* ExampleTrie::complement(int* example, int length) {
	map<int,int>::iterator iter = lang->begin();
	list<int>* result = new list<int>();
	
	while (iter != lang->end()) {
		if (length == 0 || *example > iter->first) {
			result->push_back(iter->first);
			iter++;
		} else if (*example == iter->first) {
			example++; iter++; length--;
		} else {
			// this case should never occur since the example cannot contain 
			// elements that are not in the language
			cerr << "WARNING: finding complement of example reached an unexpected state " << endl;
			// however do continue
			example++; length--;
		}
	}
	
	return result;
}

void ExampleTrie::complement(int* example, int length, int** r, int* rl) {
	map<int,int>::iterator iter = lang->begin();
	
	list<int>* result = new list<int>();
	
	while (iter != lang->end()) {
		if (length == 0 || *example > iter->first) {
			result->push_back(iter->first);
			iter++;
		} else if (*example == iter->first) {
			example++; iter++; length--;
		} else {
			// this case should never occur since the example cannot contain 
			// elements that are not in the language
			cerr << "WARNING: finding complement of example reached an unexpected state " << endl;
			// however do continue
			example++; length--;
		}
	}
	
	*rl = result->size();
	*r = new int[*rl];
	
	list<int>::iterator it;
	
	for (it = result->begin(); it != result->end();  it++) {
		**r = *it;
		*r++;
	}
	
} */

/*
void ExampleStore::addExample(int index, int* example, int length) {
	pair<int,Trie*> addInfo = examples->add(index,example,length);
//	cerr << "addInfo.first = " << addInfo.first << endl;
	if (addInfo.first > 0)
		exampleindex->erase(addInfo.first);
	exampleindex->insert(make_pair(index,addInfo.second));
	
}
*/

bool ExampleTrie::remove(int index) {
	triemap::iterator iter = examples_index.find(index);
	
	if (iter != examples_index.end()) {
		iter->second->remove();
		examples_index.erase(iter);
		return true;
	} else {
		return false;
	}
}

void ExampleTrie::getElements(int*& el, int& ell) const {
	ell = getSize(); 
	el = new int[getSize()];
	
	std::map<int,int>::iterator iter;
	int i=0;
	for (iter = lang.begin(); iter != lang.end(); iter++) {
		*(el+i++) = iter->first;
	} 
}

void ExampleTrie::getElements(intarray& elements) const {
	for (std::map<int,int>::iterator iter = lang.begin(); iter != lang.end(); iter++) {
		elements.push_back(iter->first);
	} 
}


