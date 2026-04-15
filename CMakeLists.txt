cmake_minimum_required(VERSION 3.26)
project(imgsteg)

set(CMAKE_CXX_STANDARD 20)

add_executable(imgsteg
        src/main.cpp
        src/utils.cpp
        src/file_io.cpp
        src/functions.cpp
)

target_include_directories(imgsteg PRIVATE include)