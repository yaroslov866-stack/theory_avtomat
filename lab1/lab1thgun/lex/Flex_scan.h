#pragma once 
#include "../IUrl.h"
#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <sstream>
#include <cctype>

class yyFlexLexer;
extern std::string getPage();
class Flex_scan : public IUrl {
private:
    std::map<std::string, int> table;
public:
    Flex_scan(std::istream* stream);
    virtual ~Flex_scan() = default;
    
    bool validUrl(const std::string& str) override;
    void printStatistic(std::ostream& stream) const override;
    bool addStatistic(const std::string &str) override;
};