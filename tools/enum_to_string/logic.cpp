
#include <cstring>
#include <fstream>

namespace
{
    const char* const whitespaces = "\n\r\t ";
}

std::string read_word(std::istream& input)
{
    std::string word;

    // skip all whilespaces
    while(input.good())
    {
        const auto ch = input.get();
        if (ch != std::ifstream::traits_type::eof())
        {
            if (strchr(whitespaces, ch))
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
            if (strchr(whitespaces, ch))
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

