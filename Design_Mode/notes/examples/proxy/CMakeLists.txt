cmake_minimum_required(VERSION 3.20)

project(proxy_test)

set(SRC 
#  proxy.cc
   aop.cc
)

add_executable(${PROJECT_NAME} ${SRC})
