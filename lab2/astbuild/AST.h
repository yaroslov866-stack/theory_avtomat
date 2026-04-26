#pragma once
#include <vector>
#include <set>
struct ASTNode{
    enum class Type{
        LITERAL,
        EPSILON,
        CONCAT,
        OR,
        PLUS,
        RANGE,
        CAPTURE_GROUP
    };
    Type type;
    char literal;
    int range_min;
    int range_max;
    int capt_num;
    std::vector<ASTNode> children;

    std::set<int> firstpos;
    std::set<int> lastpos;
    bool nullable;
    int position;

    ASTNode():type(Type::LITERAL),literal(0),range_min(0),range_max(-1),capt_num(0),nullable(false),position(0){}
    explicit ASTNode(char c):type(Type::LITERAL),literal(c),range_min(0),range_max(-1),capt_num(0),nullable(false),position(0){}
    
    static void countNullable(ASTNode& node);
    static void countFirstposLastPos(ASTNode& node);
    static void numeration(ASTNode& node,int& count);
    static ASTNode addMark(ASTNode& root);
    static std::set<int> unityset(const std::set<int>& first,const std::set<int>& second);

    static ASTNode literal_(char c){
        return ASTNode(c);
    }
    static ASTNode epsilon(){
        ASTNode node;
        node.type = Type::EPSILON;
        node.nullable = true;
        return node;
    }

    static ASTNode concat(const ASTNode& left,const ASTNode& right){
        ASTNode node;
        node.children.push_back(left);
        node.children.push_back(right);
        node.type = Type::CONCAT;
        return node;
    }
    static ASTNode or_(const ASTNode& left,const ASTNode& right){
        ASTNode node;
        node.type = Type::OR;
        node.children.push_back(left);
        node.children.push_back(right);
        return node;
    }

    static ASTNode plus(const ASTNode& child){
        ASTNode node;
        node.type = Type::PLUS;
        node.children.push_back(child);
        return node;
    }

    static ASTNode range(const ASTNode& child,int min,int max){
        if(min == 0 && max == 0){
            return epsilon();
        }
        if(min == 0 && max == -1){
            return or_(epsilon(),plus(child));
        }
        if(min == 0 && max != -1){
            ASTNode result =epsilon();
            ASTNode current = child;
            for(int i = 1; i <= max; ++i){
                result = or_(result, current);
                if(i < max){
                    current = concat(current, child);
                }
            }
            return result;
        }
        if(max == -1){
            ASTNode result = child;
            for(int i = 1;i<min;++i){
                result = concat(result,child);
            }
            ASTNode star = or_(epsilon(), plus(child));
            return concat(result,star);
        }
        if(min == max){
            ASTNode result = child;
            for(int i = 1;i<min;++i){
                result = concat(result,child);
            }
            return result;
        }
        ASTNode result = child;
        for(int i = 1;i<min;++i){
            result = concat(result,child);
        }
        ASTNode current = result;
        for(int i = min+1;i<= max;++i){
            current = concat(current,child);
            result = or_(result,current);
        }
        return result;
    }
    static ASTNode capture_group(const ASTNode& child,int num){
        ASTNode node;
        node.type = Type::CAPTURE_GROUP;
        node.capt_num = num;
        node.children.push_back(child);
        return node;
    }
    void hasCapture(const ASTNode& node,bool& fl){
        for(auto child:node.children){
            hasCapture(child,fl);
            if(child.capt_num != 0){
                fl = true;
                return;
            }
        }
        return;
    }
    bool is_leaf() const{
        return type == Type::LITERAL || type == Type::EPSILON;
    }
    bool is_binary()const{
        return type == Type::CONCAT || type == Type::OR;
    }
    bool is_unary()const{
        return type == Type::PLUS || type == Type::RANGE || type == Type::CAPTURE_GROUP;
    }

};

