#include "Flex_scan.h"
#include <FlexLexer.h>
Flex_scan::Flex_scan(std::istream* stream){}

bool Flex_scan::validUrl(const std::string& str) {
    std::istringstream stream(str);
    yyFlexLexer temp(&stream);    
    int token;
    while((token = temp.yylex()) != 0){
        if (token == 1) {
            return true;
        }
    }
    return false;
}