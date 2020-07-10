/*
	Filename	: lib/command.h
	Author		: Anton Dries
	Description	: command line parsing library
	
	Long Description:
		This file contains code for parsing the command line arguments provided to the application.
		This code stores the settings in a std::map of strings.
		Usage of the code is typically as follows:
	
		1. Create a map for the values:
			std::map<std::string,std::string> cmd;
			
		2. Initialize the map:
			init_cmd(cmd, "a:b:cd:"); // colon (:) means that a, b and d expect an additional value
			
			// or manually add some parameters
			cmd.insert(std::make_pair<std::string,std::string>("a",""));
			cmd.insert(std::make_pair<std::string,std::string>("b",""));

			// argument without value (insert string "NULL" as value)
			cmd.insert(std::make_pair<std::string,std::string>("c","NULL"));	

			// argument with long name
			cmd.insert(std::make_pair<std::string,std::string>("-longname","")); 
			
		3. Parse the command line:
			if (!read_command_line(argc,argv,cmd)) {
				// invalid command line arguments
			}
			
		4. Read the values:
		
			double a = read_param_default("a",cmd,0.0);		// default value 0
			std::string b;
			if (!read_param("b","-longname",cmd,b)) {
				// parameter b was not given
			}
			bool c = read_param("c",cmd);
			
			// and read unnamed arguments
			std::string filename = read_param_default("arg_0",cmd,"_");
			
		There are methods for reading std::string, int, double and boolean parameters.
					
*/

#ifndef COMMAND_H_
#define COMMAND_H_

#include "functions.h"
#include <map>
#include <string>
#include <iostream>

/*
	Reads command line and extracts the parameters.
	
	@param	argc	length of command line array
	@param	argv	command line array
	@param	pars	map containing parameter definitions

	@return		true if parsing the command line was successful
			Parsing fails if :
				- an unknown parameter is encountered (everything starting with '-' is considered as parameter
				- a parameter expecting a value has a value starting with '-'
	@return		pars will contain parameters values
					for on/off arguments it will contain 'true' if it is present 'NULL' otherwise
					unnamed parameters will be included as 'arg_0', 'arg_1', ...

*/
inline bool read_command_line(int argc, char* argv[],std::map<std::string,std::string>& pars) {
	int p=0;
	for (int i=1; i<argc; i++) {
		std::string arg = argv[i];
		if (arg[0] == '-') {
			std::map<std::string,std::string>::iterator it = pars.find(arg.substr(1));
			if (it == pars.end()) { 
				std::cerr << "unexpected command line parameter: " << arg << std::endl;
				return false; 
			}
			if (it->second == "NULL") it->second = "true";
			else if (++i < argc && argv[i][0] != '-') {
				it->second = argv[i];
			} else {
				std::cerr << "value expected for parameter: " << arg << std::endl;
				return false;
			}
		} else {
			pars.insert(make_pair("arg_" + str(p++) ,arg));
		}
	}
	return true;
	
}

inline void init_cmd(std::map<std::string,std::string>& cmd, std::string parameters) {
	for (unsigned int i=0; i<parameters.size(); ++i) {
		char c = parameters[i];
		if (i < parameters.size() -1 && parameters[i+1] == ':') {
			cmd.insert(std::make_pair<std::string,std::string>(std::string(&c,1),""));
			++i;
		} else {
			cmd.insert(std::make_pair<std::string,std::string>(std::string(&c,1),"NULL"));
		}	
	}
}

/* String parameters */

inline bool read_param(const std::string& name, std::map<std::string,std::string>& pars, std::string& value) {
	std::map<std::string,std::string>::iterator iter = pars.find(name);
	if (iter == pars.end()) return false;
	else {
		if (iter->second != "") {
			value = iter->second;
			return true;
		} else {
			return false;
		}	
	}
}

inline std::string read_param_default(const std::string& name, std::map<std::string,std::string>& pars, const std::string& defvalue) {
	std::string value;
	if (read_param(name,pars,value)) {
		return value;
	} else {
		return defvalue;
	}
}

inline std::string read_param_default(const std::string& shortname, const std::string& longname, std::map<std::string,std::string>& pars, const std::string& defvalue) {
	std::string value;
	if (read_param(shortname,pars,value) || read_param(longname,pars,value)) {
		return value;
	} else {
		return defvalue;
	}
}

inline bool read_param(const std::string& nameshort, const std::string& namelong, std::map<std::string,std::string>& pars, std::string& value) {
	return read_param(nameshort,pars,value) || read_param(namelong,pars,value);	
}

/* Int parameters */

inline bool read_param(const std::string& name, std::map<std::string,std::string>& pars, int& value) {
	std::string temp;
	if (read_param(name,pars,temp)) { 
		value = atoi(temp.c_str());
		return true;
	} else {
		return false;
	}
}

inline int read_param_default(const std::string& name, std::map<std::string,std::string>& pars, int defvalue) {
	int value = 0;
	if (read_param(name,pars,value)) {
		return value;
	} else {
		return defvalue;
	}
}

inline int read_param_default(const std::string& shortname, const std::string& longname, std::map<std::string,std::string>& pars, int defvalue) {
	int value = 0;
	if (read_param(shortname,pars,value) || read_param(longname,pars,value)) {
		return value;
	} else {
		return defvalue;
	}
}

inline bool read_param(const std::string& nameshort, const std::string& namelong, std::map<std::string,std::string>& pars, int& value) {
	return read_param(nameshort,pars,value) || read_param(namelong,pars,value);	
}

/* Double parameters */

inline bool read_param(const std::string& name, std::map<std::string,std::string>& pars, double& value) {
	std::string temp;
	if (read_param(name,pars,temp)) { 
		value = atof(temp.c_str());
		return true;
	} else {
		return false;
	}
}

inline double read_param_default(const std::string& name, std::map<std::string,std::string>& pars, double defvalue) {
	double value = 0;
	if (read_param(name,pars,value)) {
		return value;
	} else {
		return defvalue;
	}
}

inline double read_param_default(const std::string& shortname, const std::string& longname, std::map<std::string,std::string>& pars, double defvalue) {
	double value = 0;
	if (read_param(shortname,pars,value) || read_param(longname,pars,value)) {
		return value;
	} else {
		return defvalue;
	}
}

inline bool read_param(const std::string& nameshort, const std::string& namelong, std::map<std::string,std::string>& pars, double& value) {
	return read_param(nameshort,pars,value) || read_param(namelong,pars,value);	
}


/* Boolean parameters */

inline bool read_param(const std::string& name, std::map<std::string,std::string>& pars) {
	std::string temp;
	if (read_param(name,pars,temp) && temp == "true") return true; else return false;
}

inline bool read_param(const std::string& nameshort, const std::string& namelong, std::map<std::string,std::string>& pars) {
	return read_param(nameshort,pars) || read_param(namelong,pars);	
}

inline void shift_array(int& len, char**& arr) {
	len--;
	arr++;
}


#endif /*COMMAND_H_*/
