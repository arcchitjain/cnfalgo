#ifndef NODEMAP_H_
#define NODEMAP_H_

#include <map>
#include <vector>
#include <iostream>

namespace cnf { 

class ANode;

typedef std::pair<unsigned int, ANode*> basetype;
typedef std::vector<basetype> basecontainer;



class VectorNodeMap : public basecontainer {
	
private:
	inline int find_position(unsigned int key) const {
		if (empty()) return -1;
		if (front().first > key || back().first < key) return -1;
		int min = 0; int max = size(); 
		while (max >= min) {
			int mid = (min+max)/2;
			if ((*this)[mid].first == key) {
				return mid;
			} else if (key < (*this)[mid].first) {
				max = mid - 1;
			} else {
				min = mid + 1;
			}
		}
		return -1;
	}
	
	inline int insert_position(unsigned int key) const {
		if (empty()) return 0;		if (front().first >= key) return 0;		if (back().first <= key) return size();		int min = 0; int max = size();
		int mid = (min+max)/2;		while (max >= min) {
			//for (int i=0; i<size(); i++) std::cout << (*this)[i].first << " "; std::cout << std::endl;
			if ((*this)[mid].first == key) {			        return mid;			} else if (key < (*this)[mid].first) {			        max = mid - 1;			} else {			        min = mid + 1;			}
			mid = (min+max)/2;        }		return (((*this)[mid].first < key) ? mid+1 : mid) ;	}
	
	
	
public:
	VectorNodeMap() {} // : std::vector<std::pair<unsigned int, ANode*> >() {}
	
	~VectorNodeMap() {}
	
	
	inline basecontainer::iterator find(unsigned int key) {
	//	std::cout << "looking A for " << key << " in ";
	//	for (unsigned int i=0; i<size(); i++) std::cout << (*this)[i].first << " "; std::cout << std::endl;
		int pos = find_position(key);
	//	std::cout << "--> " << pos << std::endl;
		if (pos == -1) return end();
		else return begin() + pos;
	}
	
	
	inline basecontainer::const_iterator find(unsigned int key) const {
	//	std::cout << "looking B for " << key << " in ";
	//	for (std::vector<std::pair<unsigned int, ANode*> >::const_iterator i = begin(); i != end(); i++) std::cout << (*i).first << " "; std::cout << std::endl;
		int pos = find_position(key);
	//	std::cout << "--> " << pos << std::endl;
		if (pos == -1) return end();
		else return begin() + pos;
	}
	
	inline basecontainer::iterator lower_bound(unsigned int key) {
		int pos = insert_position(key);
		if (pos == -1) return end();
		else return begin() + pos;
	}
	
	inline basecontainer::const_iterator lower_bound(unsigned int key) const {
		int pos = insert_position(key);
		if (pos == -1) return end();
		else return begin() + pos;
	}
	
	inline basecontainer::iterator upper_bound(unsigned int key) {
		int pos = insert_position(key);
		if (pos == -1) return end();
		else return begin() + (pos+1);
	}
	
	inline basecontainer::const_iterator upper_bound(unsigned int key) const {
		int pos = insert_position(key);
		if (pos == -1) return end();
		else return begin() + (pos+1);
	}
	
	inline void insert(const basetype& item) {
	//	std::cout << "inserting " << item.first << " in ";
	//	for (unsigned int i=0; i<size(); i++) std::cout << (*this)[i].first << " "; std::cout << std::endl;
		
		int pos = insert_position(item.first);
	//	std::cout << "--> " << pos << std::endl;
		basecontainer::insert(begin()+pos,item);
		
	//	std::cout << "resulted in ";
	//	for (unsigned int i=0; i<size(); i++) std::cout << (*this)[i].first << " "; std::cout << std::endl;
				
	}
	
};

typedef std::map<unsigned int, ANode*> NodeMap;  // slightly faster 15 %
//typedef VectorNodeMap NodeMap;						// slightly less memory 30 % BUT DOES NOT WORK

}

#endif /*NODEMAP_H_*/
