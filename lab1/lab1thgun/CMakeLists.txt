cmake_minimum_required(VERSION 3.10)
project(URLPar)

set(CMAKE_CXX_STANDART 23)

add_executable(add
    main.cpp
    lex/Flex_scan.cpp
    lex/lex.yy.cc
    smc/URLPars.cpp
    smc/URLPars_sm.cpp
)
