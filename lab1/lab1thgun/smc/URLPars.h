#pragma once
#include <iostream>
#include <string>
#include <map>
#include <cctype>
#include <algorithm>
#include <fstream>
#include "URLPars_sm.h"
#include "../IUrl.h"
class URLPars : public IUrl{
public:
    std::string buffer;
    std::string protocol;
    std::string server;
    std::string zone;
    std::string page;
    std::map<std::string, int> statistics;
    bool valid;
    size_t charCount;

    URLParsFSM _fsm;
    URLPars();
    virtual ~URLPars() = default;

    bool validUrl(const std::string& url) override;
    void printStatistic(std::ostream& stream) const override;
    bool addStatistic(const std::string& str) override;

    void startFSM();
    
    void clearBuffer();
    void addChar(char c);
    void saveProtocol();
    void saveServer();
    void saveZone();
    void savePage();
    void validProtocol();
    void validServer();
    void validZone();
    void validPage();
    void result();
    void reset();
    void setSuccess(bool status);

    bool isValidServerChar(char c) const;
    bool isValidZoneChar(char c) const;
    bool isValidPageChar(char c) const;
};