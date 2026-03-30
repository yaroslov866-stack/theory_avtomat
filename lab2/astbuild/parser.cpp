#include "parser.h"
namespace myregex{
ASTNode Parser::parse(const std::string& str){
    std::vector<Token> tokens = tokenization(str);
    ASTNode node = buildTree(tokens);
}

bool Parser::canbeleft(const Token& token){
    switch(token.type){
        case Token::Type::LITERAL:
        case Token::Type::RIGHT_PAR:
        case Token::Type::OP_RANGE_START:
        case Token::Type::OP_SLASHN:
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
        case Token::Type::OP_SLASHN:
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
                tok.number = find_number(str,i);
                res.push_back(tok);
                continue;
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
                throw;
            }
            tok.type = Token::Type::OP_RANGE_START;
            int min = find_number(str,i);
            if(str[i+1] == '}'){
                tok.range_max = -1;
                i +=2;
            }
            else{
                int max = find_number(str,i);
                tok.range_max = max;
                i++;
            }
            tok.range_min = min;
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
        else if(c == '\\'){
            if(i< str.length() && std::isdigit(str[i+1])){
                int num = find_number(str,i);
                tok.type = Token::Type::OP_SLASHN;
                tok.number = num;
                res.push_back(tok);
                continue;
            }
            tok.literal = c;
            tok.type = Token::Type::LITERAL;
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

}