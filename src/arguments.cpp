#include "arguments.hpp"

#include <getopt.h>

#include <string>

bool verbose = false;

std::string in_file;
std::string out_file;

void handle_args(int argc, char **argv)
{
    static struct option long_options[] = {
        {"verbose", no_argument, 0, 'v'},
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };

    int c;
    int option_idx;

    while ((c = getopt_long(argc, argv, "vi:o:", long_options, &option_idx)) != -1) {
        switch (c) {
        case 'v':
            verbose = true;
            break;
        case 'i':
            in_file = optarg;
            break;
        case 'o':
            out_file = optarg;
            break;
        }
    }
}
