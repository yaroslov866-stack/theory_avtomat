#pragma once
#include "../dfa/dfa.h"
#include "../nfa/nfa.h"
class Pattern{
    private:
        DFA dfa;
        NFA nfa;
        std::string str;
    public:
        DFA getDFA()const;
        Pattern()= default;
        Pattern(const std::string& rv);
        MatchResult match(std::string& );
        Pattern reverse()const;
        DFA::Capture_res search(const std::string& str);
        Pattern substract(const Pattern& pat2);
};