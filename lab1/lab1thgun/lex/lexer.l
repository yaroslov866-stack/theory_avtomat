%option noyywrap c++
%option never-interactive
%{
#include "Flex_scan.h"
#include <string>
std::string current_page_lexer{};
std::string getPage(){return current_page_lexer;}
%}
%x PROTOCOL SERVER ZONE NAME_ZONE SLASH PAGE FILE TAIL

%%
<INITIAL>{
    ^https?            {BEGIN(PROTOCOL);current_page_lexer.clear();}
    [ \t]+             ; 
    \n                 return 0;        
    .                  return 0;
}
<PROTOCOL>{
    :\/\/              BEGIN(SERVER);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<SERVER>{
    [a-z]{1,20}        BEGIN(ZONE);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<ZONE>{
    \.                 BEGIN(NAME_ZONE);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<NAME_ZONE>{
    [a-zA-Z]{1,5}      BEGIN(SLASH);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<SLASH>{
    \/                 BEGIN(PAGE);
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<PAGE>{
    [a-zA-Z0-9]{1,20}  {BEGIN(FILE);current_page_lexer+=yytext;}
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<FILE>{
    \.html             { BEGIN(TAIL); return 1; }
    .                  BEGIN(INITIAL);
    \n                 BEGIN(INITIAL);
}
<TAIL>{
    .                  ;              
    \n                 BEGIN(INITIAL);
}
<<EOF>> {
    return 0;   
}
%%