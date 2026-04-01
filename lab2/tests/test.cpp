#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../astbuild/parser.h"
#include "../astbuild/tokens.h"
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
    }
}