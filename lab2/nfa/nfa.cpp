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

    std::set<int> started = left.started_groups;
    started.insert(right.started_groups.begin(), right.started_groups.end());
    std::set<int> ended = left.ended_groups;
    ended.insert(right.ended_groups.begin(), right.ended_groups.end());

    Metka m;
    m.start = newStart;
    m.end = newEnd;
    m.started_groups = started;
    m.ended_groups = ended;
    return m;
}

NFA::Metka NFA::buildPlus(const Metka& child,NFA& nfa){
    nfa.addEpsilon(child.end,child.start);
    //nfa.setAccepting(child.end,false);
    Metka m;
    m.start = child.start;
    m.end = child.end;
    m.started_groups = child.started_groups;
    m.ended_groups = child.ended_groups;
    return m;
}
NFA::Metka NFA::buildRange(const Metka& child, int min, int max, NFA& nfa,const ASTNode& childNode){
    Metka result;
    if (max == -1) {
        if (min == 0){
            int newstart = nfa.addState(false);
            int newend = nfa.addState(true);
            nfa.addEpsilon(newstart,child.start);
            nfa.addEpsilon(child.end,child.start);
            nfa.addEpsilon(child.end,newend);
            nfa.addEpsilon(newstart,newend);
            nfa.setAccepting(child.end,false);
            Metka m;
            m.start = newstart;
            m.end = newend;
            m.started_groups = child.started_groups;
            m.ended_groups = child.ended_groups;
           
            return m;
        } 
        else{
            Metka starchild = buildFromAST(childNode,nfa);
            for(int i = 1; i < min; ++i){
                Metka next = buildFromAST(childNode, nfa);
                starchild = buildConcat(starchild, next, nfa);
            }
            int newstart = nfa.addState(false);
            int newend = nfa.addState(true);
            nfa.addEpsilon(starchild.end,newstart);
            nfa.addEpsilon(newstart,child.start);
            nfa.addEpsilon(child.end,child.start);
            nfa.addEpsilon(child.end,newend);
            nfa.addEpsilon(newstart,newend);
            nfa.setAccepting(child.end,false);
            nfa.setAccepting(starchild.end,false);
            Metka m;
            
            m.start = starchild.start;
            m.end = newend;
            m.started_groups = starchild.started_groups;
            m.ended_groups = starchild.ended_groups;
            return m;
        }
    }
    else{
        if(min ==0){
        
        }
        else{
            Metka starchild = child;
            for(int i = 1; i < min; ++i){
                Metka next = buildFromAST(childNode, nfa);
                starchild = buildConcat(starchild, next, nfa);
            }
            Metka left = buildFromAST(childNode,nfa);
            Metka right = buildEpsilon(nfa);
            if(max-min>1){
                nfa.addEpsilon(left.end,right.start);
            }
            for(int i = min; i < max-1; ++i){
                Metka next = buildFromAST(childNode, nfa);
                left = buildConcat(left, next, nfa); 
                Metka nexttt = buildEpsilon(nfa);
                right = buildConcat(right, nexttt, nfa);
                if(i<max-2)nfa.addEpsilon(next.end,nexttt.start);
            }
            Metka m = buildAlternation(left,right,nfa);
            Metka res;
            nfa.addEpsilon(starchild.end,m.start);
            nfa.setAccepting(starchild.end,false);
            res.start = starchild.start;
            res.end = m.end;
            return res;
            }

    }
    return result;
}

NFA::Metka NFA::buildCaptureGroup(const Metka& child, int number, NFA& nfa) {
    int newStart = nfa.addState(false);
    int newEnd = nfa.addState(true);
    nfa.addGroupStartTransition(newStart, child.start, number);
    nfa.addGroupEndTransition(child.end, newEnd, number);
    nfa.setAccepting(child.end, false);
    std::set<int> started = child.started_groups;
    started.insert(number);
    return {newStart,newEnd,started,child.ended_groups};
}

