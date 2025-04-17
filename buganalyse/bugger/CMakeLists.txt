cmake_minimum_required(VERSION 3.2)
project(Bugger)

set(CMAKE_CXX_STANDARD 17 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#if (WIN32)
#    #编译使用utf8文件
#    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
#    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
#    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
#        # 为Release添加调试信息
#        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
#        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
#    endif ()
#endif()

find_package(Qt5 COMPONENTS Core Gui Widgets Network Concurrent)
find_package(libssh2 REQUIRED)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(test)
add_subdirectory(src)