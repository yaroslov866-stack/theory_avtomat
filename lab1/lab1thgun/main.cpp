#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <unordered_map>
#include <fstream>
#include "smc/URLPars.h"
#include "regex/Regex.h"
#include "lex/Flex_scan.h"
int main() {
    Regex regex;
    Flex_scan lexer(nullptr);
    URLPars smc;
    std::cout << "\n===== MENU =====\n";
    std::cout << "Input source:\n";
    std::cout << "  1 - консоль\n";
    std::cout << "  2 - файл\n";
    std::cout << "Вид:\n";
    std::cout << "  1 - regex\n";
    std::cout << "  2 - lexer\n";
    std::cout << "  3 - smc\n";
    std::cout << "Введите: (например:12)->";
    
    std::string option;
    getline(std::cin, option);

    if (option == "exit" || option == "0") {
        std::cout << "\nВыход из программы\n";
        return 0;
    }

    if (option.size() != 2) {
        std::cout << "Должно быть два значения(без пробела)\n";
        return 1;
    }
    
    char input_mode = option[0];
    char process_mode = option[1];

    if ((input_mode != '1' && input_mode != '2') || 
        (process_mode < '1' || process_mode > '3')) {
        std::cout << "Неверный выбор режимов\n";
        return 1;
    }
    
    std::string input;

    if (input_mode == '2') {
        std::cout << "Введите название файла:\n";
        std::getline(std::cin, input);
        std::ifstream file(input);
        if (!file.is_open()) {
            std::cout << "Ошибка открытия файла\n";
            return 1;
        }

        std::cout << "Чтение URL из файла:\n";
        while (std::getline(file, input)) {
            if (input.empty() || input == "*") 
                continue;
                
            std::cout << "Обработка: " << input << " -> ";
            
            bool result;
            if (process_mode == '1') {
                result = regex.validUrl(input);
                if(result) {
                    regex.addStatistic(input);
                    std::cout << "Корректный URL\n";
                } else {
                    std::cout << "Некорректный URL\n";
                }
            }
            else if (process_mode == '2') {
                result = lexer.validUrl(input);
                if(result) {
                    lexer.addStatistic(input);
                    std::cout << "Корректный URL\n";
                } else {
                    std::cout << "Некорректный URL\n";
                }
            }
            else if (process_mode == '3') {
                result = smc.validUrl(input);
                if(result) {
                    smc.addStatistic(input);
                    std::cout << "Корректный URL\n";
                } else {
                    std::cout << "Некорректный URL\n";
                }
            }
        }
        file.close();
    }
    else if (input_mode == '1') {
        std::cout << "Введите URL (для завершения введите *):\n";
        
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, input);
            
            if (input == "*") {
                std::cout << "Завершение ввода\n";
                break;
            }
            
            if (input.empty())
                continue;
            
            bool result;
            if (process_mode == '1') {
                result = regex.validUrl(input);
                if(result) {
                    regex.addStatistic(input);
                    std::cout << "Корректный URL\n";
                } else {
                    std::cout << "Некорректный URL\n";
                }
            }
            else if (process_mode == '2') {
                result = lexer.validUrl(input);
                if(result) {
                    lexer.addStatistic(input);
                    std::cout << "Корректный URL\n";
                } else {
                    std::cout << "Некорректный URL\n";
                }
            }
            else if (process_mode == '3') {
                result = smc.validUrl(input);
                if(result) {
                    smc.addStatistic(input);
                    std::cout << "Корректный URL\n";
                } else {
                    std::cout << "Некорректный URL\n";
                }
            }
        }
    }
 
    std::cout << "\n===== СТАТИСТИКА =====\n";
    if (process_mode == '1') {
        std::cout << "Режим: Regex парсер\n";
        regex.printStatistic(std::cout);
    }
    else if (process_mode == '2') {
        std::cout << "Режим: Lexer парсер\n";
        lexer.printStatistic(std::cout);
    }
    else if (process_mode == '3') {
        std::cout << "Режим: SMC парсер\n";
        smc.printStatistic(std::cout);
    }
    return 0;
}
