#pragma once
#include <iostream>
#include <string>
#include <regex>
class IUrl{
    public:
        virtual ~IUrl() = default;
        virtual bool validUrl(const std::string&) = 0;
        virtual void printStatistic(std::ostream& stream)const = 0;
        virtual bool addStatistic(const std::string &str) = 0;
};