NFA::Metka NFA::buildFromAST(const ASTNode& node,NFA& nfa){
    switch(node.type){
        case ASTNode::Type::LITERAL:
            return buildSym(node.literal, nfa);
        case ASTNode::Type::EPSILON:
            return buildEpsilon(nfa);
        case ASTNode::Type::CONCAT: {
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
        
        case ASTNode::Type::RANGE:{
            Metka child = buildFromAST(node.children[0],nfa);
            return buildRange(child,node.range_min,node.range_max,nfa,node.children[0]);
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


NFA::ResultGroup NFA::epsilonMove(const std::set<int>& states) const {
    ResultGroup result;
    result.states = states;
    std::queue<int> queue;
    for(int s:states){
        queue.push(s);
    }
    while(!queue.empty()){
        int s = queue.front();
        queue.pop();
        for(int next:states_[s].epsilon_transitions){
            if(result.states.find(next) == result.states.end()){
                result.states.insert(next);
                queue.push(next);
            }
        }
        for(const auto& [group, targets]:states_[s].group_start_transitions){
            for(int next : targets){
                if(result.states.find(next) == result.states.end()){
                    result.states.insert(next);
                    result.started_groups.insert(group);
                    queue.push(next);
                }
            }
        }
        for(const auto& [group, targets]:states_[s].group_end_transitions){
            for(int next : targets){
                if(result.states.find(next) == result.states.end()) {
                    result.states.insert(next);
                    result.ended_groups.insert(group);
                    queue.push(next);
                }
            }
        }
    }
    return result;
}

std::set<int> NFA::move(const std::set<int>& states,char symbol)const{
    std::set<int> result;
    for(int s:states){
        auto it = states_[s].transitions.find(symbol);
        if(it != states_[s].transitions.end()){
            for(int target :it->second){
                result.insert(target);
            }
        }
    }
    return result;
}

NFA::ResultGroup NFA::moveWithGroups(const std::set<int>& states,char symbol)const{
    std::set<int> nextStates = move(states, symbol);
    return epsilonMove(nextStates);
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
    struct Config {
        std::set<int> states; 
        size_t pos;
        std::map<int, size_t> active_groups;
        std::map<int, size_t> completed_groups;
    };
    
    std::queue<Config> queue;
    MatchResult best_result;
    auto start_closure = epsilonMove({start_state_});
    Config start;
    start.states = start_closure.states;
    start.pos = start_pos;
    for (int g : start_closure.started_groups) {
        start.active_groups[g] = start_pos;
    }
    for (int g : start_closure.ended_groups) {
        if (start.active_groups.count(g)) {
            start.completed_groups[g] = start_pos;
        }
    }
    queue.push(start);
    while (!queue.empty()) {
        Config current = queue.front();
        queue.pop();
        bool is_accepting_state = false;
        for (int s : current.states) {
            if (states_[s].is_accepting) {
                is_accepting_state = true;
                break;
            }
        }
        if (is_accepting_state) {
            MatchResult result;
            result.success = true;
            result.full_match = input.substr(start_pos, current.pos - start_pos);
            for (const auto& [gid, gstart] : current.active_groups) {
                size_t gend = current.pos;
                auto it_end = current.completed_groups.find(gid);
                if (it_end != current.completed_groups.end()) {
                    gend = it_end->second;
                }
                result.group_positions[gid] = {gstart - start_pos, gend - start_pos};
            }
            if (result.full_match.length() > best_result.full_match.length()) {
                best_result = result;
            }
        }
        if (current.pos < input.size()) {
            char sym = input[current.pos];
            auto next_closure = moveWithGroups(current.states, sym);
            if (!next_closure.states.empty()) {
                Config next;
                next.states = next_closure.states;
                next.pos = current.pos + 1;
                next.active_groups = current.active_groups;
                next.completed_groups = current.completed_groups;
                for (int g : next_closure.started_groups) {
                    if (next.active_groups.find(g) == next.active_groups.end()) {
                        next.active_groups[g] = next.pos; 
                    }
                }
                for(int g : next_closure.ended_groups){
                    if (next.active_groups.count(g) && !next.completed_groups.count(g)) {
                        next.completed_groups[g] = next.pos;
                    }
                }
                queue.push(next);
            }
        }
    }
    return best_result;
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


