#include "nfa.h"
#include <fstream>




int NFA::addState(bool accept){
    State state;
    state.is_accepting = accept;
    states_.push_back(state);
    return states_.size()-1;
}
void NFA::addTransitions(int from,int to,char sym){
    if(from>=0 && from<states_.size() && to>=0 && to<states_.size()){
        states_[from].transitions[sym].insert(to);
    }
}

void NFA::addEpsilon(int from,int to){
    if(from>=0 && from<states_.size() && to>=0 && to<states_.size()){
        states_[from].epsilon_transitions.insert(to);
    }
}
void NFA::setStartState(int state){
    if(state>=0 && state<states_.size()){
        start_state_ = state;
    }
}
void NFA::addGroupStartTransition(int from, int to, int group){
    if (from >= 0 &&from<states_.size()&& to >= 0 && to<states_.size()){
        states_[from].group_start_transitions[group].insert(to);
    }
}

void NFA::addGroupEndTransition(int from,int to,int group){
    if(from>=0 && from<states_.size() && to>=0 && to<states_.size()){
        states_[from].group_end_transitions[group].insert(to);
    }
}

void NFA::setAccepting(int state, bool accepting) {
    if(state >= 0 && state < states_.size()){
        states_[state].is_accepting = accepting;
    }
}

NFA::Metka NFA::buildSym(char c,NFA& nfa){
    int start = nfa.addState(false);
    int end = nfa.addState(true);
    nfa.addTransitions(start,end,c);
    Metka m;
    m.end = end;
    m.start = start;
    return m;
}

NFA::Metka NFA::buildEpsilon(NFA& nfa){
    int start = nfa.addState(false);
    int end = nfa.addState(true);
    nfa.addEpsilon(start,end);
    Metka m;
    m.end = end;
    m.start = start;
    return m;
}

NFA::Metka NFA::buildConcat(const Metka& left, const Metka& right, NFA& nfa){
    nfa.addEpsilon(left.end, right.start);
    nfa.setAccepting(left.end, false);
    Metka m;
    m.end = right.end;
    m.start = left.start;
    return m;
}

NFA::Metka NFA::buildAlternation(const Metka& left, const Metka& right, NFA& nfa){
    int newStart = nfa.addState(false);
    int newEnd = nfa.addState(true);
    nfa.addEpsilon(newStart, left.start);
    nfa.addEpsilon(newStart, right.start);
    nfa.addEpsilon(left.end, newEnd);
    nfa.addEpsilon(right.end, newEnd);
    nfa.setAccepting(left.end, false);
    nfa.setAccepting(right.end, false);
    Metka m;
    m.start = newStart;
    m.end = newEnd;
    return m;
}

NFA::Metka NFA::buildPlus(const Metka& child,NFA& nfa){
    //int newStart = nfa.addState(false);
    //int newEnd = nfa.addState(true);
    //nfa.addEpsilon(newStart, child.start);
    nfa.addEpsilon(child.end,child.start);
    //nfa.addEpsilon(child.end, newEnd);
    //nfa.setAccepting(child.end,false);
    Metka m;
    // m.start = newStart;
    // m.end = newEnd;
    m.start = child.start;
    m.end = child.end;
    return m;
}

NFA::Metka NFA::buildCaptureGroup(const Metka& child, int number, NFA& nfa) {
    int newStart = nfa.addState(false);
    int newEnd = nfa.addState(true);
    nfa.addGroupStartTransition(newStart, child.start, number);
    nfa.addGroupEndTransition(child.end, newEnd, number);
    nfa.setAccepting(child.end, false);
    return {newStart,newEnd};
}

NFA::Metka NFA::buildFromAST(const ASTNode& node,NFA& nfa){
    switch(node.type){
        case ASTNode::Type::LITERAL:
            return buildSym(node.literal, nfa);
        case ASTNode::Type::EPSILON:
            return buildEpsilon(nfa);
        case ASTNode::Type::CONCAT:{
            Metka left = buildFromAST(node.children[0],nfa);
            Metka right = buildFromAST(node.children[1],nfa);
            return buildConcat(left, right, nfa);
        }
        case ASTNode::Type::OR:{
            Metka left = buildFromAST(node.children[0],nfa);
            Metka right = buildFromAST(node.children[1],nfa);
            return buildAlternation(left,right,nfa);
        }
        case ASTNode::Type::PLUS:{
            Metka child = buildFromAST(node.children[0], nfa);
            return buildPlus(child, nfa);
        }    
        case ASTNode::Type::CAPTURE_GROUP:{
            Metka child = buildFromAST(node.children[0],nfa);
            return buildCaptureGroup(child, node.capt_num,nfa);
        }
        default:
            throw std::runtime_error("Unknown AST node type");
    }
}

NFA NFA::fromAST(const ASTNode& node){
    NFA nfa;
    Metka root = buildFromAST(node, nfa);
    nfa.setStartState(root.start);
    return nfa;
}

