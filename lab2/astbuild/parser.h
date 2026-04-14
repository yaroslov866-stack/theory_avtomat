#pragma once 
#include "AST.h"
#include <string>
#include "tokens.h"

class Parser{
    public:
        static ASTNode parse(const std::string& str);
    // private:
        static std::vector<Token> tokenization(const std::string& str);
        static bool canbeleft(const Token& token);
        static bool canberight(const Token& token);
        static int prioritet(const Token& token);

        static int find_number(const std::string& str,size_t& i);
        static std::vector<Token> insert_conc(const std::vector<Token>& tokens);
        static ASTNode buildTree(const std::vector<Token>& tokens);
        
};

