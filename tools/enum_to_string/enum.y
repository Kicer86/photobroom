
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

  int enum_error(yyscan_t scanner, char const* s)
  {
    std::cout << "error: " << s << std::endl;
    return 1;
  }

%}

%token TEXT

%%

//eat lines
input:    /* empty */;
input:    input word;

word:     TEXT     {}

%%
