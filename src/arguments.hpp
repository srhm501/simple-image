#ifndef ARGUMENTS_HPP_
#define ARGUMENTS_HPP_

#include <CL/cl.hpp>

#include <string>

extern bool verbose;
extern std::string in_file;
extern std::string out_file;

void handle_args(int argc, char **argv);

#endif // ARGUMENTS_HPP_
