#include "dfa.h"
#include <queue>
#include <iostream>
#include <fstream>
#include <stack>
#include <functional>




DFA DFA::makeComplete(const std::set<char>& alphabet) const {
    DFA complete;
    complete.alphabet_ = alphabet;

    for (size_t i = 0; i < states_.size(); ++i) {
        complete.addState(states_[i].is_accept);
    }
    complete.setStartState(start_state);
    bool needSink = false;
    for (size_t i = 0; i < states_.size() && !needSink; ++i) {
        for (char c : alphabet) {
            if (states_[i].transitions.find(c) == states_[i].transitions.end()) {
                needSink = true;
                break;
            }
        }
    }

    int sink = -1;
    if (needSink) {
        sink = complete.addState(false);
        for (char c : alphabet)
            complete.addTransition(sink, c, sink);
    }

    for (size_t i = 0; i < states_.size(); ++i) {
        for (char c : alphabet) {
            auto it = states_[i].transitions.find(c);
            if (it != states_[i].transitions.end())
                complete.addTransition(i, c, it->second);
            else if (needSink)
                complete.addTransition(i, c, sink);
        }
    }

    return complete;
}
DFA DFA::subtract(const DFA& other) const {
    std::set<char> commonAlphabet = this->alphabet_;
    commonAlphabet.insert(other.alphabet_.begin(), other.alphabet_.end());
    DFA completeThis  = this->makeComplete(commonAlphabet);
    DFA completeOther = other.makeComplete(commonAlphabet);
    DFA result;
    result.alphabet_ = commonAlphabet;

    std::map<std::pair<int,int>, int> pairToState;
    std::queue<std::pair<int,int>> queue;
    auto getOrCreate = [&](int s1, int s2) -> int {
        auto key = std::make_pair(s1, s2);
        auto it = pairToState.find(key);
        if (it != pairToState.end())
            return it->second;
        bool accept = completeThis.states_[s1].is_accept &&
                      !completeOther.states_[s2].is_accept;
        int newId = result.addState(accept);
        pairToState[key] = newId;
        queue.push(key);
        return newId;
    };

    int startIdx = getOrCreate(completeThis.start_state, completeOther.start_state);
    result.setStartState(startIdx);

    while (!queue.empty()) {
        auto [s1, s2] = queue.front();
        queue.pop();
        int from = pairToState[{s1, s2}];

        for (char c : commonAlphabet) {
            int next1 = completeThis.states_[s1].transitions.at(c);
            int next2 = completeOther.states_[s2].transitions.at(c);
            int to = getOrCreate(next1, next2);
            result.addTransition(from, c, to);
        }
    }

    return result;
}

void DFA::addOrMergeTransition(int from, int to, const std::string& regex) {
    auto key = std::make_pair(from, to);
    auto it = regexTransitions_.find(key);
    if (it == regexTransitions_.end()) {
        regexTransitions_[key] = regex;
    } else {
        regexTransitions_[key] = "(" + it->second + "|" + regex + ")";
    }
}

std::string DFA::getTransitionRegex(int from, int to) const {
    auto it = regexTransitions_.find({from, to});
    if(it != regexTransitions_.end()){
        return it->second;
    }
    return "";
}

void DFA::eliminateState(int state) {
    std::vector<std::pair<int, std::string>> incoming;
    for (const auto& [key, regex] : regexTransitions_) {
        if (key.second == state && key.first != state) {
            incoming.push_back({key.first, regex});
        }
    }
    std::vector<std::pair<int, std::string>> outgoing;
    for (const auto& [key, regex] : regexTransitions_) {
        if (key.first == state && key.second != state) {
            outgoing.push_back({key.second, regex});
        }
    }
    std::string loop;
    auto it = regexTransitions_.find({state, state});
    if(it != regexTransitions_.end()){
        loop = it->second;
    }
    for(const auto& [from, inRegex] : incoming){
        for(const auto& [to, outRegex] : outgoing){
            std::string newRegex;
            if(loop.empty()){
                newRegex = inRegex + outRegex;
            }
            else{
                newRegex = inRegex + "(" + loop + ")*" + outRegex;
            }
            addOrMergeTransition(from, to, newRegex);
        }
    }
    for(auto it = regexTransitions_.begin(); it != regexTransitions_.end(); ){
        if(it->first.first == state || it->first.second == state){
            it = regexTransitions_.erase(it);
        }
        else{
            ++it;
        }
    }
}

