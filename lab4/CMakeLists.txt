cmake_minimum_required(VERSION 3.9)
project(lab4)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)
find_package(Threads)


add_executable(lab4 main.cpp Scheduler.cpp Scheduler.h Job.cpp Job.h Worker.cpp Worker.h)