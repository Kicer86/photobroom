
#include <iostream>
#include <fstream>

#include "logic.hpp"

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

    const Enums enums = parse(input);

    std::ofstream output_stream(output);

    output_stream << "#include <map>\n";
    output_stream << "#include \"" << input << "\"\n";
    output_stream << "\n";

    for(const Enum& e: enums)
    {
        output_stream << "std::map<" << e.name << ", std::string> " << e.name << "_strings = \n";
        output_stream << "{\n";

        for (const std::string& entry: e.entries)
            output_stream << "\t{ " << entry << ", \"" << entry << "\" },\n";

        output_stream << "};\n";
    }

    return 0;
}
