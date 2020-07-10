/*
 *  ExampleStore.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 25/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#ifndef EXAMPLESTORE_H_
#define EXAMPLESTORE_H_

#include <map>
#include <vector>

namespace cnf {

class ExampleStore;

typedef ExampleStore Example;
typedef std::map<int, Example*> ExampleMap;

class ExampleStore {

  private :
  
	ExampleMap children_;
  
	bool end_flag_;
	
	int count_;
	
	int value_;

    ExampleStore* parent_;
		
  protected :
  
	ExampleStore(int value, ExampleStore& parent) :  end_flag_(false), count_(0), value_(value), parent_(&parent) {}
  
	inline bool isEnd() const { return end_flag_; }
	inline void setEnd() { end_flag_ = true; }
	inline void unsetEnd() { end_flag_ = false; }
	
	inline int getCount() const { return count_; }
	
	inline const ExampleMap& getChildren() const { return children_; }
	inline ExampleMap& getChildren() { return children_; }
	inline Example* getChild(int value) {
		ExampleMap::iterator child = getChildren().find(value);
		return (child == getChildren().end()) ? NULL : child->second;
	}	
	inline const Example* getChild(int value) const {
		ExampleMap::const_iterator child = getChildren().find(value);
		return (child == getChildren().end()) ? NULL : child->second;
	}
	
	inline bool isRoot() const { return (parent_ == NULL); }
	inline ExampleStore& getParent() const { return *parent_; }
	
	inline int getValue() const { return value_; }
	
	bool empty() const;

  public :
  
	ExampleStore(int value=0) : end_flag_(false), count_(0), value_(value), parent_(NULL) {}
  
	Example& add(const int* example, unsigned int length);

	Example* find(const int* example, unsigned int length) const;
	
	void getContent(std::vector<int>& content) const;
	
	void increaseCount();
	
	void decreaseCount();
	
	int size() const;

	void show(std::string prefix) const;
		
  private :
  
	Example* addChild(int value) {
		Example* child = new Example(value,*this);
		getChildren().insert(std::pair<int,Example*>(value,child));
		return child;
	}
	
	void removeChild(int value);
	


};


}

#endif