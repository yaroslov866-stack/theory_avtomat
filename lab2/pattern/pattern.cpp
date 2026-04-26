#include "pattern.h"

Pattern::Pattern(const std::string& rv){
    str = rv;
    ASTNode node = Parser::parse(str);
    bool fl = false;
    node.hasCapture(node,fl);
    if(fl){
        nfa = NFA::fromAST(node);
        return;
    }
    dfa = DFA::compile(str);
}
DFA::Capture_res Pattern::search(const std::string& str){
    return dfa.search(str);
}

Pattern Pattern::substract(const Pattern& pat2){
    Pattern res;
    DFA resdfa = dfa.subtract(pat2.getDFA());
    res.dfa = resdfa;
    res.str = resdfa.toRegex();
    return res;
}

DFA Pattern::getDFA()const{
    return dfa;
}

Pattern Pattern::reverse()const{
    Pattern patrn;
    patrn.dfa = dfa.reverse();
    patrn.str = dfa.toRegex();
    return patrn;

}

MatchResult Pattern::match(std::string& str){
    return nfa.match(str);
}