
%define api.pure
%define parse.error verbose
%name-prefix="enum_"
%parse-param {Enums* enums }
%parse-param {void *scanner}
%lex-param {yyscan_t *scanner}

%{

    #define YYSTYPE std::string

    //define this macro to satisfy VisualStudio
    #define YY_NO_UNISTD_H

    #include <iostream>
    #include <string>
    #include <vector>

    #include "enum.yy.hh"

    #include "logic.hpp"

    int enum_error(Enums*, yyscan_t, char const* s)
    {
        std::cout << "error: " << s << std::endl;
        return 1;
    }

%}

%token ENUM
%token CLASS
%token WORD

%%

//eat lines
input:    /* empty */;
input:    input enum_definition;
input:    input error;              // allow errors, this is not full c++ parser.


enum_definition:    enum_header '{' entries '}' ';'  {}


enum_header:        enum_keyword enum_name {}


enum_keyword:       ENUM            {
                                        enums->push_back(Enum{});
                                    }


enum_name:          CLASS WORD  {
                                    enums->back().name = $2;
                                }

enum_name:          WORD        {
                                    enums->back().name = $1;
                                }

enum_name:                      {
                                    enums->back().name = "";
                                }


entries:
entries: entry
entries: entries entry


entry:  entry_name
entry:  entry_name ','


entry_name: WORD                {
                                    enums->back().entries.push_back($1);
                                }

%%
