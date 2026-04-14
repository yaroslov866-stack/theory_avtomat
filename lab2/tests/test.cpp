#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../astbuild/parser.h"
#include "../astbuild/tokens.h"
#include "../dfa/dfa.h"
#include "../nfa/nfa.h"
#include "../pattern/pattern.h"
#include <iostream>
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
    //     ASTNode node5 = ASTNode::range(node,5,25);
    //    // REQUIRE(node5.type == ASTNode::Type::RANGE);
    //     REQUIRE(node5.literal == 0);
    //     REQUIRE(node5.range_max == 25);
    //     REQUIRE(node5.range_min == 5);
    //     REQUIRE(node5.position == 0);
    //     REQUIRE(node5.nullable == false);
    //     REQUIRE(node5.capt_num == 0);
    //     REQUIRE(node5.children[0].type == ASTNode::Type::LITERAL);
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
TEST_CASE("Parser"){
    SECTION("TOKEN"){
        std::string str = "a(b|c)+";
        std::vector<Token> tokenss = Parser::tokenization(str);
        std::vector<Token> tokens = Parser::insert_conc(tokenss);
        ASTNode node = Parser::parse(str);
        REQUIRE(node.children[0].type == ASTNode::Type::LITERAL);
        REQUIRE(node.children[1].type == ASTNode::Type::PLUS);
        REQUIRE(node.children[1].children[0].type == ASTNode::Type::OR);
        REQUIRE(node.children[1].children[0].children[0].type == ASTNode::Type::LITERAL);
        REQUIRE(node.children[1].children[0].children[1].type == ASTNode::Type::LITERAL);
        REQUIRE(node.children[1].children[0].children[0].children.empty());
        REQUIRE(node.children[1].children[0].children[0].children.empty());
        REQUIRE(node.children[0].children.empty());

        
    }
}




// str = "a(b|c)+v{1,5}";
//         tokenss = Parser::tokenization(str);
//         tokens = Parser::insert_conc(tokenss);
//         for(auto token:tokens){
//             std::string type;
//             switch(token.type){
//                 case Token::Type::LITERAL:
//                     type = "LITERAL(" + std::string(1,token.literal) + ")";
//                     break;
//                 case Token::Type::LEFT_PAR:
//                     type = "LEFT_PAR";
//                     break;
//                 case Token::Type::RIGHT_PAR:
//                     type = "RIGHT_PAR";
//                     break;
//                 case Token::Type::OP_OR:
//                     type = "OP_OR";
//                     break;
//                 case Token::Type::OP_PLUS:
//                     type =  "OP_PLUS";
//                     break;
//                 case Token::Type::OP_CONC:
//                     type = "OP_CONC";
//                     break;
//                 case Token::Type::OP_RANGE_START:
//                     if (token.range_max == -1) {
//                         type = "OP_RANGE_START(" + std::to_string(token.range_min) + ",)";
//                     } else {
//                         type = "OP_RANGE_START(" + std::to_string(token.range_min) + "," + 
//                             std::to_string(token.range_max) + ")";
//                     }
//             }
//             std::cout<<type<<" ";
//         }
//         std::cout<<std::endl;
//         node = Parser::parse(str);

