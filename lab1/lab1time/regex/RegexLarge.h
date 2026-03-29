#pragma once
#include "../IUrl.h"
#include <map>
#include <regex>
#include <sstream>

class Regex:public IUrl{
    private:
        std::regex UrlPattern;
    public:
        Regex(){
            std::string name_server;
            std::string urlPattern;
            std::string protocol = "(https?)";
            name_server = "([a-zA-Z]+)";
            std::string name_zone = "([a-zA-Z]+)";
            std::string name_page = "([a-zA-Z0-9]+)";
            std::string extension = "(html)";        
            urlPattern = "^"+protocol + "://" + 
                        name_server + "\\." + 
                        name_zone + "/" + 
                        name_page + "\\." + 
                        extension+"$";
            UrlPattern = std::regex(urlPattern, std::regex::icase);  
        }
        virtual ~Regex() = default;
        bool validUrl(const std::string& str) override {
            std::smatch match;
            if(!std::regex_match(str, match, UrlPattern)){
                return false;
            }
            return true;
        }
};