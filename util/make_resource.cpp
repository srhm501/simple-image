#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " buffername /path/to/file" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const std::string filename(argv[2]);

    std::ifstream input(filename);
    if (!input.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }

    const std::string source(std::istreambuf_iterator<char>(input), (std::istreambuf_iterator<char>()));

    std::cout << "extern const char " << argv[1] << "[] = {" << std::hex;

    for (size_t i=0; i<source.length(); ++i) {
        if (i % 10 == 0) {
            std::cout << '\n';
        } else {
            std::cout << ' ';
        }

        std::cout << "0x" << (int)source[i] << ',';
    }

    std::cout << "\n};" << std::endl;
}
