cmake_minimum_required(VERSION 3.14)
project(lab5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)

list(APPEND LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Charts)

list(REMOVE_DUPLICATES LIBRARIES)

add_executable(lab5 main.cpp InputPath.cpp InputPath.h WholeDiskUsage.cpp WholeDiskUsage.h InputPathDiskUsage.cpp InputPathDiskUsage.h Model.cpp Model.h PathValidator.cpp PathValidator.h)
target_link_libraries(lab5 ${LIBRARIES})