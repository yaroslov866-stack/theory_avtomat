#include "URLPars.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <fstream>

URLPars::URLPars():valid(true),charCount(0),_fsm(*this){}

void URLPars::startFSM() {
    _fsm.enterStartState();
}

bool URLPars::validUrl(const std::string& url) {
    _fsm.~URLParsFSM();
    new (&_fsm) URLParsFSM(*this);
    reset();      
    startFSM(); 
    for (char c : url) {
        _fsm.curr_char(c);
    }
    _fsm.eof();
    return valid;
}
void URLPars::printStatistic(std::ostream& stream)const{
    stream << "\n=== СТАТИСТИКА ИСПОЛЬЗОВАНИЯ СТРАНИЦ ===\n";
    if(statistics.empty()){
        stream << "Нет корректных URL\n";
    } 
    else{
        for(const auto& [pageName,count]:statistics){
            stream << "  " << pageName << " - " << count << "\n";
        }
    }
}

bool URLPars::addStatistic(const std::string& str){
    return true;
}

void URLPars::clearBuffer(){
    buffer.clear();
    charCount = 0;
}

void URLPars::addChar(char c){
    buffer += c;
    charCount++;
}

bool URLPars::isValidServerChar(char c)const{
    return std::isalpha(c) != 0;
}

bool URLPars::isValidZoneChar(char c)const{
    return std::isalpha(c) != 0;
}

bool URLPars::isValidPageChar(char c)const{
    return std::isalnum(c) != 0;
}

void URLPars::saveProtocol(){
    protocol = buffer;
    clearBuffer();
}

void URLPars::saveServer(){
    server = buffer;
    clearBuffer();
}

void URLPars::saveZone(){
    zone = buffer;
    clearBuffer();
}

void URLPars::savePage(){
    page = buffer;
    clearBuffer();
}
void URLPars::validProtocol(){
    valid = valid && (protocol=="http" ||protocol == "https");
}

void URLPars::validServer(){
    valid = valid && !server.empty() && server.length() <= 20;
}

void URLPars::validZone(){
    valid = valid && !zone.empty() && zone.length() <= 5;
}

void URLPars::validPage(){
    valid = valid && !page.empty() && page.length() <= 20;
}

void URLPars::result(){
    if(valid){
        statistics[page]++;
    }
}

void URLPars::reset(){
    buffer.clear();
    protocol.clear();
    server.clear();
    zone.clear();
    page.clear();
    valid = true;
    charCount = 0;
}

void URLPars::setSuccess(bool status){
    if(!status){
        valid = false;
    }
}