/*
TEST_CASE("DFA"){
    SECTION("FOLLOWSPOS"){
        std::string str = "a(b|c)+v";
        ASTNode node = Parser::parse(str);
        ASTNode root = ASTNode::addMark(node);
        int num = 1;
        ASTNode::numeration(root,num);
        ASTNode::countNullable(root);
        ASTNode::countFirstposLastPos(root);
        REQUIRE(root.type == ASTNode::Type::CONCAT);
        std::vector<std::set<int>> followpos(num);
        DFA::computeFollowpos(root,followpos);
        REQUIRE(followpos[1].count(2) == true);
        REQUIRE(followpos[1].count(3) == true);
        REQUIRE(followpos[1].count(1) == false);
        REQUIRE(followpos[1].count(4) == false);
        REQUIRE(followpos[2].count(3) == true);
        REQUIRE(followpos[2].count(2) == true);
        REQUIRE(followpos[2].count(4) == true);
        REQUIRE(followpos[3].count(4) == true);
        REQUIRE(followpos[4].count(5) == true);
        REQUIRE(followpos[4].count(4) == false);
        REQUIRE(followpos[5].empty() == true);
        REQUIRE(DFA::findMarkerPos(root) == 5);
        std::map<int,char> posToChar;
        std::set<char> alphabet;
        DFA::collectSym(root,posToChar,alphabet);
        REQUIRE(alphabet.count('a') == true);
        REQUIRE(alphabet.count('b') == true);
        REQUIRE(alphabet.count('c') == true);
        REQUIRE(alphabet.count('v') == true);
        REQUIRE(alphabet.count('#') == true);
        REQUIRE(alphabet.count('s') == false);
        REQUIRE(alphabet.count('A') == false);
        REQUIRE(posToChar[1] == 'a');
        REQUIRE(posToChar[2] == 'b');
        REQUIRE(posToChar[3] == 'c');
        REQUIRE(posToChar[4] == 'v');
        REQUIRE(posToChar[5] == '#');
        REQUIRE(posToChar.size() == 5);
        DFA dfa = DFA::compile("a(b|c)+v");
        REQUIRE(dfa.accepts("abcv") == true);
        REQUIRE(dfa.accepts("abc") == false);
        REQUIRE(dfa.accepts("accv") == true);
        REQUIRE(dfa.accepts("abcccccccv") == true);
        REQUIRE(dfa.accepts("abcccc") == false);
        REQUIRE(dfa.accepts("bcv") == false);
        REQUIRE(dfa.accepts("abv") == true);
        REQUIRE(dfa.accepts("abcv#") == false);

        str = "a(b|c)+v{1,5}";
        node = Parser::parse(str);
        root = ASTNode::addMark(node);
        num = 1;
        ASTNode::numeration(root,num);
        ASTNode::countNullable(root);
        ASTNode::countFirstposLastPos(root);
        REQUIRE(root.type == ASTNode::Type::CONCAT);
        std::vector<std::set<int>> followpos1(num);
        DFA::computeFollowpos(root,followpos1);
        REQUIRE(followpos1[1].count(2) == true);
        REQUIRE(followpos1[1].count(3) == true);
        REQUIRE(followpos1[1].count(1) == false);
        REQUIRE(followpos1[1].count(4) == false);
        REQUIRE(followpos1[2].count(3) == true);
        REQUIRE(followpos1[2].count(2) == true);
        REQUIRE(followpos1[2].count(4) == true);
        REQUIRE(followpos1[3].count(4) == true);
        REQUIRE(followpos1[4].count(5) == true);
        REQUIRE(followpos1[4].count(4) == true);
        REQUIRE(DFA::findMarkerPos(root) == 5);
        REQUIRE(followpos1[5].empty() == true);
        posToChar.clear();
        alphabet.clear();
        DFA::collectSym(root,posToChar,alphabet);
        REQUIRE(alphabet.count('a') == true);
        REQUIRE(alphabet.count('b') == true);
        REQUIRE(alphabet.count('c') == true);
        REQUIRE(alphabet.count('v') == true);
        REQUIRE(alphabet.count('#') == true);
        REQUIRE(alphabet.count('s') == false);
        REQUIRE(alphabet.count('A') == false);
        REQUIRE(posToChar[1] == 'a');
        REQUIRE(posToChar[2] == 'b');
        REQUIRE(posToChar[3] == 'c');
        REQUIRE(posToChar[4] == 'v');
        REQUIRE(posToChar[5] == '#');
        REQUIRE(posToChar.size() == 5);
        dfa = DFA::compile("l(l|d)+");
        std::string s = "dfa.dot";
        DFA::saveToDot(dfa,s);
        // REQUIRE(dfa.accepts("abcv") == true);
        // REQUIRE(dfa.accepts("abc") == false);
        // REQUIRE(dfa.accepts("accv") == true);
        // REQUIRE(dfa.accepts("abcccccccv") == true);
        // REQUIRE(dfa.accepts("abcccc") == false);
        // REQUIRE(dfa.accepts("bcv") == false);
        // REQUIRE(dfa.accepts("abv") == true);
        // REQUIRE(dfa.accepts("abcv#") == false);
        // REQUIRE(dfa.accepts("abcvvvvv") == true);
        // REQUIRE(dfa.accepts("abcvvvvvvvvvv") == false);
        // REQUIRE(dfa.accepts("abc") == false);


    }
}
*/
TEST_CASE("NFA"){
    SECTION("Method Metka"){
        std::string str = "v{1,3}";
        ASTNode node = Parser::parse(str);
        NFA nfa = NFA::fromAST(node);
        std::string s = "nfa.dot";
        s = "nfa.dot";
        NFA::saveNFAToDot(nfa,s);
        DFA dfa1 = DFA::compile("c");
        s = "dfa1.dot";
        DFA::saveToDot(dfa1,s);
        auto ma = nfa.match("aaaa");

        // auto ma1 = nfa.match("aacvbndq");
        // REQUIRE(ma1.match()== false);
        // REQUIRE(nfa.hasCaptureGroups() == true);
        // REQUIRE(ma.match()== true);
        // REQUIRE(ma.group(1) == "v");
        // REQUIRE(ma.group(2) == "qq");
        // std::string s = "nfa.dot";
        // NFA::saveNFAToDot(nfa,s);
        // DFA dfa = nfa.makeDFA();
        // s = "dfa.dot";
        // DFA::saveToDot(dfa,s);
        // //DFA dfa1 = DFA::compile(str);
        // s = "dfa1.dot";
        // DFA::saveToDot(dfa1,s);
        // NFA::Metka m1 = NFA::buildSym('a',nfa);
        // NFA::Metka m2 = NFA::buildSym('b',nfa);
        // NFA::Metka m3 = NFA::buildConcat(m1,m2,nfa);
        // REQUIRE(nfa.states_[0] == )
        // NFA::Metka m1 = NFA::buildSym('a',nfa);

    }
}


/*
DFA dfa = DFA::compile("a(b|c)+");
        REQUIRE(dfa.accepts("abc") == false);
        REQUIRE(dfa.accepts("acc") == true);
        REQUIRE(dfa.accepts("b") == false);
        REQUIRE(dfa.accepts("aab") == false);
        REQUIRE(dfa.accepts("ac") == true);
        REQUIRE(dfa.accepts("Ab") == false);
        REQUIRE(dfa.accepts("c") == false);
        REQUIRE(dfa.accepts("a#") == false);
        REQUIRE(dfa.accepts("abbb") == true);
        REQUIRE(dfa.accepts("abb") == true);
        REQUIRE(dfa.accepts("a") == false);
        DFA dfa1 = DFA::compile("a*(b|c)d*e");
        REQUIRE(dfa1.accepts("abde") == true);
        */
TEST_CASE("Pattern"){
    SECTION("constructor"){
        Pattern pattern("acb(1:2)");

    }
}