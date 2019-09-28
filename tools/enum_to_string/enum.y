
%pure_parser
%error-verbose
%parse-param {void *scanner}
%lex-param {yyscan_t *scanner}
%name-prefix="enum_"

%{

    #define YYSTYPE std::string

    //define this macro to satisfy VisualStudio
    #define YY_NO_UNISTD_H

    #include <iostream>
    #include <string>
    #include <vector>

    #include "enum.yy.hh"

    #include "logic.hpp"

    int enum_error(yyscan_t scanner, char const* s)
    {
        std::cout << "error: " << s << std::endl;
        return 1;
    }

    Enum current_enum;
    std::vector<Enum> enums;
%}

%token ENUM
%token CLASS
%token WORD

%%

//eat lines
input:    /* empty */;
input:    input enum_definition;

enum_definition:    enum_header '{' entries '}' ';'  {}


enum_header:        enum_keyword enum_name {}


enum_keyword:       ENUM            {
                                        enums.push_back(Enum{});
                                    }


enum_name:          CLASS WORD  {
                                    enums.back().name = $2;
                                }

enum_name:          WORD        {
                                    enums.back().name = $1;
                                }

enum_name:                      {
                                    enums.back().name = "";
                                }


entries:
entries:    WORD
entries:    WORD ',' entries

%%
