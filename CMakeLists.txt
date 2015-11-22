cmake_minimum_required(VERSION 2.8)
project(aghsm)

set(SOURCES
    Lexer.h
    Lexer.cpp
    Parser.h
    Parser.cpp
    CodeEmitter.h
    CodeEmitter.cpp
    Assembler.h
    Assembler.cpp
    main.cpp
    VM.h
    VM.cpp Language.cpp Language.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(${PROJECT_NAME} ${SOURCES})

