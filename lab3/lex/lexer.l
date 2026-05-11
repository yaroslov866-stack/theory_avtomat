%{
#include "../bison/bison.tab.hpp"
#include <cstring>
#include <cstdio>
extern int yylineno;
using token = yy::parser::token_kind_type;
yy::parser::semantic_type* yylval = nullptr;
%}

%option noyywrap
%option yylineno
%option c++

%%

[ \t\n\r]+          { /* ignore whitespace */ }

"<PROGRAM>"         { return token::PROGRAM; }
"</PROGRAM>"        { return token::PROGRAM_END; }
"<VARDECLARATION>"  { return token::VARDECL; }
"</VARDECLARATION>" { return token::VARDECL_END; }
"<VAR"              { return token::VAR; }
"</VAR>"            { return token::VAR_END; }
"<TYPE>"            { return token::TYPE; }
"</TYPE>"           { return token::TYPE_END; }
"<DIMENSIONS"       { return token::DIMENSIONS; }
"</DIMENSIONS>"     { return token::DIMENSIONS_END; }
"<DIMENSION>"       { return token::DIMENSION; }
"</DIMENSION>"      { return token::DIMENSION_END; }
"<VALUES>"          { return token::VALUES; }
"</VALUES>"         { return token::VALUES_END; }
"<VALUE>"           { return token::VALUE; }
"</VALUE>"          { return token::VALUE_END; }
"<ASSIGN>"          { return token::ASSIGN; }
"</ASSIGN>"         { return token::ASSIGN_END; }
"<TO>"              { return token::TO; }
"</TO>"             { return token::TO_END; }
"<NOT>"             { return token::NOT; }
"</NOT>"            { return token::NOT_END; }
"<AND>"             { return token::AND; }
"</AND>"            { return token::AND_END; }
"<OR>"              { return token::OR; }
"</OR>"             { return token::OR_END; }
"<ADD>"             { return token::ADD; }
"</ADD>"            { return token::ADD_END; }
"<MUL>"             { return token::MUL; }
"</MUL>"            { return token::MUL_END; }
"<SUB>"             { return token::SUB; }
"</SUB>"            { return token::SUB_END; }
"<DIV>"             { return token::DIV; }
"</DIV>"            { return token::DIV_END; }
"<MAX>"             { return token::MAX; }
"</MAX>"            { return token::MAX_END; }
"<MIN>"             { return token::MIN; }
"</MIN>"            { return token::MIN_END; }
"<EQ>"              { return token::EQ; }
"</EQ>"             { return token::EQ_END; }
"<WHILE>"           { return token::WHILE; }
"</WHILE>"          { return token::WHILE_END; }
"<CHECK>"           { return token::CHECK; }
"</CHECK>"          { return token::CHECK_END; }
"<DO>"              { return token::DO; }
"</DO>"             { return token::DO_END; }
"<SWITCH>"          { return token::SWITCH; }
"</SWITCH>"         { return token::SWITCH_END; }
"<CONDITION>"       { return token::CONDITION; }
"</CONDITION>"      { return token::CONDITION_END; }
"<LEFT>"            { return token::LEFT; }
"</LEFT>"           { return token::LEFT_END; }
"<RIGHT>"           { return token::RIGHT; }
"</RIGHT>"          { return token::RIGHT_END; }
"<UP>"              { return token::UP; }
"</UP>"             { return token::UP_END; }
"<DOWN>"            { return token::DOWN; }
"</DOWN>"           { return token::DOWN_END; }
"<SENDDRONS>"       { return token::SENDDRONS; }
"</SENDDRONS>"      { return token::SENDDRONS_END; }
"<GETDRONSCOUNT>"   { return token::GETDRONSCOUNT; }
"</GETDRONSCOUNT>"  { return token::GETDRONSCOUNT_END; }
"<FUNC"             { return token::FUNC; }
"</FUNC>"           { return token::FUNC_END; }
"<CALL>"            { return token::CALL; }
"</CALL>"           { return token::CALL_END; }
"<DIM>"             { return token::DIM; }
"</DIM>"            { return token::DIM_END; }
"<INDEX>"           { return token::INDEX; }
"</INDEX>"          { return token::INDEX_END; }

"INT"               { return token::INT_TYPE; }
"BOOL"              { return token::BOOL_TYPE; }
"CELL"              { return token::CELL_TYPE; }

"true"              { yylval->emplace<int>(1); return token::BOOL_LIT; }
"false"             { yylval->emplace<int>(0); return token::BOOL_LIT; }
"EMPTY"             { yylval->emplace<int>(0); return token::CELL_LIT; }
"WALL"              { yylval->emplace<int>(1); return token::CELL_LIT; }
"EXIT"              { yylval->emplace<int>(2); return token::CELL_LIT; }
"UNDEF"             { yylval->emplace<int>(3); return token::CELL_LIT; }

"CONST"             { return token::CONST; }

"TRUE"              { return token::TRUE_VAL; }
"FALSE"             { return token::FALSE_VAL; }

"name"              { return token::NAME; }
"main"              {  yylval->emplace<std::string>("main"); return token::IDENTIFIER; }
"count"             { return token::COUNT; }

"="                 { return '='; }
">"                 { return '>'; }
"/"                 { return '/'; }

[a-zA-Z_][a-zA-Z0-9_]*   { yylval->emplace<std::string>(YYText()); return token::IDENTIFIER; }
[0-9]+                   { yylval->emplace<int>(atoi(YYText())); return token::INT_LIT; }

.                        { fprintf(stderr, "Неизвестный символ '%c' на строке %d\n", yytext[0], yylineno); }

%%
yyFlexLexer* lexer = nullptr;

//Устанавливаем входной поток
void set_input_stream(std::istream* is) {
    if (lexer == nullptr) {
        lexer = new yyFlexLexer(is, &std::cout);
    } else {
        lexer->switch_streams(is, &std::cout);
    }
}

//yylex вызывает Bison
int yylex(yy::parser::semantic_type* yylval_param) {
    yylval = yylval_param;
    if (lexer == nullptr) {
        lexer = new yyFlexLexer(&std::cin, &std::cout);
    }
    int tok = lexer->yylex();
    return tok;
}