cmake_minimum_required(VERSION 3.2)
project(Bugger)

set(CMAKE_CXX_STANDARD 17 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network Concurrent)
find_package(libssh2 REQUIRED)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(test)
add_subdirectory(src)