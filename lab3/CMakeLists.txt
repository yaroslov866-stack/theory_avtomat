cmake_minimum_required(VERSION 3.10)
project(Interpretator)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SFML_DIR "/usr/lib/x86_64-linux-gnu/cmake/SFML")
find_package(SFML REQUIRED COMPONENTS graphics window system)
add_executable(app
    main.cpp
    lex/lex.yy.cc
    bison/bison.tab.cpp   
)
target_link_libraries(app 
    sfml-system 
    sfml-graphics 
    sfml-window
)