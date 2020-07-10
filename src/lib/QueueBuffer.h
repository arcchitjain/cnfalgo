/*
 *  QueueBuffer.h
 *  cnfalgo
 *
 *  Created by Anton Dries on 14/04/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <deque>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>

template<class T>
class QueueBuffer {
public:
	typedef std::deque<T> buffer_type;
  private:
  
	
	unsigned int size;
	
	QueueBuffer* next;
	
	buffer_type buffer;
	
  public:
	
	QueueBuffer(unsigned int* sizes, int count) {
		size = sizes[0];
		if (count > 1) {
			next = new QueueBuffer<T>(sizes+1,count-1);
		} else {
			next = NULL;
		}
	}
	
	~QueueBuffer() {
		delete next;
		buffer.clear();
	}
	
	const T push(const T element) {
		if (buffer.size() < size) {
			buffer.push_back(element);
			return T(); 
		} else {
			T temp = buffer.front();
			buffer.pop_front();
			buffer.push_back(element);
			if (next != NULL) {
				return next->push(temp);
			} else {
				return temp;
			}
		}
	}
	
	const buffer_type& get_buffer(int index) const {
		if (index == 0) {
			return buffer;
		} else {
			assert(next != NULL);
			return next->get_buffer(index-1);
		}
	}
	
	

};

