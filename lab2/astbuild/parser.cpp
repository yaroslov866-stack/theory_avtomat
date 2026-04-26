#include "parser.h"
#include <iostream>
#include <stack>
ASTNode Parser::parse(const std::string& str){
    try{
        std::vector<Token> tokens = tokenization(str);
        std::vector<Token> tokens_conc = insert_conc(tokens);
        ASTNode node = buildTree(tokens_conc);
        return node;
    }
    catch(...){
        throw;
    }
    
}

int Parser::find_number(const std::string& str,size_t& i){
    if(str.empty())throw std::runtime_error("Empty find_number");
    int num = 0;
    if(i>= str.length() || !std::isdigit(str[i])){
        throw (std::runtime_error("Error { or ("));
    }
    while(i<str.length() && std::isdigit(str[i])){
        num = num*10 + (str[i]- '0');
        i++;
    }
    return num;
}


bool Parser::canbeleft(const Token& token){
    switch(token.type){
        case Token::Type::LITERAL:
        case Token::Type::RIGHT_PAR:
        case Token::Type::OP_RANGE_START:
        case Token::Type::OP_PLUS:
        case Token::Type::EPSILON:
            return true;
        default:
            return false;
    }
}

bool Parser::canberight(const Token& token){
    switch(token.type){
        case Token::Type::LITERAL:
        case Token::Type::LEFT_PAR:
        case Token::Type::EPSILON:
        case Token::Type::CAPTURE_START:
            return true;
        default:
            return false;
    }
}

int Parser::prioritet(const Token& token){
    int fl = 0;
    switch(token.type){
        case Token::Type::OP_PLUS:
            fl = 2;
            break;
        case Token::Type::OP_CONC:
            fl = 1;
            break;
        case Token::Type::OP_OR:
            fl = 0;
            break;
        default:
            break;
    }
    return fl;
}

std::vector<Token> Parser::tokenization(const std::string& str){
    std::vector<Token> res;
    for(size_t i = 0;i<str.length();i++){
        char c = str[i];
        Token tok;
        if(std::isalpha(c)){
            tok.literal = c;
            tok.type = Token::Type::LITERAL;
            res.push_back(tok);
            continue;
        }
        else if(c == '('){
            if(i < str.length() && std::isdigit(str[i+1])){
                tok.type = Token::Type::CAPTURE_START;
                i++;
                tok.number = find_number(str,i);
                if(str[i] == ':'){
                    res.push_back(tok);
                    continue;
                }
                else{
                    tok.type = Token::Type::LEFT_PAR;
                    res.push_back(tok);
                    continue;
                }
            }
            else{
                tok.type = Token::Type::LEFT_PAR;
                res.push_back(tok);
                continue;
            }
        }
        else if(c == ')'){
            tok.type = Token::Type::RIGHT_PAR;
            res.push_back(tok);
            continue;
        }
        else if( c == '|'){
            tok.type = Token::Type::OP_OR;
            res.push_back(tok);
            continue;
        }
        else if(c == '+'){
            tok.type = Token::Type::OP_PLUS;
            tok.range_min = 1;
            tok.range_max = -1;
            res.push_back(tok);
            continue;
        }
        else if(c == '{'){
            if(i+1>=str.length() || isalpha(str[i+1])){
                throw std::runtime_error("Error { ");
            }
            tok.type = Token::Type::OP_RANGE_START;
            i++;
            int min = find_number(str,i);
            if(i >= str.length()){
                throw std::runtime_error("Unclosed {");
            }
            if(str[i+1] == '}'){
                tok.range_max = -1;
                i++;
            }
            else if(str[i] == ','){
                i++;
                if(i < str.length() && std::isdigit(str[i])){
                    int max = find_number(str, i);
                    tok.range_max = max;
                }
                else{
                    tok.range_max = -1;
                }
                if(i >= str.length() || str[i] != '}'){
                    throw std::runtime_error("Missing } in range");
                }
                i++;
                tok.range_min = min;
            }
            else{
                throw std::runtime_error("Invalid range syntax");
            }
            res.push_back(tok);
            continue;
        }
        else if(c == '#' && i<str.length()){
            i++;
            c = str[i];
            tok.type = Token::Type::LITERAL;
            tok.literal = c;
            res.push_back(tok);
            continue; 
        }
        else if(c == '^'){
            tok.type = Token::Type::EPSILON;
            res.push_back(tok);
            continue;
        }
        else{
            tok.literal = c;
            tok.type = Token::Type::LITERAL;
            res.push_back(tok);
            continue;
        }
    }
    return res;
}

