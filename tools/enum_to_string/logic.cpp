
#include <cstring>
#include <fstream>
#include <vector>


#include "enum.tab.hh"
#include "enum.yy.hh"


struct Enum
{
    std::string name;
    std::vector<std::string> entries;
};


std::vector<Enum> find_enum(std::istream& data)
{
  YY_BUFFER_STATE bp;

#if defined YYDEBUG && YYDEBUG==1
  enum_debug=0;
#endif

  const std::string stream(std::istreambuf_iterator<char>(data), {});

  yyscan_t scanner;
  enum_lex_init_extra(nullptr, &scanner);

  bp=enum__scan_string(stream.data(), scanner);
  enum__switch_to_buffer(bp, scanner);
  enum_parse(scanner);

  enum__delete_buffer(bp, scanner);
  enum_lex_destroy(scanner);

  return {};
}


bool parse(const std::string& input_file_path)
{
    std::ifstream input(input_file_path);

    return true;
}

