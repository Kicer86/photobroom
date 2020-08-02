
// for problems with name-prefix see https://www.mail-archive.com/bug-bison@gnu.org/msg03244.html thread

%define api.pure
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
        //std::cout << "error: " << s << std::endl;
        return 1;
    }

    std::vector<std::string> namespaces;

    std::string join_namespace()
    {
        std::string r;

        for (const std::string& n: namespaces)
            if (n.empty() == false)
                r += n + "::";

        return r;
    }
%}

%token ENUM
%token CLASS
%token NAMESPACE
%token WORD

%%

//eat lines
input:    /* empty */;
input:    input enum_definition;
input:    input namespace_scope;
input:    input error;              // allow errors, this is not full c++ parser.


enum_definition:    enum_header '{' entries '}' ';'  {}


enum_header:        enum_keyword enum_name {}


enum_keyword:       ENUM            {
                                        enums->push_back(Enum{});
                                        enums->back().namespace_name = join_namespace();
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


entries: entry
entries: entries entry


entry:  entry_name
entry:  entry_name ','


entry_name: WORD                {
                                    enums->back().entries.push_back($1);
                                }


namespace_scope: namespace_name namespace_content  {  namespaces.pop_back(); }


namespace_name: NAMESPACE       {   namespaces.push_back({}); }
namespace_name: NAMESPACE WORD  {   namespaces.push_back($2); }


namespace_content: '{' input '}'

%%
