#pragma once 

#include <iostream>
#include <map>
#include "../astbuild/AST.h"
#include "../astbuild/parser.h"
class DFA{
    public:
        struct State{
            bool is_accept = false;
            std::map<char,int>  transitions;
        };
        struct Capture_res{
            bool find = false;
            size_t start = 0;
            size_t end = 0;
            std::map<int,std::pair<size_t,size_t>> capture;
        };
        DFA() = default;
        int addState(bool fl=false);
        void setStartState(int state);
        void addTransition(int from, char symbol, int to);
        void setAccepting(int state,bool accepting);
        bool accepts(const std::string&)const;

        static DFA compile(const std::string& str);
        static DFA fromAST(ASTNode& root);
        std::string toRegex()const;


        DFA subtract(const DFA& other) const;
        DFA minimize() const;
        void eliminateState(int state);
        std::string getTransitionRegex(int from, int to) const;
        void addOrMergeTransition(int from, int to, const std::string& regex);
        std::string simplifyRegex(const std::string& regex) const;
        mutable std::map<std::pair<int, int>, std::string> regexTransitions_;
        DFA reverse() const;
        DFA makeComplete(const std::set<char>& alphabet) const;
        Capture_res search(const std::string& text) const;
    //private:
        std::vector<State> states_;
        int start_state = -1;
        std::set<char> alphabet_;

        static void computeFollowpos(const ASTNode& node,std::vector<std::set<int>>& followpos);
        static DFA constructDFA(const ASTNode& root,const std::vector<std::set<int>>& followpos,int numPositions);
        static void collectSym(const ASTNode& root,std::map<int,char>& posToChar,std::set<char>& alphabet);

        std::vector<std::set<int>> getStartInitialization() const;
        std::vector<std::set<int>> refinePartition(const std::vector<std::set<int>>& partition) const;
        std::vector<int> getNextState(int state, const std::vector<std::set<int>>& partition) const;
        DFA buildFromPartition(const std::vector<std::set<int>>& partition) const;
        int findIndexState(const std::vector<std::set<int>>& partition, int state) const;


        std::string toDot() const;
        static void saveToDot(const DFA& dfa, const std::string& filename);
};
