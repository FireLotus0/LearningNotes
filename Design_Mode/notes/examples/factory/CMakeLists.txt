cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)

project(factory_test)


set(SRC 
#	simple_factory.cc
#	factory.cc
	abstract_factory.cc
)

add_executable(${PROJECT_NAME} ${SRC})
