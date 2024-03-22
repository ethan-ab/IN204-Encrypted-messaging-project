cmake_minimum_required(VERSION 3.28)
project(SodiumDebug)

set(CMAKE_CXX_STANDARD 17)

add_executable(SodiumDebug main.cpp
        Utility.h)

target_link_libraries(SodiumDebug sodium)
