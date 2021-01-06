cmake_minimum_required(VERSION 3.17)
project(Calculator)

set(CMAKE_CXX_STANDARD 14)

add_executable(Calculator   main.cpp
        grammar.h           grammar.cpp
        calculator.h        calculator.cpp
        error_functions.h   error_functions.cpp
        text_colors.h                           )