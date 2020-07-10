#ifndef DEFAULTS_H_
#define DEFAULTS_H_

#include <string>

namespace cnf {

extern int LIMIT_MEMORY;

extern const int CLASS_OFFSET;

extern const int DEFAULT_LIMIT_MEMORY;

// Ranges of values: class label << binary attributes << non binary attributes

extern const int ATTR_WIDTH;

extern const int CLASS_WIDTH;

extern const int BINARY_WIDTH;

extern const int BINARY_START;

extern const int DEFAULT_K;

extern const std::string DEFAULT_METRIC;

extern const std::string DEFAULT_SCORER;


void print_defaults();

}

#endif /*DEFAULTS_H_*/