std::vector<Token> Parser::insert_conc(const std::vector<Token>& tokens){
    std::vector<Token> result;
    for(size_t i=0;i<tokens.size()-1;i++){
        result.push_back(tokens[i]);
        if(canbeleft(tokens[i]) && canberight(tokens[i+1])){
            Token token;
            token.type = Token::Type::OP_CONC;
            result.push_back(token);
        }
    }
    result.push_back(tokens[tokens.size()-1]);
    return result;
}

ASTNode Parser::buildTree(const std::vector<Token>& tok){
    std::vector<Token> tokens = tok;
    std::stack<ASTNode> leaf_st;
    std::stack<Token> oper_st;
    for(auto& token:tokens){
        if(token.type == Token::Type::LITERAL){
            leaf_st.push(ASTNode::literal_(token.literal));
        }
        else if(token.type == Token::Type::EPSILON){
            leaf_st.push(ASTNode::epsilon());
        }
        else if(token.type == Token::Type::OP_PLUS){
            if(leaf_st.empty()){
                throw std::runtime_error("Ничего нет перед +");
            }
            ASTNode child = leaf_st.top();
            leaf_st.pop();
            leaf_st.push(ASTNode::plus(child));
        }
        else if(token.type == Token::Type::OP_RANGE_START){
            if(leaf_st.empty()){
                throw std::runtime_error("Ничего нет перед {}");
            }
            ASTNode child = leaf_st.top();
            leaf_st.pop();
            leaf_st.push(ASTNode::range(child,token.range_min,token.range_max));
        }
        else if(token.type == Token::Type::LEFT_PAR ||token.type == Token::Type::CAPTURE_START){
            oper_st.push(token);
        }
        else if(token.type == Token::Type::RIGHT_PAR){
            while (!oper_st.empty() && oper_st.top().type != Token::Type::LEFT_PAR &&oper_st.top().type != Token::Type::CAPTURE_START) {
                Token op = oper_st.top();
                oper_st.pop();
                if(op.type ==Token::Type::OP_OR){
                    ASTNode right = leaf_st.top();
                    leaf_st.pop();
                    ASTNode left = leaf_st.top();
                    leaf_st.pop();
                    leaf_st.push(ASTNode::or_(left,right));
                }
                else if(op.type ==Token::Type::OP_CONC){
                    ASTNode right = leaf_st.top();
                    leaf_st.pop();
                    ASTNode left = leaf_st.top();
                    leaf_st.pop();
                    leaf_st.push(ASTNode::concat(left,right));
                }
                
            }
            if(oper_st.empty()) {
                throw std::runtime_error("Не найдено открытие скобки (");
            }
            if(!oper_st.empty() && oper_st.top().type == Token::Type::CAPTURE_START){
                Token capture = oper_st.top(); 
                oper_st.pop();
                ASTNode child = leaf_st.top(); 
                leaf_st.pop();
                leaf_st.push(ASTNode::capture_group(child, capture.number));
            }
            if(!oper_st.empty() && oper_st.top().type == Token::Type::LEFT_PAR){
                oper_st.pop();
            }
        }
        else if(token.type == Token::Type::OP_OR ||token.type == Token::Type::OP_CONC) {
            while(!oper_st.empty() && oper_st.top().type != Token::Type::LEFT_PAR &&oper_st.top().type != Token::Type::CAPTURE_START &&
                   prioritet(oper_st.top()) >= prioritet(token)) {
                Token op = oper_st.top();
                oper_st.pop();
                if(op.type ==Token::Type::OP_OR){
                    ASTNode right = leaf_st.top();
                    leaf_st.pop();
                    ASTNode left = leaf_st.top();
                    leaf_st.pop();
                    leaf_st.push(ASTNode::or_(left,right));
                }
                else if(op.type ==Token::Type::OP_CONC){
                    ASTNode right = leaf_st.top();
                    leaf_st.pop();
                    ASTNode left = leaf_st.top();
                    leaf_st.pop();
                    leaf_st.push(ASTNode::concat(left,right));
                }
            }
            oper_st.push(token);
        }
        
    }
    while(!oper_st.empty()){
        Token tok = oper_st.top();
        oper_st.pop();
        if(tok.type ==Token::Type::OP_OR){
            ASTNode right = leaf_st.top();
            leaf_st.pop();
            ASTNode left = leaf_st.top();
            leaf_st.pop();
            leaf_st.push(ASTNode::or_(left,right));
        }
        else if(tok.type ==Token::Type::OP_CONC){
            ASTNode right = leaf_st.top();
            leaf_st.pop();
            ASTNode left = leaf_st.top();
            leaf_st.pop();
            leaf_st.push(ASTNode::concat(left,right));
        }
    }
    if(leaf_st.size() !=1){
        throw std::runtime_error("ERROR stack:not empty");
    }
    return leaf_st.top();
}



