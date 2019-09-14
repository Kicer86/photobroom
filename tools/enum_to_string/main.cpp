
#include <iostream>

#include "logic.cpp"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " input output" << std::endl;
        std::cout << std::endl;
        std::cout << "input  - source c/c++ file" << std::endl;
        std::cout << "output - target c/c++ file" << std::endl;

        return 1;
    }

    const std::string input = argv[1];
    const std::string output = argv[2];

    const bool status = parse(input);
}
