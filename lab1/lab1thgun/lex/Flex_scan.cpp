#include "Flex_scan.h"
#include <FlexLexer.h>
#include <cstdio>


Flex_scan::Flex_scan(std::istream* stream) {}

bool Flex_scan::validUrl(const std::string& str) {
    std::istringstream stream(str);
    yyFlexLexer temp(&stream, &std::cout);
    
    int token = temp.yylex();
    
    if(token == 1){
        return true;
    }
    return false;
}

void Flex_scan::printStatistic(std::ostream& stream) const {
    stream << "\nСТАТИСТИКА (Flex)\n" << std::endl;
    for (const auto& [page, count] : table) {
        stream << "  " << page << " – " << count << std::endl;
    }
}

bool Flex_scan::addStatistic(const std::string &str) {
    std::istringstream iss(str);
    yyFlexLexer tempLexer(&iss, &std::cout);
    
    int token = tempLexer.yylex();
    std::string pageName = getPage();
    if(token == 1){
        table[pageName]++;
        return true;
    }
    return false;
}