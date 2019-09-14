
#include <cstring>
#include <fstream>

std::string read_word(std::istream& input)
{
    std::string word;

    while(input.good())
    {
        const auto ch = input.get();
        if (ch != std::ifstream::traits_type::eof())
        {
            if (strchr("\n \t", ch))
                break;
            else
                word += static_cast<char>(ch);
        }
    }

    return word;
}

bool parse(const std::string& input_file_path)
{
    std::ifstream input(input_file_path);

    return true;
}

