cmake_minimum_required(VERSION 3.9)
project(lab2)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab2 main.cpp Base.cpp Base.h Directory.cpp Directory.h File.cpp File.h FsException.h)