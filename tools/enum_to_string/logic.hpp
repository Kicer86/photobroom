
#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <string>
#include <vector>

struct Enum
{
    std::string name;
    std::vector<std::string> entries;
};

using Enums = std::vector<Enum>;

Enums find_enum(std::istream& data);
bool parse(const std::string& input_file_path);

#endif
