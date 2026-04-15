cmake_minimum_required(VERSION 3.26)
project(TestEnvironment)

set(CMAKE_CXX_STANDARD 20)

add_executable(TestEnvironment
        src/main.cpp
        src/utils.cpp
        include/bmp_header.h
        include/ppm_header.h
        include/utils.h
        include/file_io.h
        include/functions.h
        src/file_io.cpp
        src/functions.cpp
)
