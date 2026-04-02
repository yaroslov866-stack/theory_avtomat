#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../astbuild/parser.h"
#include "../astbuild/tokens.h"
#include <iostream>
using namespace myregex;
TEST_CASE("AST"){
    SECTION("TOKEN_CONSTRUCTOR"){
        Token token('q');
        REQUIRE(token.type == Token::Type::LITERAL);
        REQUIRE(token.literal == 'q');
        REQUIRE(token.number == 0);
        REQUIRE(token.range_max == -1);
        REQUIRE(token.range_min == 0);
        Token token1(token);
        REQUIRE(token1.type == Token::Type::LITERAL);
        REQUIRE(token1.literal == 'q');
        REQUIRE(token1.number == 0);
        REQUIRE(token1.range_max == -1);
        REQUIRE(token1.range_min == 0);
        Token token2(1,25);
        REQUIRE(token2.type == Token::Type::OP_RANGE_START);
        REQUIRE(token2.literal == 0);
        REQUIRE(token2.number == 0);
        REQUIRE(token2.range_max == 25);
        REQUIRE(token2.range_min == 1);
        Token token3(Token::Type::CAPTURE_START,3);
        REQUIRE(token3.type == Token::Type::CAPTURE_START);
        REQUIRE(token3.literal == 0);
        REQUIRE(token3.number == 3);
        REQUIRE(token3.range_max == -1);
        REQUIRE(token3.range_min == 0);
        Token token4;
        REQUIRE(token4.type == Token::Type::LITERAL);
        REQUIRE(token4.literal == 0);
        REQUIRE(token4.number == 0);
        REQUIRE(token4.range_max == -1);
        REQUIRE(token4.range_min == 0);
    }
    SECTION("ASTNode constructor"){
        ASTNode node;
        ASTNode nodee;
        REQUIRE(node.type == ASTNode::Type::LITERAL);
        REQUIRE(node.literal == 0);
        REQUIRE(node.range_max == -1);
        REQUIRE(node.range_min == 0);
        REQUIRE(node.position == 0);
        REQUIRE(node.nullable == false);
        REQUIRE(node.capt_num == 0);
        ASTNode node1 = ASTNode::capture_group(node,1);
        REQUIRE(node1.type == ASTNode::Type::CAPTURE_GROUP);
        REQUIRE(node1.literal == 0);
        REQUIRE(node1.range_max == -1);
        REQUIRE(node1.range_min == 0);
        REQUIRE(node1.position == 0);
        REQUIRE(node1.nullable == false);
        REQUIRE(node1.capt_num == 1);
        REQUIRE(node1.children[0].type == ASTNode::Type::LITERAL);
        ASTNode node2 = ASTNode::concat(nodee,node);
        REQUIRE(node2.type == ASTNode::Type::CONCAT);
        REQUIRE(node2.literal == 0);
        REQUIRE(node2.range_max == -1);
        REQUIRE(node2.range_min == 0);
        REQUIRE(node2.position == 0);
        REQUIRE(node2.nullable == false);
        REQUIRE(node2.capt_num == 0);
        REQUIRE(node2.children[0].type == ASTNode::Type::LITERAL);
        REQUIRE(node2.children[1].type == ASTNode::Type::LITERAL);
        ASTNode node3 = ASTNode::literal_('a');
        REQUIRE(node3.type == ASTNode::Type::LITERAL);
        REQUIRE(node3.literal == 'a');
        REQUIRE(node3.range_max == -1);
        REQUIRE(node3.range_min == 0);
        REQUIRE(node3.position == 0);
        REQUIRE(node3.nullable == false);
        REQUIRE(node3.capt_num == 0);
        ASTNode node4 = ASTNode::or_(nodee,node);
        REQUIRE(node4.type == ASTNode::Type::OR);
        REQUIRE(node4.literal == 0);
        REQUIRE(node4.range_max == -1);
        REQUIRE(node4.range_min == 0);
        REQUIRE(node4.position == 0);
        REQUIRE(node4.nullable == false);
        REQUIRE(node4.capt_num == 0);
        REQUIRE(node4.children[0].type == ASTNode::Type::LITERAL);
        REQUIRE(node4.children[1].type == ASTNode::Type::LITERAL);
        ASTNode node5 = ASTNode::range(node,5,25);
        REQUIRE(node5.type == ASTNode::Type::RANGE);
        REQUIRE(node5.literal == 0);
        REQUIRE(node5.range_max == 25);
        REQUIRE(node5.range_min == 5);
        REQUIRE(node5.position == 0);
        REQUIRE(node5.nullable == false);
        REQUIRE(node5.capt_num == 0);
        REQUIRE(node5.children[0].type == ASTNode::Type::LITERAL);
        ASTNode node6 = ASTNode::epsilon();
        REQUIRE(node6.type == ASTNode::Type::EPSILON);
        REQUIRE(node6.literal == 0);
        REQUIRE(node6.range_max == -1);
        REQUIRE(node6.range_min == 0);
        REQUIRE(node6.position == 0);
        REQUIRE(node6.nullable == true);
        REQUIRE(node6.capt_num == 0);
        ASTNode node7 = ASTNode::plus(node);
        REQUIRE(node7.type == ASTNode::Type::PLUS);
        REQUIRE(node7.literal == 0);
        REQUIRE(node7.range_max == -1);
        REQUIRE(node7.range_min == 0);
        REQUIRE(node7.position == 0);
        REQUIRE(node7.nullable == false);
        REQUIRE(node7.capt_num == 0);
        REQUIRE(node7.children[0].type == ASTNode::Type::LITERAL);
        ASTNode node8 = ASTNode::slashn(7);
        REQUIRE(node8.type == ASTNode::Type::SLASHN);
        REQUIRE(node8.literal == 0);
        REQUIRE(node8.range_max == -1);
        REQUIRE(node8.range_min == 0);
        REQUIRE(node8.position == 0);
        REQUIRE(node8.nullable == false);
        REQUIRE(node8.capt_num == 7);
    }
    SECTION("Test function"){
        ASTNode node = ASTNode::literal_('a');
        ASTNode node1= ASTNode::literal_('b');
        ASTNode res = ASTNode::concat(node,node1);
        int count = 1;
        ASTNode res1 =  ASTNode::addMark(res);
        ASTNode::numeration(res1,count);
        REQUIRE(res1.position == 0);
        REQUIRE(res1.children[0].type == ASTNode::Type::CONCAT);
        REQUIRE(res1.children[1].type == ASTNode::Type::LITERAL);
        ASTNode::countNullable(res1);

        ASTNode::countFirstposLastPos(res1);
        REQUIRE(res1.firstpos.count(1) == 1);
        REQUIRE(res1.firstpos.count(2) == 0);
        REQUIRE(res1.lastpos.count(1) == 0);
        REQUIRE(res1.lastpos.count(3) == 1);
        REQUIRE(res1.children[0].firstpos.count(2) == 0);
        REQUIRE(res1.children[0].firstpos.count(1) == 1);
        REQUIRE(res1.children[1].firstpos.count(1) == 0);
        REQUIRE(res1.children[1].firstpos.count(2) == 0);
        REQUIRE(res1.children[1].firstpos.count(3) == 1);
        
        REQUIRE(res1.children[0].lastpos.count(1) == 0);
        REQUIRE(res1.children[0].lastpos.count(2) == 1);
        REQUIRE(res1.children[1].lastpos.count(1) == 0);
        REQUIRE(res1.children[1].lastpos.count(2) == 0);
        REQUIRE(res1.children[1].lastpos.count(3) == 1);

        REQUIRE(res1.children[0].children[0].firstpos.count(1) == 1);
        REQUIRE(res1.children[0].children[0].lastpos.count(1) == 1);
        REQUIRE(res1.children[0].children[1].firstpos.count(1) == 0);
        REQUIRE(res1.children[0].children[1].firstpos.count(2) == 1);
        REQUIRE(res1.children[0].children[1].lastpos.count(2) == 1);

        REQUIRE(res1.children[1].children.empty() ==true);

    }
}