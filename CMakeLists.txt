cmake_minimum_required(VERSION 3.26)
project(TestEnvironment)

set(CMAKE_CXX_STANDARD 20)

add_executable(TestEnvironment
        BMPHeaderStruct.h
        MainFunctions.h
        TextDecorations.h
        main.cpp
)
