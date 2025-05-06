cmake_minimum_required(VERSION 3.30)
project(WorldCities)

set(CMAKE_CXX_STANDARD 20)

add_executable(WorldCities main.cpp
        trie.cpp)
