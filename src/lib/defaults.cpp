#include "defaults.h"
#include <iostream>

int cnf::LIMIT_MEMORY = 0;

const int cnf::DEFAULT_LIMIT_MEMORY = 1200;

const int cnf::CLASS_OFFSET = 0;

const int cnf::ATTR_WIDTH = 1000;

const int cnf::CLASS_WIDTH = cnf::ATTR_WIDTH;

const int cnf::BINARY_WIDTH = 1000;

const int cnf::BINARY_START = 1000000;

const int cnf::DEFAULT_K = 5;

const std::string cnf::DEFAULT_METRIC = "length";

const std::string cnf::DEFAULT_SCORER = "major";

void cnf::print_defaults() {
	std::cout << "default values in use: " << std::endl;
	std::cout << "   BINARY_START        " << BINARY_START << std::endl;
	std::cout << "   ATTR_WIDTH          " << ATTR_WIDTH << std::endl;
	std::cout << "   CLASS_WIDTH         " << CLASS_WIDTH << std::endl;
	std::cout << "   BINARY_WIDTH        " << BINARY_WIDTH << std::endl;	
	std::cout << "   DEFAULT_K           " << DEFAULT_K << std::endl;
	std::cout << "   DEFAULT_METRIC      " << DEFAULT_METRIC << std::endl;
	std::cout << "   DEFAULT_SCORER      " << DEFAULT_SCORER << std::endl;
}
