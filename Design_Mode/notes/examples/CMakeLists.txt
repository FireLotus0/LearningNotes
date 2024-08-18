cmake_minimum_required(VERSION 3.20)

project(single_test)

set(THREADS_PREFER_PTHREAD on)
find_package(Threads REQUIRED)

set(SRC singleton.cc)

add_executable(single_test ${SRC})

target_link_libraries(single_test PUBLIC  Threads::Threads)
