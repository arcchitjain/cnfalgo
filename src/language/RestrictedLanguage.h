#ifndef RESTRICTEDLANGUAGE_H_
#define RESTRICTEDLANGUAGE_H_

#include "Language.h"

namespace cnf
{

class RestrictedLanguage : public Language {
	
private:

	std::vector<int> language_;
	
protected:

	int getElement(int index) const { return language_[index]; }
	
public:
	RestrictedLanguage(std::vector<int>& language) : language_(language) {}
	
	virtual ~RestrictedLanguage() {
		// delete[] rule; 
	}
	
	virtual bool add(const std::vector<int>& example, int ex_id) { return false; }
	
	virtual inline unsigned int getSize() const { return static_cast<int>(language_.size()); } 
	
	virtual void getElements(std::vector<int>& example) const { 
		example = language_;
	}
	
	virtual void extensions(const std::vector<int>& example, std::vector<int>& extensions) const;
	
	virtual inline int getOccurence(int element) const {
		for (unsigned int i=0; i<getSize();i++) if (getElement(i) == element) return 1;
		return 0;	
	}
};

}

#endif /*RESTRICTEDLANGUAGE_H_*/