bool NFA::hasCaptureGroups() const {
    for (const auto& state : states_) {
        if (!state.group_start_transitions.empty() || 
            !state.group_end_transitions.empty()) {
            return true;
        }
    }
    return false;
}
MatchResult NFA::simulateNFA(const std::string& input, size_t start_pos) const {
    MatchResult result;
    result.success = false;
    auto closure = epsilonClosure({start_state_}, start_pos);
    std::set<int> current = closure.states;
    size_t pos = start_pos;
    std::map<int, size_t> group_starts = closure.started_groups;
    std::map<int, size_t> group_ends = closure.ended_groups;
    while(pos < input.length() && !current.empty()){
        char c = input[pos];
        std::set<int> next;
        for(int s:current){
            auto it = states_[s].transitions.find(c);
            if(it != states_[s].transitions.end()){
                next.insert(it->second.begin(), it->second.end());
            }
        }
        auto next_closure = epsilonClosure(next, pos + 1);
        current = next_closure.states;
        for(const auto& [g, start] : next_closure.started_groups){
            group_starts[g] = start;
            
        }
        for(const auto& [g, end]:next_closure.ended_groups){
            group_ends[g] = end;
        }
        
        pos++;
    }
    for(int s : current){
        if(states_[s].is_accepting){
            result.success = true;
            break;
        }
    }
    
    if(result.success){
        result.full_match = input.substr(start_pos, pos - start_pos);
        for (const auto& [g, start] : group_starts) {
            auto end_it = group_ends.find(g);
            size_t end = (end_it != group_ends.end()) ? end_it->second : pos;
            result.group_positions[g] = {start - start_pos, end - start_pos};
        }
    }
    return result;
}

NFA::ClosureResult NFA::epsilonClosure(const std::set<int>& states,size_t pos) const {
    ClosureResult result;
    result.states = states;
    std::queue<int> queue;
    for(int s : states){
        queue.push(s);
    }
    while(!queue.empty()){
        int s = queue.front();
        queue.pop();
        for(int next:states_[s].epsilon_transitions){
            if(result.states.insert(next).second){
                queue.push(next);
            }
        }
        for(const auto& [group, targets]:states_[s].group_start_transitions){
            for(int next : targets){
                if(result.states.insert(next).second){
                    if(result.started_groups.find(group) == result.started_groups.end()){
                        result.started_groups[group] = pos;
                    }
                    queue.push(next);
                }
            }
        }
        for(const auto& [group, targets]:states_[s].group_end_transitions){
            for(int next : targets){
                if(result.states.insert(next).second){
                    result.ended_groups[group] = pos;
                    queue.push(next);
                }
            }
        }
    }
    return result;
}
MatchResult NFA::match(const std::string& input)const{
    MatchResult result = simulateNFA(input,0);
    if(result.success){
        return result;
    }
    return MatchResult();
}




















std::string NFA::toDot() const {
    std::string result = "digraph NFA {\n";
    result += "    rankdir=LR;\n";
    result += "    node [shape=circle];\n";
    result += "    start [shape=point];\n";
    result += "    start -> " + std::to_string(start_state_) + ";\n";
    for (size_t i = 0; i < states_.size(); ++i) {
        if (states_[i].is_accepting) {
            result += "    " + std::to_string(i) + " [shape=doublecircle];\n";
        }
    }
    for (size_t from = 0; from < states_.size(); ++from) {
        for (const auto& [symbol, targets] : states_[from].transitions) {
            for (int to : targets) {
                std::string label = std::string(1, symbol);
                result += "    " + std::to_string(from) + " -> " + std::to_string(to);
                result += " [label=\"" + label + "\"];\n";
            }
        }
    }
    for (size_t from = 0; from < states_.size(); ++from) {
        for (int to : states_[from].epsilon_transitions) {
            result += "    " + std::to_string(from) + " -> " + std::to_string(to);
            result += " [label=\"ε\", color=blue];\n";
        }
    }
    for (size_t from = 0; from < states_.size(); ++from) {
        for (const auto& [group, targets] : states_[from].group_start_transitions) {
            for (int to : targets) {
                result += "    " + std::to_string(from) + " -> " + std::to_string(to);
                result += " [label=\"START(" + std::to_string(group) + ")\", color=green];\n";
            }
        }
    }
    for (size_t from = 0; from < states_.size(); ++from) {
        for (const auto& [group, targets] : states_[from].group_end_transitions) {
            for (int to : targets) {
                result += "    " + std::to_string(from) + " -> " + std::to_string(to);
                result += " [label=\"END(" + std::to_string(group) + ")\", color=red];\n";
            }
        }
    }
    result += "}\n";
    return result;
}
void NFA::saveNFAToDot(const NFA& nfa, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    file << nfa.toDot();
    file.close();
}


