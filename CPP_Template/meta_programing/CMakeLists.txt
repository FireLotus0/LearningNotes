cmake_minimum_required(VERSION 3.20)

project(meta_test)

set(SRC is_prime.cc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=20000")

add_executable(${PROJECT_NAME} ${SRC})
