cmake_minimum_required(VERSION 3.10)

project(datagenerator
        VERSION 1.0.0
        LANGUAGES CXX
        DESCRIPTION "A tool for generating input and output test data for evaluation machines"
)

add_executable(datagenerator
    src/main.cpp
)

set_target_properties(datagenerator PROPERTIES
    CXX_STANDARD 23
)