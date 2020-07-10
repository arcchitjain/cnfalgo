#include <stack>
#include "experiment/MultiStreamExperiment.h"
#include "experiment/CompletionExperiment.h"
#include "experiment/MissingValueExperiment.h"

#include "tests.h"
#include "drift.h"
#include "lib/defaults.h"

#include <string>
#include <iostream>

/*
 *  pipe.cpp
 *  cnfalgo
 *
 *  Created by Anton Dries on 12/04/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>
#include <string>
#include <fstream>
#include <iostream>


int main(int argc, char *argv[]) {
	
	//clog << argc << " command line parameters given" << std::endl;
	//for (int i=0; i<argc; i++) clog << argv[i] << " "; clog << endl; 
	try {
		if (argc > 1) {
			std::string select = argv[1];
			//shift_array(argc,argv);
			
			if (select == "ms") {
				return cnf::MultiStreamExperiment::run_exp(argc-1,argv+1);
			} else if (select == "cc") {
				return cnf::CompletionExperiment::run_exp(argc-1,argv+1);
			} else if (select == "mv") {
				return cnf::MissingValueExperiment::run_exp(argc-1,argv+1);
			} else if (select == "pipe") {
				return run_pipe(argc-1,argv+1);
			} else if (select == "measure") {
				return run_measure(argc-1,argv+1);
			} else if (select == "online") {
				return run_online(argc-1,argv+1);
			} else if (select == "test") {
				return main_test(argc-1,argv+1);
			} else if (select == "show") {
				cnf::print_defaults();
				print_sizes();
				return 0;
			}
		} 
		
		std::cout << " The first parameter should be one of the following: ms, cc, mv, test, show, pipe, measure " << std::endl;
		return -1;
	} catch (int e) {
		if (e == 5) std::cerr << "Process aborted : memory limit exceeded !" << std::endl;
		return -e;	
	}
		
}


