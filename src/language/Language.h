#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#include <list>
#include <vector>
#include "LanguageListener.h"
#include "../lib/functions.h"


namespace cnf
{

class Language
{

private:
	
	std::vector<LanguageListener*> listeners;
	
protected:

	void notifyListenersAdd(int element, int ex_id) {
		std::vector<LanguageListener*>::iterator iter;
		for (iter = listeners.begin(); iter != listeners.end(); iter++) {
			(*iter)->languageElementAdded(element, ex_id);	
		}	
	}
	
	void notifyListenersRemove(int element, int ex_id) {
		std::vector<LanguageListener*>::iterator iter;
		for (iter = listeners.begin(); iter != listeners.end(); iter++) {
			(*iter)->languageElementRemoved(element, ex_id);	
		}	
	}

public:
	Language() {}
	virtual ~Language() {}
	
	/* Returns possible extensions of any rule based so that does not cover the example.
	 * 
	 * For example:
	 *  if example = {abc} and lang =  {abcdef} then the list
	 *    { a,b,c,-d,-e,-f } is returned
	 * 
	 */
	virtual void extensions(const std::vector<int>& example, std::vector<int>& extensions) const;
	
	// virtual void extensions(int* example, int length, intarray& extensions) const;
	
	virtual bool add(const std::vector<int>& example, int ex_id) = 0;
	
	virtual unsigned int getSize() const = 0;
	
	virtual void getElements(std::vector<int>& elements) const = 0;
	
	void addListener(LanguageListener* l) {
		listeners.push_back(l);	
	}
	
	void removeListener(LanguageListener* l) {
		for (std::vector<LanguageListener*>::iterator i = listeners.begin(); i != listeners.end(); i++) {
			if (*i == l) {
				listeners.erase(i);
				return;	
			}	
		}
	}
	
	virtual int getOccurence(int element) const = 0;
	
	
};

}

#endif /*LANGUAGE_H_*/
