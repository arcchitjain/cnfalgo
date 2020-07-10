#ifndef PROFILE_H_
#define PROFILE_H_

#include "memory.h"

#include <map>
#include <vector>
#include <iostream>
#include <string>

#define PROF_TOTAL "00 Total Time              "
#define PROF_FINDFAILING "01 Find Failing            "
#define PROF_QUICKREMOVE "02 Quick Remove            "
#define PROF_GENERATEEXT "03 Generate Rule Extensions"
#define PROF_FILTEREXT "04 Filter Rule Extensions  "
#define PROF_FINDSUBRULES "05 Find Subrules           "
#define PROF_ADDRULE "06 Add Rule                "
#define PROF_REMOVERULE "07 Remove Rule             "
#define PROF_RERUN "08 Re Run                   "
#define PROF_PRUNING "09 Pruning                  "
#define PROF_NEWADD "10 New Add                  "
#define PROF_FILTEREXA "11 Filter Exa              "

namespace profile {

class profile_timer {
	
private:
	double start_time;
	double total_time;
	int steps;

public:	
	profile_timer() : start_time(-1), total_time(0), steps(0) {}
	inline bool active() const { return start_time > -1; }
	inline double total() const { return total_time; }
	inline int times() const { return steps; }
	inline void start() { steps++; start_time = timeUsage();  }
	inline void count() { steps++; }
	inline void stop() { 
		if (active()) {
			double diff = timeUsage() - start_time;
			total_time += (diff >= 0) ? diff : 0;
			start_time = -1;
		}
	}
};

class Profile {

public:
	

	typedef std::string modetype;
	typedef std::string timertype;
	typedef std::string idtype;
	
	
private: 


	std::map<idtype,profile_timer> timers;
	
	std::vector<modetype> modes;
		
	inline idtype get_id(const modetype& mode, const timertype& timer) const {
		if (mode == "") return timer; else return mode + "/" + timer;
		//return ((idtype)mode<<(8*sizeof(timertype))) + (idtype)timer;
	}
	
	static Profile* instance_;
	
	
		
public:

	
	static inline Profile& instance() {
		if (!instance_) {
			instance_ = new Profile();
		}
		return *instance_;
	}

	Profile() {	modes.push_back(""); }
	
	inline modetype current_mode() const { return modes.back(); }

	inline profile_timer& get_timer(const timertype& timer) {
		idtype id = get_id(current_mode(),timer);
		std::map<idtype,profile_timer>::iterator t = timers.find(id);
		if (t == timers.end()) {
			timers.insert(std::make_pair(id,profile_timer()));
			t = timers.find(id);
		}
		return (t->second);		
	}

	inline void set_mode(const modetype& mode) {
		if (current_mode() != mode) {
			modes.push_back(mode);
		}
	}	
	
	inline void reset_mode() {
		if (modes.size() > 1) modes.pop_back();
	}
	
	inline void start(const timertype& timer) {
		//std::cout << "starting timer " << (int)timer << std::endl;
		get_timer(timer).start();
	}
	
	inline void count(const timertype& timer) {
		get_timer(timer).count();
	}

	inline void stop(const timertype& timer) {
		//std::cout << "stopping timer " << (int)timer << std::endl;
		get_timer(timer).stop();
	}
	
	inline void print() {
		std::map<idtype,profile_timer>::iterator stop = timers.end();
		for (std::map<idtype,profile_timer>::iterator i = timers.begin(); i != stop; ++i) {
			std::cout << i->first << " -- " << (i->second).total() << " (" << (i->second).times() << ")" << std::endl;
		}
	}
	
	inline int get_count(const modetype& mode, const timertype& counter) const {
		std::map<idtype,profile_timer>::const_iterator count = timers.find(get_id(mode,counter));
		if (count == timers.end()) {
			return 0; 
		} else {
			return (count->second).times(); 
		}
	}

};

//#define NPROFILE
#ifndef NPROFILE

inline Profile& get_profile() {
	return Profile::instance();	
}

inline void start_timer(const Profile::timertype& action) {
	get_profile().start(action);	
}

inline void stop_timer(const Profile::timertype& action) {
	get_profile().stop(action);
}

inline void count_operation(const Profile::timertype& action) {
	get_profile().count(action);
}

inline int get_count(const Profile::modetype& mode, const Profile::timertype& counter) {
	return get_profile().get_count(mode,counter);
}

inline void set_mode(const Profile::modetype& mode) {
	get_profile().set_mode(mode);
}

inline void reset_mode() {
	get_profile().reset_mode();
}

inline void print_profile() {
	std::cout << "\nExecution profile: " << std::endl;
	get_profile().print();
    std::cout << std::endl;
}

#else

inline void start_timer(const Profile::timertype& action) {}

inline void stop_timer(const Profile::timertype& action) {}

inline void count_operation(const Profile::timertype& action) {}

inline void set_mode(const Profile::modetype& mode) {}

inline void reset_mode() {}

inline int get_count(const Profile::modetype& mode, const Profile::timertype& counter) { return 0;}

inline void print_profile() {}
#endif

} // end namespace profile 
#endif /*PROFILE_H_*/