std::string DFA::simplifyRegex(const std::string& regex) const {
    std::string r = regex;
    bool changed; 
    do{
        changed = false;
        size_t pos;
        while ((pos = r.find("()")) != std::string::npos) {
            r.replace(pos, 2, "^");
            changed = true;
        }
        while((pos = r.find("(^)")) != std::string::npos){
            r.replace(pos, 3, "^");
            changed = true;
        }
        while((pos = r.find("^*")) != std::string::npos){
            r.replace(pos, 2, "^");
            changed = true;
        }
        pos = r.find("(|");
        if(pos != std::string::npos && pos > 0 && r[pos-1] == '('){
            size_t end = r.find(')', pos);
            if (end != std::string::npos) {
                std::string inner = r.substr(pos + 2, end - pos - 2);
                r.replace(pos - 1, end - pos + 2, inner + "?");
                changed = true;
                continue;
            }
        }
        pos = r.find("|)");
        if(pos != std::string::npos){
            size_t start = r.rfind('(', pos);
            if(start != std::string::npos){
                std::string inner = r.substr(start + 1, pos - start - 1);
                r.replace(start, pos - start + 2, inner + "?");
                changed = true;
                continue;
            }
        }
        while(r.find("^") == 0){
            r = r.substr(1);
            changed = true;
        }
        while(r.length() > 1 && r.back() == '^'){
            r = r.substr(0, r.length() - 1);
            changed = true;
        }
        if(r.length() >= 2 && r[0] == '(' && r.back() == ')'){
            std::string inner = r.substr(1,r.length() - 2);
            if (inner.find('|') == std::string::npos && inner.find('*') == std::string::npos) {
                r = inner;
                changed = true;
            }
        }
        if (r.length() >= 5 && r[0] == '(' && r.back() == ')') {
            std::string inner = r.substr(1, r.length() - 2);
            if (inner.length() >= 2 && inner.back() == '*') {
                r = inner;
                changed = true;
            }
        }
        for(char c = 'a'; c <= 'z'; ++c){
            std::string pattern = std::string(1,c) + "(" + std::string(1,c) + "?)*";
            pos = r.find(pattern);
            if (pos != std::string::npos) {
                r.replace(pos, pattern.length(), std::string(1, c) + "+");
                changed = true;
                break;
            }
        }
        pos = r.find(")(");
        if (pos != std::string::npos) {
            size_t start = r.rfind('(', pos);
            if (start != std::string::npos) {
                std::string group = r.substr(start, pos - start + 1);
                std::string pattern = group + "((" + group + ")?)*";
                size_t patPos = r.find(pattern);
                if(patPos != std::string::npos){
                    r.replace(patPos, pattern.length(), group + "+");
                    changed = true;
                }
            }
        }
        while((pos = r.find("^")) != std::string::npos){
            r.erase(pos, 1);
            changed = true;
        }
    }while(changed);
    
    return r;
}
std::string DFA::toRegex() const {
    DFA temp = *this;
    temp.regexTransitions_.clear();
    for (size_t i = 0; i < temp.states_.size(); ++i) {
        for (const auto& [symbol, to] : temp.states_[i].transitions) {
            std::string sym(1, symbol);
            temp.addOrMergeTransition(i, to, sym);
        }
    }
    for (size_t i = 0; i < temp.states_.size(); ++i) {
        temp.addOrMergeTransition(i, i, "^");
    }
    int newStart = temp.states_.size();
    temp.states_.push_back({});
    temp.addOrMergeTransition(newStart, start_state, "^");
    int newFinal = temp.states_.size();
    temp.states_.push_back({});
    for(size_t i = 0; i < states_.size(); ++i){
        if (states_[i].is_accept) {
            temp.addOrMergeTransition(i, newFinal, "^");
        }
    }
    std::vector<int> toRemove;
    for(size_t i = 0;i<states_.size();++i){
        if(i != newStart && i != newFinal){
            toRemove.push_back(i);
        }
    }
    for (int state : toRemove) {
        temp.eliminateState(state);
    }
    std::string result = temp.getTransitionRegex(newStart, newFinal);
    return simplifyRegex(result);
}






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
   int mark = num;
   
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


