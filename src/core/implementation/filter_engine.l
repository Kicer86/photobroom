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

<*>\".*[^\\]\"          { //string
                            *yylval=yytext;
                            return STRING;
                        }

<*>\'.*[^\\]\'          { //string
                            *yylval=yytext;
                            return STRING;
                        }

<INITIAL>all            { // selector
                            *yylval=yytext;
                            return SELECTOR;
                        }

<INITIAL>photos         { // scope
                            *yylval=yytext;
                            return SCOPE_PHOTOS;
                        }

<INITIAL>with           { // possession
                            *yylval=yytext;

                            BEGIN(COND);
                            return WITH;
                        }

<INITIAL>without        { // possession
                            *yylval=yytext;

                            BEGIN(COND);
                            return WITHOUT;
                        }

<COND>tag               {
                            *yylval=yytext;
                            return TAG;
                        }

<COND>flag              {
                            *yylval=yytext;
                            return FLAG;
                        }

<COND>sha               {
                            *yylval=yytext;
                            return SHA;
                        }

<COND>id                {
                            *yylval=yytext;
                            return ID;
                        }

<COND>any               {
                            *yylval=yytext;
                            return ANY;
                        }

<COND>role              {
                            *yylval=yytext;
                            return ROLE;
                        }


<COND>member            {
                            *yylval=yytext;
                            return MEMBER;
                        }

<COND>representative    {
                            *yylval=yytext;
                            return REPRESENTATIVE;
                        }

<COND>regular           {
                            *yylval=yytext;
                            return REGULAR;
                        }


<COND>[[:alnum:]_-]+    { //text
                            *yylval=yytext;
                            return TEXT;
                        }

<COND>[=,~]             { // some punctuation marks
                            return yytext[0];
                        }

<*>[[:blank:]$]+        {} //remove blanks

%%
