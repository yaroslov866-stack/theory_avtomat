#include "AST.h"


namespace myregex{
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
        node.nullable = false;
    }
    else if(node.type == ASTNode::Type::SLASHN){
        node.nullable = false;
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

}