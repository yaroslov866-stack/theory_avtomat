#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <fstream>
#include "smcc/URLPars.h"
#include "regex/RegexLarge.h"
#include "lexe/Flex_scan.h"
#include <chrono>
int main() {  
    URLPars smc;
    std::string input;
    Regex regex;
    Flex_scan lexer(nullptr);
    std::cout << "Введите название файла:\n";
    std::getline(std::cin, input);
    std::ifstream file(input);
    if (!file.is_open()) {
        std::cout << "Ошибка открытия файла\n";
        return 1;
    }
    std::chrono::duration<double> time;
    std::chrono::duration<double> time1;
    std::chrono::duration<double> time2;
    std::cout << "Чтение URL из файла:\n";
    while(std::getline(file, input)) {
        if (input.empty() || input == "*") 
            continue;
            
        bool result;
        auto start = std::chrono::high_resolution_clock::now();
        bool fl = regex.validUrl(input);
        auto end = std::chrono::high_resolution_clock::now();
        time += end - start;
        if(fl){
            std::cout << "Корректный URL\n";
        }
        else{
            std::cout << "Некорректный URL\n";
        }
        auto start1 = std::chrono::high_resolution_clock::now();
        result = lexer.validUrl(input);
        auto end1 = std::chrono::high_resolution_clock::now();
        time1 += end1 - start1;
        if(result) {
            std::cout << "Корректный URL\n";
        } else {
            std::cout << "Некорректный URL\n";
        }
        auto start2 = std::chrono::high_resolution_clock::now();
        result = smc.validUrl(input);
        auto end2 = std::chrono::high_resolution_clock::now();
        time2 += end2 - start2;
        if(result) {
            std::cout << "Корректный URL\n";
        } else {
            std::cout << "Некорректный URL\n";
        }
    }
    file.close();
    std::cout<<"Время Regex->"<<std::fixed<<time.count()/10<<std::endl;
    
    std::cout<<"Время Lex->"<<std::fixed<<time1.count()/10<<std::endl;

    std::cout<<"Время Smc->"<<std::fixed<<time2.count()/10<<std::endl;
    
    return 0;
}

