%{
  #define YYSTYPE std::string    //container for lexer data

  #include <iostream>

  #include "logic.hpp"

  #include "enum.tab.hh"         //include bison's definitions
%}

%option nounistd
%option never-interactive
%option reentrant
%option bison-bridge
%option noyywrap
%option prefix="enum_"


%%

enum            {   // enum keyword
                    std::cout << "enum lexer: got \"enum\"\n";

                    *yylval=yytext;
                    return ENUM;
                }

class           {   // class keyword
                    std::cout << "enum lexer: got \"class\"\n";

                    *yylval=yytext;
                    return CLASS;
                }

[[:alnum:]_-]+  {   // word
                    std::cout << "enum lexer: got \"" << yytext << "\"\n";

                    *yylval=yytext;
                    return WORD;
                }

[[:blank:]$]+ {} //remove blanks

.               {   //some character
                    std::cout << "enum lexer: got character: '" << yytext[0] << "'\n";

                    return yytext[0];
                }

%%
