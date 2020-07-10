#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

#include "functions.h"

// color codes
// black 	30
// red		31
// green	32
// yellow	33
// blue		34
// magenta	35
// cyan		36
// white	37

#ifndef W_COL
	#define W_COL "\E[32m"
#endif

#ifndef E_COL
	#define E_COL "\E[31m"
#endif 

#ifndef D_COL
	#define	D_COL "\E[34m"
#endif

#ifndef END_COL
	#define END_COL "\E[0m"
#endif

extern int _verbose; 

inline void print_message(int verbosity, const std::string& message,const char* file = "", int line = -1) {
	if (_verbose >= verbosity) {
		if (strcmp(file,"") && line != -1)
//		if (file != "" && line != -1)
			fprintf(stdout, "%s%s (%s:%i)%s",
				D_COL,message.c_str(),file,line,END_COL);		
		else
			fprintf(stdout, "%s%s%s",
				D_COL,message.c_str(),END_COL);
	}
}


inline void print_message_nl(int verbosity, const std::string& message,const char* file = "", int line = -1) {
	if (_verbose >= verbosity) {
		if (strcmp(file,"") && line != -1)	
//		if (file != "" && line != -1)
			fprintf(stdout, "%s%s (%s:%i)%s\n",
				D_COL,message.c_str(),file,line,END_COL);		
		else
			fprintf(stdout, "%s%s%s\n",
				D_COL,message.c_str(),END_COL);
	}}



inline void print_warning(const std::string& warning,const std::string& extra = "", const char* file = "", int line = -1) {
	if (_verbose >= 0) {
		if (strcmp(file,"") && line != -1)
//		if (file != "" && line != -1)
			fprintf(stdout, "%sWarning : %s %s (%s:%i)%s\n",
				W_COL,warning.c_str(),extra.c_str(),file,line,END_COL);		
		else
			fprintf(stdout, "%sWarning : %s %s%s\n",
				W_COL,warning.c_str(),extra.c_str(),END_COL);
	}
}

inline void warning_not_implemented(const std::string& name, const char* file = "", int line = -1) {
	print_warning("Method not implemented:", name,file,line); 
}

inline void print_error(const std::string& error, const std::string& extra = "", const char* file = "", int line = -1) {
	if (_verbose >= -1) {
		if (strcmp(file,"") && line != -1)
//		if (file != "" && line != -1)
			fprintf(stderr, "%sError : %s %s (%s:%i)%s\n",
				E_COL,error.c_str(),extra.c_str(),file,line,END_COL);		
		else
			fprintf(stderr, "%sError : %s %s%s\n",
				E_COL,error.c_str(),extra.c_str(),END_COL);
	}
}

inline void print_error_if(bool test, const std::string& error, const std::string& extra = "", const char* file = "", int line = -1) {
	if (test) print_error(error,extra,file,line);	
}

extern int _tracedepth;

inline const std::string trace_prefix() {
	std::string result = "";
	for (int i=0; i<_tracedepth;i++) {
		result += "  ";
	}
	return result;
}

inline void print_trace(bool traceid, const std::string& trace, const std::string& extra = "") {
	#ifndef NO_TRACE
		fprintf(stdout, "%s%sTrace : %s %s%s\n",
				E_COL,trace_prefix().c_str(),trace.c_str(),extra.c_str(),END_COL);
	#endif /* NO_TRACE */
}

inline void trace_enter_function(bool traceid, const std::string& function) {
	_tracedepth++;
	print_trace(traceid,"Entering function",function);
}

inline void trace_leave_function(bool traceid, const std::string& function) {
	_tracedepth--;
	print_trace(traceid,"Leave function",function);	
}

inline void trace_success(bool traceid) {
	print_trace(traceid,"  ==> SUCCESS !");	
}

inline void print_array(bool traceid, int* r, int rl) {
	for (int i=0; i<rl;i++) { fprintf(stdout, "%i ",*(r+i)); } 
	fprintf(stdout, "\n");
}

#endif /*MESSAGE_H_*/
