cmake_minimum_required(VERSION 3.20)

project(builder_test)

set(SRC builder.cc)

add_executable(${PROJECT_NAME} ${SRC})

