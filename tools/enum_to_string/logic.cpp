
#include <cassert>
#include <fstream>
#include <vector>

#include "logic.hpp"

#include "enum.tab.hh"
#include "enum.yy.hh"


Enums find_enum(std::istream& data)
{
  YY_BUFFER_STATE bp;

#if defined YYDEBUG && YYDEBUG==1
  enum_debug=0;
#endif

  Enums enums;

  const std::string stream(std::istreambuf_iterator<char>(data), {});

  yyscan_t scanner;
  enum_lex_init_extra(&enums, &scanner);

  bp = enum__scan_string(stream.data(), scanner);
  enum__switch_to_buffer(bp, scanner);
  enum_parse(&enums, scanner);

  enum__delete_buffer(bp, scanner);
  enum_lex_destroy(scanner);

  return enums;
}


Enums parse(const std::string& input_file_path)
{
    std::ifstream input(input_file_path);
    assert(input.is_open());
    assert(input.good());

    Enums enums = find_enum(input);

    return enums;
}

