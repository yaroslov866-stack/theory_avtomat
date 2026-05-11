#include "bison/bison.tab.hpp"
#include <cstdio>
#include <fstream>
#include "symbol/sym.h"
#include "interpretator/inter.h"
extern void set_input_stream(std::istream* is);
int main(int argc, char** argv){
    printf("\n=== ИНТЕРПРЕТАТОР РОБОТА ===\n\n");
    if(argc < 2){
        printf("Error: need file name\n");
        return 1;
    }
    
    std::ifstream file(argv[1]);
    if(!file.is_open()){
        printf("Error open file: %s\n", argv[1]);
        return 1;
    }
    
    printf("File: %s\n", argv[1]);
    printf("----------------------------\n");
    set_input_stream(&file);
    yy::parser parser;
    int result = parser.parse();
    
    printf("----------------------------\n");
    if(result == 0){
        printf("SUCCESS\n");
    } else {
        printf("FAIL\n");
    }
    extern Interpreter* interpreter;
    delete interpreter;
    file.close();
    return result;
}