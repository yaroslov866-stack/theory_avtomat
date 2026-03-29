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
    valid = valid && !server.empty();
}

void URLPars::validZone(){
    valid = valid && !zone.empty();
}

void URLPars::validPage(){
    valid = valid && !page.empty();
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



void URLPars::processFile(const std::string& filename){
    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return;
    }
    std::string line;
    int lineNum = 0;
    while(std::getline(file, line)){
        lineNum++;
        std::cout << "\nСтрока " << lineNum << ": " << line << std::endl;
        validUrl(line);
    }
    file.close();
}

void URLPars::processKeyboard(){
    std::cout << "Введите URL (пустая строка для выхода):\n";
    std::string line;
    int lineNum = 0;
    while(true){
        std::cout << "> ";
        std::getline(std::cin, line);
        if(line.empty()){
            break;
        }
        lineNum++;
        validUrl(line);
    }
}