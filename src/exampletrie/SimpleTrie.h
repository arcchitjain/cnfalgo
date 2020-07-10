/* 
Filename: exampletrie.h
Author: anton dries (anton@cs.kuleuven.be)
Original version: 29-03-2007
*/


#ifndef TRIE_H
#define TRIE_H

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include "ExampleContainer.h"

// #include "ExampleStore.h"

namespace cnf {

class SimpleTrie;

typedef std::map<int,SimpleTrie*> triemap;

/*
This class describe the nodes of a trie-structure that is used to store the examples in the current window.
It's main methods are:
	* add(index,example) to add the given rule with the given index
	* find(example) to find a specific example in the structure
Removing a rule can be done by calling the remove() method on the example found with the find method.

Every node has several children of the same class Trie.
*/
class SimpleTrie {
		private :

			/* The parent of the current node. */
			SimpleTrie* parent;
			
			/* The value of the node (>=0). */
			int value;
	
			/* 
				The index of the example represented by the node.
				Larger than or equal to 0 if the node is the end of an example, -1 otherwise.
			*/
			int index;
	
			/* The children of the current node. */
			triemap* children;
			

			/* 
				Removes the child with the given value from the list of children.
				Calls this.remove() if this was the last child and the index == 0, causing a cascade through the trie.
			*/
			void removeChild(int value);
			
			/*
				To be called when a node is added.
				Standard behavior is passing this on to the parent.
			*/
			virtual void nodeAdded(int value, int index, SimpleTrie* node);
			
			/*
				To be called when a node is removed.
				Standard behavior is passing this on to the parent.
			*/
			virtual void nodeRemoved(int value, int index);
			
			/*
				To be called when the index of a node is modified.
				Standard behavior is passing this on to the parent.
			*/
			virtual void nodeUpdated(int index, int newindex, SimpleTrie* node);		
	
			virtual bool hasParent() const { return parent != NULL; }
	
			virtual SimpleTrie& getParent() { return *parent; }
			
			virtual const SimpleTrie& getParent() const { return *parent; }
	
	public :
			
			/*
				The default constructor for a node in the Trie.
				This constructor intializes the node with the given value as a child of the given parent node.
			*/
			SimpleTrie(int v, SimpleTrie* p) : parent(p), value(v), index(-1), children(new triemap) {}
	
			
			
			/* 
				Adds a rule to the current structure.
				Precondition: the elements of the rule are ordered
				Precondition: the node was selected correctly (the previous element of the rule == value of the node)
				
				Result: true if a new example was actually added to the tree, false if the example was already there and only the index was updated.
			*/
			virtual bool add(int index, const int* rule, int length);
			
			/*
				Finds the given rule in the trie and returns its final node (if it exists, NULL otherwise).
			*/
			SimpleTrie* find(int* rule, int length);
			
			/*
				Returns if the current node is empty.
				A node is empty if its index == -1 and it has no children.
			*/
			virtual inline bool empty() { return (children->empty() && index == -1); }
			
			/*
				Returns a representation of the Trie in GraphViz-dot-format.
			*/
			virtual std::string toDot(std::string id);
					
			/*
				Sets the index of the node to 0.
				If the node has no children it calls removeChild(this) on the parent node and removes itself from memory.
				Calling this method creates a cascade through the trie-branch removing all empty ancestor nodes.
			*/
			void remove();
			
			/*
				Returns an ordered array with the elements of the example.
			*/
			virtual void getExample(std::vector<int>& example) const;
			
			/*
				Destructor.
				Disconnects all links and removes children.
			*/
			virtual ~SimpleTrie();
			
			virtual bool remove(const int* example, int length); 
			

};

}
#endif
