%{
  #define YYSTYPE std::string    //kontener na dane leksera

  #include <string>

  #include "filter_engine.hpp"
  #include "filter_engine.tab.hh"         //include bison's definitions
%}

%option nounistd
%option never-interactive
%option reentrant
%option bison-bridge
%option noyywrap
%option extra-type="IFilterEngineCallback *"
%option prefix="filterEngine_"

%x COND

%%

\".*[^\\]\"   { //string
                *yylval=yytext;
                return STRING;
              }

\'.*[^\\]\'   { //string
                *yylval=yytext;
                return STRING;
              }

<INITIAL>all  { // selector
                *yylval=yytext;
                return SELECTOR;
              }

<INITIAL>photos  { // scope
                   *yylval=yytext;
                   return SCOPE;
                 }

<INITIAL>with|without  { // possession
                            *yylval=yytext;

                            BEGIN(COND);
                            return POSSESSION;
                       }

<COND>tag {
            *yylval=yytext;
            return TAG;
          }

<COND>flag {
                *yylval=yytext;
                return FLAG;
           }

<COND>sha {
            *yylval=yytext;
            return SHA;
          }

<COND>[[:alnum:]_-]+  { //text
                        *yylval=yytext;
                        return TEXT;
                      }

<*>[[:blank:]$]+ {} //remove blanks

<COND>"="     { // '='
                return yytext[0];
              }

%%
