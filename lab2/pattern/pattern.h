#pragma once
#include "../dfa/dfa.h"
#include "../nfa/nfa.h"
class Pattern{
    private:
        DFA dfa;
        NFA nfa;
        
    public:
        Pattern(const std::string& rv);


};