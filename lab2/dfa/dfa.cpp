#include "dfa.h"
#include <queue>
#include <iostream>
#include <fstream>


void DFA::computeFollowpos(const ASTNode& node,std::vector<std::set<int>>& followpos){
    switch(node.type){
        case ASTNode::Type::CONCAT:{
            const ASTNode left = node.children[0];
            const ASTNode right = node.children[1];
            for(int i:left.lastpos){
                for(int j:right.firstpos){
                    followpos[i].insert(j);
                }
            }
            computeFollowpos(left,followpos);
            computeFollowpos(right,followpos);
            break;
        }
        case ASTNode::Type::PLUS:
        case ASTNode::Type::RANGE:{
            const ASTNode child = node.children[0];
            for(int i:child.lastpos){
                for(int j:child.firstpos){
                    followpos[i].insert(j);
                }
            }
            computeFollowpos(child,followpos);
            break;
        }
        default:
            for(auto& child:node.children){
                computeFollowpos(child,followpos);
            }
            break;
    }
}

void DFA::collectSym(const ASTNode& root,std::map<int,char>& posToChar,std::set<char>& alphabet){
    if(root.type == ASTNode::Type::LITERAL){
        posToChar[root.position] = root.literal;
        alphabet.insert(root.literal);
    }
    for(const auto& child:root.children){
        collectSym(child,posToChar,alphabet);
    }
}

int DFA::findMarkerPos(const ASTNode& root){
    if(root.type == ASTNode::Type::LITERAL && root.literal == '#'){
        return root.position;
    }
    for(const auto& child:root.children){
        int pos = findMarkerPos(child);
        if(pos != -1){
            return pos;
        }
    }
    return -1;
}

int DFA::addState(bool accept){
    State s;
    s.is_accept = accept;
    states_.push_back(s);
    return states_.size()-1;
}
void DFA::setStartState(int state){
    start_state = state;
}
void DFA::setAccepting(int state,bool accepting){
    if(state>=0 && state< states_.size()){
        states_[state].is_accept = accepting;
    } 
}
void DFA::addTransition(int from, char symbol, int to){
    if(from >= 0 && from < states_.size() && to >= 0 && to < states_.size()){
        states_[from].transitions[symbol] = to;
        alphabet_.insert(symbol);
    }
}
void DFA::addCaptureStart(int state,int group){
    if(state >=0 && state< states_.size()){
        states_[state].capture_start[group] = state;
    }
}
void DFA::addCaptureEnd(int state,int group){
    if(state >=0 && state< states_.size()){
        states_[state].capture_end[group] = state;
    }
}

bool DFA::accepts(const std::string& str)const{
    if(start_state == -1)return false;
    int state = start_state;
    for(char c:str){
        auto it = states_[state].transitions.find(c);
        if(it == states_[state].transitions.end()){
            return false;
        }
        state = it->second;
    }
    return states_[state].is_accept;
}



DFA DFA::constructDFA(const ASTNode& root,const std::vector<std::set<int>>& followpos,int num){
    DFA dfa;
    std::map<int,char> posTochar;
    std::set<char> alphabet;
    collectSym(root,posTochar,alphabet);


    std::set<int> startSet = root.firstpos;

    std::map<std::set<int>,int> stateMap;
    std::queue<std::set<int>> queue;
    int startIndex = dfa.addState();
    stateMap[startSet] = startIndex;
    queue.push(startSet);
    dfa.setStartState(startIndex);
    while(!queue.empty()){
        std::set<int> current = queue.front();
        queue.pop();
        int currIndex = stateMap[current];
        for(char sym:alphabet){
            std::set<int> nextSet;
            for(int pos:current){
                auto it = posTochar.find(pos);
                if(it!=posTochar.end() && it->second == sym){
                    for(int next:followpos[pos]){
                        nextSet.insert(next);
                    }
                }
            }
            if(!nextSet.empty()){
                if(stateMap.find(nextSet) == stateMap.end()){
                    int newind = dfa.addState();
                    stateMap[nextSet] = newind;
                    queue.push(nextSet);
                }
                dfa.addTransition(currIndex,sym,stateMap[nextSet]);
            }
        }
    }
    int mark = findMarkerPos(root);
    for(const auto& [stateSet,idx]:stateMap){
        if(stateSet.find(mark) != stateSet.end()){
            dfa.setAccepting(idx,true);
        }
    }
    
    return dfa;



}




DFA DFA::compile(const std::string& str){
    ASTNode ast = Parser::parse(str);
    return fromAST(ast);
}


DFA DFA::fromAST(ASTNode& root){
    ASTNode root_new = ASTNode::addMark(root);
    int num = 1;
    ASTNode::numeration(root_new,num);
    num--;
    ASTNode::countNullable(root_new);
    ASTNode::countFirstposLastPos(root_new);

    std::vector<std::set<int>> followpos(num+1);
    computeFollowpos(root_new,followpos);
    return constructDFA(root_new,followpos,num);
}





std::string DFA::toDot() const {
    std::string result = "digraph DFA {\n";
    result += "    rankdir=LR;\n";
    result += "    node [shape=circle];\n";
    result += "    start [shape=point];\n";
    result += "    start -> " + std::to_string(start_state) + ";\n";
    for (size_t i = 0; i < states_.size(); ++i) {
        if (states_[i].is_accept) {
            result += "    " + std::to_string(i) + " [shape=doublecircle];\n";
        } else {
            result += "    " + std::to_string(i) + " [shape=circle];\n";
        }
    }
    for (size_t from = 0; from < states_.size(); ++from) {
        for (const auto& [symbol, to] : states_[from].transitions) {
            result += "    " + std::to_string(from) + " -> " + std::to_string(to);
            result += " [label=\"" + std::string(1, symbol) + "\"];\n";
        }
    }
    result += "}\n";
    return result;
}


void DFA::saveToDot(const DFA& dfa, const std::string& filename) {
    std::ofstream file(filename);
    file << dfa.toDot();
    file.close();
}

