/*!  
   \file Log.h 

   $Id$
   $URL$
   
   \version   $Rev$
   \author    Anton Dries
   \date      15/02/08
   \namespace cnfalgo
*/
#ifndef cnfalgoLog
#define cnfalgoLog

#include <map>

#include <fstream>
#include <iostream>

// namespace logging {

class Log
{

  private:

	std::ostream* f;
	
	static std::map<std::string,Log*>* logs;
	
	static inline void initLogs() {
		logs = new std::map<std::string,Log*>();
		logs->insert(std::make_pair("_", new Log()));
		logs->insert(std::make_pair("*", new Log(std::cout)));
	}
	
	static inline std::map<std::string,Log*>& getLogs() {
		if (logs == NULL) initLogs();
		return *logs;
	}
	
	static inline Log& noLog() {
		return *getLogs()["_"];
	}
	
	static inline Log& defLog() {
		return *getLogs()["*"];
	}
	
  public:

	Log() : f(NULL) { }
	
	Log(std::ostream& of) : f(&of) {}

	~Log() {}
	
	static Log& instance(const std::string& area) {
		Log* r = getLogs()[area];
		if (r == NULL) return noLog();
		else return *r;
	}
	
	static void enableLog(const std::string& area, Log* log = NULL) {
		if (log == NULL) {
			logs->insert(std::make_pair(area, &defLog()));
		} else {
			logs->insert(std::make_pair(area, log));
		}
	}
	
	static void disableLog(const std::string& area) {
		logs->insert(std::make_pair(area, &noLog()));
	}
	
	template<typename type>
	inline Log& operator<< (type& val) {
		if (f != NULL) {
			*f << val;
		}
		return *this;
	}
	
	inline void redirect(std::ostream& of) {
		f = &of;
	}
	
};


inline Log& log(std::string area) {
	return Log::instance(area);
}

inline void enableLog(const std::string& area, Log* log = NULL) {
	Log::enableLog(area, log);
}

inline void disableLog(const std::string& area) {
	Log::disableLog(area);
}


// }  // end namespace cnfalgo

#endif // cnfalgoLog
