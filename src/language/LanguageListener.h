#ifndef LANGUAGELISTENER_H_
#define LANGUAGELISTENER_H_

namespace cnf
{

class LanguageListener
{
public:
	LanguageListener() {}
	virtual ~LanguageListener() {}
	
	virtual void languageElementAdded(int element, int ex_id) = 0;
	
	virtual void languageElementRemoved(int element, int ex_id) = 0;
};

}

#endif /*LANGUAGELISTENER_H_*/
