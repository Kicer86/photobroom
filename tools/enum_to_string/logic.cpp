
#include <cstring>
#include <fstream>
#include <vector>

struct Enum
{
    std::string name;
    std::vector<std::string> entries;
};


namespace
{
    class EnumStateMachine
    {
        public:
            void take(const std::string& word)
            {
                switch (m_state)
                {
                    case WaitingForEnum:
                        if (word == "enum")
                        {
                            m_enums.emplace_back(Enum{});
                            m_state = WaitingForClassOrName;
                        }
                        break;

                    case WaitingForClassOrName:
                        if (word == "class")
                            break;

                        m_enums.back().name = word;
                        m_state = WaitingForBody;
                        break;
                }
            }

            const std::vector<Enum>& enums() const
            {
                return m_enums;
            }

        private:
            enum
            {
                WaitingForEnum,
                WaitingForClassOrName,
                WaitingForBody,
            } m_state = WaitingForEnum;

            std::vector<Enum> m_enums;
    };
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
    EnumStateMachine state_machine;

    while(data.good())
    {
        const std::string w = read_word(data);

        state_machine.take(w);
    }

    return state_machine.enums();
}


bool parse(const std::string& input_file_path)
{
    std::ifstream input(input_file_path);

    return true;
}

