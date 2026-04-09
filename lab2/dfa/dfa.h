#pragma once 

#include <iostream>
#include <map>
#include "../astbuild/AST.h"
#include "../astbuild/parser.h"
namespace myregex{

class DFA{
    public:
        struct State{
            bool is_accept = false;
            std::map<char,int>  transitions;

            std::map<int,int> capture_start;
            std::map<int,int> capture_end;
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
        void addCaptureStart(int state, int group);
        void addCaptureEnd(int state, int group);
        bool accepts(const std::string&)const;

        static DFA compile(const std::string& str);
        static DFA fromAST(ASTNode& root);

        std::string toRegex()const;

        DFA commplement() const;

        DFA substract(const DFA& other)const;
    
    //private:
        std::vector<State> states_;
        int start_state = -1;
        std::set<char> alphabet_;
        
        static int findMarkerPos(const ASTNode& root);
        static void computeFollowpos(const ASTNode& node,std::vector<std::set<int>>& followpos);
        static DFA constructDFA(const ASTNode& root,const std::vector<std::set<int>>& followpos,int numPositions);
        static void collectSym(const ASTNode& root,std::map<int,char>& posToChar,std::set<char>& alphabet);

};

}