%{
  #define YYSTYPE std::string    //container for lexer data

  #include <iostream>

  #include "logic.hpp"

  #include "enum.tab.hh"         //include bison's definitions

  //#define DBG(MSG) std::cout << MSG << "\n"
  #define DBG(MSG) //
%}

%option nounistd
%option never-interactive
%option reentrant
%option bison-bridge
%option noyywrap
%option prefix="enum_"

%%

enum            {   // enum keyword
                    DBG("enum lexer: got \"enum\"\n");

                    *yylval=yytext;
                    return ENUM;
                }

class           {   // class keyword
                    DBG("enum lexer: got \"class\"");

                    *yylval=yytext;
                    return CLASS;
                }

namespace       {   // namespace keyword
                    DBG("enum lexer: got \"namespace\"");

                    *yylval=yytext;
                    return NAMESPACE;
                }

[[:alnum:]_-]+  {   // word
                    DBG("enum lexer: got \"" << yytext << "\"");

                    *yylval=yytext;
                    return WORD;
                }

[[:blank:]$]+   {} //remove blanks

.               {   //some character
                    DBG("enum lexer: got character: '" << yytext[0] << "'");

                    return yytext[0];
                }

<*>\n           {}  //eat endl
%%