DFA DFA::reverse() const {
    if(states_.empty() || start_state == -1){
        return DFA();
    }
    struct NFAState{
        bool is_accept = false;
        std::map<char, std::set<int>> transitions;  
        std::set<int> epsilonTransitions;     
    };
    std::vector<NFAState> nfaStates;
    for(size_t i = 0;i<states_.size();++i)
        nfaStates.push_back(NFAState());
    for(size_t from = 0;from<states_.size(); ++from){
        for(const auto& [sym,to]:states_[from].transitions){
            nfaStates[to].transitions[sym].insert(from);//развернули переход
        }
    }
    int newStart = nfaStates.size();
    nfaStates.push_back(NFAState());
    for(size_t i=0;i<states_.size(); ++i){
        if(states_[i].is_accept){
            nfaStates[newStart].epsilonTransitions.insert(i);
        }
    }
    if(start_state >= 0 && start_state < nfaStates.size()){
        nfaStates[start_state].is_accept = true;
    }
    std::function<std::set<int>(std::set<int>)> epsilonClose =[&](std::set<int> statesSet)->std::set<int>{
        std::set<int> close = statesSet;
        std::stack<int> stack;
        for(int s:statesSet){
            stack.push(s);
        }
        while(!stack.empty()){
            int s = stack.top();
            stack.pop();
            for(int next:nfaStates[s].epsilonTransitions){
                if(close.insert(next).second){
                    stack.push(next);
                }
            }
        }
        return close;
    };
    std::set<int> startSet = epsilonClose({newStart});
    DFA result;
    result.alphabet_ = this->alphabet_;
    std::map<std::set<int>, int> setToStateId;
    std::queue<std::set<int>> queue;
    auto getOrCreate = [&](const std::set<int>& sset) -> int {
        auto it = setToStateId.find(sset);
        if(it != setToStateId.end()){return it->second;}
        int id = result.addState();
        setToStateId[sset] = id;
        queue.push(sset);
        return id;
    };
    int startId = getOrCreate(startSet);
    result.setStartState(startId);
    while(!queue.empty()){
        std::set<int> currentSet = queue.front();
        queue.pop();
        int curId = setToStateId[currentSet];
        bool accept = (currentSet.find(start_state) != currentSet.end());//проверка на допускающее по старому дка
        result.setAccepting(curId, accept);
        for(char c:result.alphabet_){//пробегаемся по всему алфавиту,находим в какое состояние можетпопасть из текущего 
            std::set<int> nextSet;
            for(int s : currentSet){
                auto it = nfaStates[s].transitions.find(c);
                if(it != nfaStates[s].transitions.end()){
                    for(int t : it->second){
                        nextSet.insert(t);
                    }
                }
            }
            if(!nextSet.empty()){
                nextSet = epsilonClose(nextSet);
                int nextId = getOrCreate(nextSet);
                result.addTransition(curId, c, nextId);
            }
        }
    }
    return result;
}

std::vector<std::set<int>> DFA::getStartInitialization()const{
    std::vector<std::set<int>> partition;
    std::set<int> accepting;
    std::set<int> nonAccepting;
    for(size_t i = 0;i<states_.size();++i){
        if(states_[i].is_accept){
            accepting.insert(i);
        }
        else{
            nonAccepting.insert(i);
        }
    }
    if(!nonAccepting.empty()){
        partition.push_back(nonAccepting);
    }
    if(!accepting.empty()){
        partition.push_back(accepting);
    }
    
    return partition;
}

