
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
        const std::string enum_in_namespace = e.namespace_name + e.name;

        output_stream << "inline std::string get_entry(" << enum_in_namespace << " entry)\n";
        output_stream << "{\n";
        output_stream << "\tswitch(entry)\n";
        output_stream << "\t{\n";

        for (const std::string& entry: e.entries)
            output_stream << "\t\tcase " << enum_in_namespace + "::" + entry << ": return \"" << entry << "\";\n";

        output_stream << "\t\tdefault: return {};\n";
        output_stream << "\t};\n";
        output_stream << "}\n";
    }

    return 0;
}
