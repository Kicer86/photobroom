
%pure_parser
%error-verbose
%parse-param { IFilterEngineCallback* engineCallback }
%parse-param {void *scanner}
%lex-param {yyscan_t *scanner}
%name-prefix="filterEngine_"

%{
  #define YYSTYPE std::string

  //define this macro to satisfy VisualStudio
  #define YY_NO_UNISTD_H

  #include <cassert>
  #include <iostream>
  #include <string>

  #include <QString>

  #include "filter_engine.hpp"
  #include "filter_engine.yy.hh"

  int filterEngine_error(IFilterEngineCallback *, yyscan_t scanner, char const* s)
  {
    std::cout << "error: " << s << std::endl;
    return 1;
  }
%}

%token SELECTOR
%token SCOPE
%token POSSESSION

%token TAG
%token FLAG
%token SHA

%token TEXT
%token STRING

%%

query: selector scope                                    {  };
query: selector scope possession conditions              {  };


selector: SELECTOR                                       {  };


scope: SCOPE                        {
                                        if ($1 == "photos")
                                            engineCallback->filterPhotos();
                                        else
                                            assert(!"Unexpected scope");
                                    };

possession: POSSESSION                                        {};

conditions: condition                                         {};

conditions: conditions condition                              {};


condition: TAG TEXT '=' TEXT       {
                                        engineCallback->photoTag($2.c_str(), $4.c_str());
                                   };

condition: FLAG TEXT '=' TEXT      {
                                        engineCallback->photoFlag($2.c_str(), $4.c_str());
                                   };

condition: SHA '=' value           {
                                        engineCallback->photoChecksum($3.c_str());
                                   };


name:  TEXT;
value: STRING;
%%