int DFA::findIndexState(const std::vector<std::set<int>>& partition, int state)const{
    for(size_t i = 0;i<partition.size(); ++i){
        if(partition[i].find(state) != partition[i].end()){
            return i;
        }
    }
    return -1;
}

std::vector<int> DFA::getNextState(int state, const std::vector<std::set<int>>& partition) const{//смотрим в какую группу ведет состояние в допускающую или нет или в другие
    std::vector<int> NextState;
    for(char symbol:alphabet_){
        auto it = states_[state].transitions.find(symbol);
        if(it != states_[state].transitions.end()){
            int nextState = it->second;
            NextState.push_back(findIndexState(partition, nextState));
        }
        else{
            NextState.push_back(-1);
        }
    }
    return NextState;
}

std::vector<std::set<int>> DFA::refinePartition(const std::vector<std::set<int>>& partition) const {
    std::vector<std::set<int>> newPartition;
    for(const auto& group:partition){
        if(group.size() == 1){
            newPartition.push_back(group);
            continue;
        }
        std::map<std::vector<int>,std::set<int>> subgroups;
        for(int state:group){
            subgroups[getNextState(state, partition)].insert(state);
        }
        for(const auto& [state, subgroup]:subgroups){
            newPartition.push_back(subgroup);
        }
    }
    return newPartition;
}

DFA DFA::buildFromPartition(const std::vector<std::set<int>>& partition) const {
    DFA result;
    std::map<int, int> oldToNew;
    for(size_t i = 0;i<partition.size();++i){
        int newState = result.addState();
        for(int oldState:partition[i]){
            oldToNew[oldState]=newState;
        }
        bool isAccepting = false;
        for(int oldState:partition[i]){
            if(states_[oldState].is_accept){
                isAccepting = true;
                break;
            }
        }
        result.setAccepting(newState,isAccepting);
        if(partition[i].find(start_state) != partition[i].end()){
            result.setStartState(newState);
        }
    }
    for(const auto& group:partition){
        int state = *group.begin();
        int fromState = oldToNew[state];
        for(char symbol:alphabet_){
            auto it = states_[state].transitions.find(symbol);
            if(it != states_[state].transitions.end()){
                int toState = oldToNew[it->second];
                result.addTransition(fromState, symbol, toState);
            }
        }
    } 
    return result;
}

DFA DFA::minimize() const {
    std::vector<std::set<int>> partition = getStartInitialization();
    std::vector<std::set<int>> oldPartition;
    do{
        oldPartition = partition;
        partition = refinePartition(partition);
    }while(partition != oldPartition);
    return buildFromPartition(partition);
}




DFA::Capture_res DFA::search(const std::string& text) const {
    Capture_res result;
    result.find = false;
    if(start_state == -1)return result;
    size_t n = text.size();
    for(size_t start=0;start<=n;++start){
        int state = start_state;
        size_t pos = start;
        struct GroupInfo{
            size_t start;
            bool closed;
            size_t end;
        };
        std::map<int, GroupInfo> groups;
        bool everAccepted = false;
        size_t bestEnd = 0;
        std::map<int, std::pair<size_t, size_t>> bestGroups;
        auto saveAccept = [&](){
            everAccepted = true;
            bestEnd = pos;
            bestGroups.clear();
            for(const auto& [grp, info] : groups){
                if (info.closed){
                    bestGroups[grp] = {info.start, info.end};
                }
                else{
                    bestGroups[grp] = {info.start, pos};
                }
            }
        };
        while(pos < n){
            char c = text[pos];
            auto it = states_[state].transitions.find(c);
            if (it == states_[state].transitions.end()) break;
            state = it->second;
            ++pos;
            const State& st = states_[state];
            if(st.is_accept){
                saveAccept();
            }
        }
        if(everAccepted){
            result.find = true;
            result.start = start;
            result.end = bestEnd;
            result.capture = bestGroups;
            if(bestEnd == start){
                start = bestEnd;
            }
            else{
                return result;
            }
        }
    }
    return result;
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






