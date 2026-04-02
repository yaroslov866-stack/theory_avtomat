#pragma once
#include <vector>
#include <set>
namespace myregex{
struct ASTNode{
    enum class Type{
        LITERAL,
        EPSILON,
        CONCAT,
        OR,
        PLUS,
        RANGE,
        CAPTURE_GROUP,
        SLASHN
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
    explicit ASTNode(int slasn):type(Type::SLASHN),literal(0),range_min(0),range_max(-1),capt_num(slasn),nullable(false),position(0){}
    
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
        ASTNode node;
        node.type = Type::RANGE;
        node.range_min= min;
        node.range_max = max;
        node.children.push_back(child);
        return node;
    }
    static ASTNode capture_group(const ASTNode& child,int num){
        ASTNode node;
        node.type = Type::CAPTURE_GROUP;
        node.capt_num = num;
        node.children.push_back(child);
        return node;
    }
    static ASTNode slashn(int num){
        return ASTNode(num);
    }

    bool is_leaf() const{
        return type == Type::LITERAL || type == Type::EPSILON || type == Type::SLASHN;
    }
    bool is_binary()const{
        return type == Type::CONCAT || type == Type::OR;
    }
    bool is_unary()const{
        return type == Type::PLUS || type == Type::RANGE || type == Type::CAPTURE_GROUP;
    }

};

}