#ifndef DYNAMICCONTAINER_H_
#define DYNAMICCONTAINER_H_

#include <vector>
#include <map>
#include <cassert>

namespace cnf {

class ANode;

typedef std::vector<std::pair<unsigned int,ANode*> > vectype;
typedef std::map<unsigned int,ANode*> maptype;

class DynamicIterator {
	
private:
	maptype::iterator mapiter_;
	vectype::iterator veciter_;
	
public:
	DynamicIterator(maptype::iterator& mapiter) : mapiter_(mapiter), veciter_(NULL) {}
	
	DynamicIterator(vectype::iterator& veciter) : veciter_(veciter), mapiter_(NULL) {}
	
	
	
};

class DynamicContainer {
	
	vectype* base_vec;
	maptype* base_map;
	
	unsigned int thresh_up;
	unsigned int thresh_dn;
	
public:
	
	DynamicContainer() : base_vec(new vectype()), base_map(NULL),
			thresh_up(12), thresh_dn(8) {}
	
	~DynamicContainer() { delete base_vec; delete base_map; }
	
	inline bool useVector() const { return base_vec != NULL; } 	
	
	inline bool switchUp() {
		assert(useVector());
		if (base_vec->size() > thresh_up) {
			base_map = new maptype(base_vec->begin(), base_vec->end());
			delete base_vec; base_vec = NULL;
			return true;
		} else {
			return false;
		}
	}
	
	inline bool switchDown() {
		assert(!useVector());
		if (base_map->size() < thresh_dn) {
			base_vec = new vectype(base_map->begin(), base_map->end());
			delete base_map; base_map = NULL;
			return true;
		} else {
			return false;
		}
	}
	
	inline DynamicIterator begin() { 
		if (useVector()) return base_vec->begin(); else base_map->begin(); 
	}
	
	inline void insert(std::pair<unsigned int,ANode*>& element) {
		switchUp();
		if (useVector() && !switchUp()) {
			std::vector<std::pair<unsigned int, ANode*> >::iterator i = base_vec->begin();
			 
		} else {
			base_map->insert(element);
		}
	}
	
	
	
};

}

#endif /*DYNAMICCONTAINER_H_*/
