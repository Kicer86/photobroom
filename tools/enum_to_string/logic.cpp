
#include <cstring>
#include <fstream>
#include <vector>


struct Enum
{
    std::string name;
    std::vector<std::string> entries;
};


std::string read_word(std::istream& input)
{
    std::string word;

    // skip all whilespaces
    while(input.good())
    {
        const auto ch = input.get();
        if (ch != std::ifstream::traits_type::eof())
        {
            if (isspace(ch))
                continue;
            else
            {
                input.unget();
                break;
            }
        }
    }

    // put all but whitespaces to 'word'
    while(input.good())
    {
        const auto ch = input.get();
        if (ch != std::ifstream::traits_type::eof())
        {
            if (isspace(ch))
                break;
            else
                word += static_cast<char>(ch);
        }
    }

    return word;
}


std::vector<Enum> find_enum(std::istream& data)
{
    std::vector<Enum> results;

    while(data.good())
    {
        std::string w = read_word(data);

        if (w == "enum")
        {
            std::string name = read_word(data);
            if (name == "class")
                name = read_word(data);

            results.emplace_back(Enum{name, {}});
        }
    }

    return results;
}


bool parse(const std::string& input_file_path)
{
    std::ifstream input(input_file_path);

    return true;
}

