#ifndef EXAMPLETRIETOP_H_
#define EXAMPLETRIETOP_H_

#include "SimpleTrie.h"
#include "../language/Language.h"
#include "ExampleContainer.h"

namespace cnf
{

class ExampleTrie : public cnf::SimpleTrie, public cnf::Language, public ExampleContainer {
	
private :

	
	/* Add the given symbol to the language (adding one occurence). */
	void langAdd(int value);
	
	/* Removes the given symbol from the language (subtracting one occurence). */
	void langDel(int value);
			
	/* Currently used symbols as a map<symbol,number of nodes for this symbol> */
	std::map<int,int>& lang;

	/* The index of the examples */
	triemap& examples_index;

	/* Updates the language statistics and index. */
	void nodeAdded(int value, int index, SimpleTrie* node);

	/* Updates the language statistics and index. */			
	void nodeRemoved(int value, int index);
			
	/* Updates the language statistics and index. */
	void nodeUpdated(int index, int newindex, SimpleTrie* node);

protected:

	virtual void getExample(std::vector<int>& example) const { /* left blank intensionally */ }

public:
	
	/*
		Default constructor.
		Uses the superclass constructor with value 0 (since it does contain a real node) and parent NULL (it's the top node)	
	*/
	ExampleTrie() : SimpleTrie(0,NULL), lang(*new std::map<int,int>), examples_index(*new triemap()) {}	

	virtual ~ExampleTrie();
	
	/*
		Returns an ordered list of symbols that are present in the trie but not in the given example.
	*/
//	list<int>* complement(int* example, int length);
	

	/*
		Return a full representation of the trie as dot.
	*/
	std::string toDot() {
		return "digraph Trie {\n" + SimpleTrie::toDot("") + "}\n";
	}
	
	virtual unsigned int size() { return static_cast<int>(examples_index.size()); }

	/*
		Test.
		Prints the symbols and the number of nodes for this symbol.
	*/
	void printLanguage() {
		std::cerr << "printing language" << std::endl;
		std::map<int,int>::iterator iter = lang.begin();
	   	while (iter != lang.end()) {
			std::cerr << iter->first << " -- " << iter->second  << std::endl;
			iter++;
		};
	}
	
	/*
		The top node should never be removed and is therefore never empty.
	*/
	inline bool empty() { return false; }
	
	/*
		Removes the example with the given index from the trie.
		If the example did not exist, return false, otherwise true.
	*/
	virtual bool remove(int index);
	
	/*
		Returns an ordered array with the elements of the example.
	*/
	virtual bool getExample(int index, std::vector<int>& example) const;
	
	virtual int getFirst() { 
		triemap::iterator first = examples_index.begin();
		if (first == examples_index.end()) 
			return -1;
		else 
			return first->first;
	}
	
	virtual int getLast() {
		triemap::reverse_iterator last = examples_index.rbegin();
		if (last == examples_index.rend()) 
			return -1;
		else 
			return last->first;
			
	}
	
	/* 
	 * 	<!-- Language
	 */
	
	virtual inline unsigned int getSize() const { return static_cast<int>(lang.size()); }
	
	virtual void getElements(int*& el, int& ell) const ;
	
	virtual void getElements(intarray& elements) const ;
	
	virtual bool add(const std::vector<int>& example, int index) {
		return SimpleTrie::add(index,&example[0], static_cast<int>(example.size())); 
	} 
	
	//using Trie::add;
	
	/*
	 * -->
	 */
	
	virtual inline int getOccurence(int element) const {
		return (lang.find(element) != lang.end());	
	}
	
};

}

#endif /*EXAMPLETRIETOP_H_*/



