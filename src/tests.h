#ifndef TESTS_H_
#define TESTS_H_

#include "filter/Filter.h"
#include "test/theorysize.h"
#include <string>
#include <ostream>

#include "ruletrie/RuleTrie.h"

int main_test(int argc, char* argv[]);

int test_reader(int argc, char* argv[]);

int test_rulereader(int argc, char* argv[]);

int test_learner(int argc, char* argv[]);

int test_metric(int argc, char* argv[]);

int test_redundancy(int argc, char* argv[]);

int test_combination(int argc, char* argv[]);

int test_insert();

int test_filter();

cnf::Filter& createFilter(bool bin, bool fil);

void print_sizes();

int test_ruletrie(int argc, char* argv[]);

void readAndAdd(const std::string& filename, cnf::RuleTrie& rules);

void readAndDelete(const std::string& filename, cnf::RuleTrie& rules);

void write(const std::string& filename, const cnf::RuleTrie& rules, bool noExId);

void write(std::ostream& file, const cnf::RuleTrie& rules, bool noExId);

int run_pipe(int argc, char* argv[]);

int run_measure(int argc, char* argv[]);

#endif /*TESTS_H_*/
