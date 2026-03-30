#pragma once
#include "../IUrl.h"
#include <map>
class Regex:public IUrl{
    private:
        std::map<std::string,int> table;
        std::regex NameUrl;
        std::regex NameServer;
        std::string getPageName(const std::string& str){
            std::smatch Url;
            if(std::regex_match(str,Url,NameUrl)){
                return Url[4].str();
            }
            return "";
        }
    public:
        Regex(){
            std::string protocol = "(https?)";
            std::string name_server = "([a-zA-Z]{1,20})";
            std::string name_zone = "([a-zA-Z]{1,5})";
            std::string name_page = "([a-zA-Z0-9]{1,20})";
            std::string domen = "(html)";
            std::string pat = "^\\s*"+protocol+"://"+name_server+"\\."+name_zone+"/"+name_page+"\\."+domen+"\\s*$";
            NameUrl = std::regex(pat,std::regex::icase);
            std::string serName = "^"+name_server+"$";
            NameServer = std::regex(serName);
        }
        virtual ~Regex() = default;
        bool validUrl(const std::string &str) override{
            std::smatch UrlAll;
            if(!std::regex_match(str,UrlAll,NameUrl)){
                return false;
            }
            std::string ser = UrlAll[2].str();
            return std::regex_match(ser,NameServer);
        }
        
        bool addStatistic(const std::string &str)override{
            std::string page = getPageName(str);
            if(page.empty()){
                return false;
            }
            auto it = table.find(page);
            if(it != table.end()){
                it->second += 1;
            }
            else{
                table[page] = 1;
            }
            return true;
        }
        void printStatistic(std::ostream& stream)const override{
            stream << "Статистика"<<std::endl;
            for(const auto&[page,count]:table){
                stream<<page<<"- "<<count<<std::endl;
            }
        }

};