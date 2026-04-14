#include "pattern.h"

Pattern::Pattern(const std::string& rv){
    ASTNode node = Parser::parse(rv);
    bool fl = false;
    node.hasCapture(node,fl);
    if(fl){
        
        std::cout<<"true"<<std::endl;
        NFA nfa = NFA::fromAST(node);
    }
    std::cout<<"false";
}