#ifndef MEMORY_H_
#define MEMORY_H_

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "sys/resource.h"
#include <iostream>

inline void skipspaces(std::FILE* file, int nb) {
    int ctr = 0;
    int ch = std::fgetc(file);
    while (ctr < nb) {
        if (ch == ' ') ctr++;
        if (ctr < nb) ch = std::fgetc(file);
    }
}

inline void skiplines(std::FILE* file, int nb) {
	int ctr = 0;
	int ch = std::fgetc(file);
	while (ctr < nb) {
		if (ch == 10) ctr++;
		if (ctr < nb) ch = std::fgetc(file);
	}
}

inline long getnumber(std::FILE* file) {
    long num = 0;
    int ch = std::fgetc(file);
    while (ch == 32) {
    	ch = std::fgetc(file);
    }
    
    while (ch != 10 && ch != 32 ) {
        num *= 10;
        num += ch - '0';
        ch = std::fgetc(file);
    }
    return num;
}


inline int memoryUsageLinux() {
    char fname[100];
    std::sprintf(fname, "/proc/%d/stat", getpid());
    std::FILE* procfile = fopen(fname, "r");
    if (procfile != NULL) {
        skipspaces(procfile, 23);
        long size = getnumber(procfile)*(sysconf(_SC_PAGE_SIZE)/1024);
        std::fclose(procfile);
        return static_cast<int>(size);
    } else {
        return 0;
    }
}


inline int memoryUsageMac() {
    char fname[100];
    std::sprintf(fname, "ps -p %d -o rss", getpid());
    std::FILE* procfile = popen(fname, "r");
    if (procfile != NULL) {
    	skiplines(procfile,1);
        //skipspaces(procfile, 23);
    	long num = getnumber(procfile);
    //	cout << num << endl;
    //	cout << (sysconf(_SC_PAGE_SIZE)/1024) << endl;
        long size = num;
        pclose(procfile);
        return static_cast<int>(size);
    } else {
        return 0;
    }
}

inline int memoryUsage() {
#ifdef MACOSX
	return memoryUsageMac();
#elif WINDOWS
	return -1;
#else
	return memoryUsageLinux();
#endif
}

inline double convert_timeval(const timeval& t) {
	return t.tv_sec + (t.tv_usec / 1e6);	
}  

inline double timeUsage() {
	struct rusage(stats);
	if (getrusage(0,&stats) != 0)
		return 0; 

	return convert_timeval(stats.ru_utime);
}

#endif /*MEMORY_H_*/
