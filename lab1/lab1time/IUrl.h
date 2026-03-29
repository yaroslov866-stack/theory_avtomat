#pragma once
#include <iostream>
#include <string>
#include <regex>
class IUrl{
    public:
        virtual ~IUrl() = default;
        virtual bool validUrl(const std::string&) = 0;
};