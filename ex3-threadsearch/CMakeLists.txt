cmake_minimum_required(VERSION 3.9)
project(ex3_threadsearch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)
find_package(Threads)

add_executable(ex3_threadsearch main.cpp StringSearcher.cpp StringSearcher.h FileSearcherException.h)
