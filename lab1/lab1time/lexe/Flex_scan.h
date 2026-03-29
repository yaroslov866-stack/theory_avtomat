#pragma once 

#include "../IUrl.h"
#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <sstream>
#include <cctype>
class yyFlexLexer;
class Flex_scan : public IUrl {
public:
    Flex_scan(std::istream* stream);
    virtual ~Flex_scan()=default;
    
    bool validUrl(const std::string& str) override;
};