#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
#include <queue>
#include "../dfa/dfa.h"

struct MatchResult{
    bool success = false;
    std::string full_match;
    std::map<int, std::pair<size_t, size_t>> group_positions;
    bool match()const {return success;}
    std::string group(int id) const{
        auto it = group_positions.find(id);
        if (it != group_positions.end() && it->second.first < it->second.second) {
            return full_match.substr(it->second.first,it->second.second - it->second.first);
        }
        return "";
    }
};


class NFA{
    public:
        NFA() = default;
        struct State{
            bool is_accepting = false;
            std::map<char,std::set<int>> transitions;

            std::set<int> epsilon_transitions;
            std::map<int,std::set<int>> group_start_transitions;
            std::map<int,std::set<int>> group_end_transitions;
        };
        int addState(bool accept = false);
        void addTransitions(int from,int to,char sym);
        void addEpsilon(int from,int to);
        void setStartState(int state);
        void setAccepting(int state,bool accept);
        void addGroupStartTransition(int from, int to, int group);
        void addGroupEndTransition(int from, int to, int group);
        static NFA fromAST(const ASTNode& node);

        int getStartSt()const{return start_state_;}
        const std::vector<State>& getStates() const{return states_;}
    //private:
        struct Metka{
            int start;
            int end;
        };
        static Metka buildSym(char c,NFA& nfa);
        static Metka buildEpsilon(NFA& nfa);
        static Metka buildConcat(const Metka& left,const Metka& right,NFA& nfa);
        static Metka buildAlternation(const Metka& left, const Metka& right, NFA& nfa);
        static Metka buildPlus(const Metka& child, NFA& nfa);
        static Metka buildCaptureGroup(const Metka& child, int number, NFA& nfa);
        static Metka buildFromAST(const ASTNode& node, NFA& nfa);
        struct ClosureResult{
            std::set<int> states;
            std::map<int, size_t> started_groups;
            std::map<int, size_t> ended_groups;
        };

        ClosureResult epsilonClosure(const std::set<int>& states,size_t pos) const;
        std::vector<State> states_;
        int start_state_ = -1;

        MatchResult match(const std::string& input) const;
        bool hasCaptureGroups() const;
        MatchResult simulateNFA(const std::string& input, size_t start_pos) const;



        std::string toDot() const;
        static void saveNFAToDot(const NFA& nfa, const std::string& filename);
};

