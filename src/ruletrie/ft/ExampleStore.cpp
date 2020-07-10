/*
 *  ExampleStore.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 25/07/2010.
 *  Copyright 2010 Departement Computerwetenschappen, K.U. Leuven. All rights reserved.
 *
 */

#include "ExampleStore.h"
#include <iostream>

namespace cnf {

Example& ExampleStore::add(const int* example, unsigned int length) {
	if (length == 0) {
		setEnd();
	//	increaseCount();
		return *this;
	} else {
		Example* child = getChild(*example);
		if (child == NULL) child = addChild(*example);
		return child->add(example+1,length-1);
	}
}

Example* ExampleStore::find(const int* example, unsigned int length) const {
	if (length == 0) {
		if (isEnd()) return const_cast<Example*>(this); else return NULL;
	} else {
		const Example* child = getChild(*example);
		if (child == NULL) return NULL; 
		else return child->find(example+1,length-1);
	}
}

int ExampleStore::size() const {
	int result=0;
	for (ExampleMap::const_iterator i = getChildren().begin(); i!=getChildren().end(); ++i) {
		result += i->second->size();
	}
	if (getCount()>0) result++;
	return result;
}

void ExampleStore::getContent(std::vector<int>& content) const {
	if (!isRoot()) {
		getParent().getContent(content);
		content.push_back(getValue());
	}
}

void ExampleStore::removeChild(int value) {
	ExampleMap::iterator child = getChildren().find(value);
	if (child != getChildren().end()) {
//		std::cout << "erasing child " << value << std::endl;
		getChildren().erase(child);
		delete child->second;
	}
	if (empty() && !isRoot()) {
//		std::cout << "cleaning up node" << std::endl;
		getParent().removeChild(getValue()); 
	}
}

void ExampleStore::increaseCount() {
	count_++;
//	std::cout << "increased count to " << count_ << " for example " << this << std::endl;
}

void ExampleStore::decreaseCount() {
	count_--;
//	std::cout << "decreased count to " << count_ << " for example " << this << std::endl;
//	if (empty() && !isRoot()) std::cout << "cleaning up example" << std::endl;
	if (empty() && !isRoot()) getParent().removeChild(getValue());
}

bool ExampleStore::empty() const {
	return (getCount() == 0 && getChildren().empty());
}

void ExampleStore::show(std::string prefix) const {
//	std::cout << prefix << "NODE " << getValue() << " " << (isEnd() ? "END " : "NO END ") << getCount() << std::endl;
	for (ExampleMap::const_iterator child = getChildren().begin(); child != getChildren().end(); ++child) {
		child->second->show(prefix + "  ");
	}
}

}