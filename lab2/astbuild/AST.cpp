#include "AST.h"


void ASTNode::countNullable(ASTNode& node){
    for(auto& child:node.children){
        countNullable(child);
    }
    if(node.type == ASTNode::Type::LITERAL){
        node.nullable = false;
    }
    else if(node.type == ASTNode::Type::OR){
        node.nullable = node.children[0].nullable || node.children[1].nullable;
    }
    else if(node.type == ASTNode::Type::CONCAT){
        node.nullable = node.children[0].nullable && node.children[1].nullable;
    }
    else if(node.type == ASTNode::Type::EPSILON){
        node.nullable = true;
    }
    else if(node.type == ASTNode::Type::CAPTURE_GROUP){
        node.nullable = node.children[0].nullable;
    }
    else if(node.type == ASTNode::Type::PLUS){
        if(!node.children.empty())node.nullable = node.children[0].nullable;
    }
    else if(node.type == ASTNode::Type::RANGE){
        if(node.range_min == 0){
            node.nullable = true;
        }
        else{
            node.nullable = node.children[0].nullable;
        }
    }
    else{
        node.nullable =false;
    }
}

void ASTNode::countFirstposLastPos(ASTNode& node){
    
    for(auto& child:node.children){
        countFirstposLastPos(child);
    }
    if(node.type == ASTNode::Type::LITERAL){
        node.firstpos.insert(node.position);
    }
    else if(node.type == ASTNode::Type::EPSILON){
        node.firstpos = {};
    }
    else if(node.type == ASTNode::Type::OR){
        node.firstpos = unityset(node.children[0].firstpos,node.children[1].firstpos);
    }
    else if(node.type == ASTNode::Type::CONCAT){
        if(node.children[0].nullable){
            node.firstpos = unityset(node.children[0].firstpos,node.children[1].firstpos);
        }
        else{
            node.firstpos = node.children[0].firstpos;
        }
    }
    else if(node.type == ASTNode::Type::PLUS){
        node.firstpos = node.children[0].firstpos; 
    }
    else if(node.type == ASTNode::Type::RANGE){
        node.firstpos = node.children[0].firstpos;
    }
    else if (node.type == ASTNode::Type::CAPTURE_GROUP) {
        node.firstpos = node.children[0].firstpos;
    }


    if(node.type == ASTNode::Type::LITERAL){
        node.lastpos.insert(node.position);
    }
    else if(node.type == ASTNode::Type::EPSILON){
        node.lastpos = {};
    }
    else if(node.type == ASTNode::Type::OR){
        node.lastpos = unityset(node.children[0].lastpos,node.children[1].lastpos);
    }
    else if(node.type == ASTNode::Type::CONCAT){
        if(node.children[1].nullable){
            node.lastpos = unityset(node.children[0].lastpos,node.children[1].lastpos);
        }
        else{
            node.lastpos = node.children[1].lastpos;
        }
    }
    else if(node.type == ASTNode::Type::PLUS){
        node.lastpos = node.children[0].lastpos; 
    }
    else if (node.type == ASTNode::Type::RANGE) {
        node.lastpos = node.children[0].lastpos;
    }
    else if (node.type == ASTNode::Type::CAPTURE_GROUP) {
        node.lastpos = node.children[1].lastpos;
    }
}

std::set<int> ASTNode::unityset(const std::set<int>& first,const std::set<int>& second){
    std::set<int> res = first;
    for(auto& tp:second){
        res.insert(tp);
    }
    return res;
}

void ASTNode::numeration(ASTNode& node,int& count){
    if(node.type == ASTNode::Type::LITERAL || node.type == ASTNode::Type::EPSILON){
        node.position = count++;
    }
    for(auto& child:node.children){
        numeration(child,count);
    }
}
ASTNode ASTNode::addMark(ASTNode& root){
    ASTNode first = ASTNode::literal_('#');
    return ASTNode::concat(root,first);
}
