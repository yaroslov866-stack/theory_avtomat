#pragma once

struct Token{
    enum class Type{
        LITERAL,
        LEFT_PAR,
        RIGHT_PAR,
        OP_OR,
        OP_PLUS,
        OP_CONC,
        OP_RANGE_START,
        CAPTURE_START,
        EPSILON
    };
    Type type;
    char literal;
    int number;
    int range_min;
    int range_max;

    Token():type(Type::LITERAL),literal(0),number(0),range_min(0),range_max(-1){}
    explicit Token(Type t) : type(t),literal(0),number(0),range_min(0),range_max(-1){}
    explicit Token(char c) : type(Type::LITERAL),literal(c),number(0),range_min(0),range_max(-1){}
    Token(Type t,int n) : type(t),literal(0),number(n),range_min(0),range_max(-1){}
    Token(int min,int max) : type(Type::OP_RANGE_START),literal(0),number(0),range_min(min),range_max(max){}

};
