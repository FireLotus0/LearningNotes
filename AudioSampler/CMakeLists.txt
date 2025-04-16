cmake_minimum_required(VERSION 3.12)

project(AudioSampler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Threads)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/bean/datadef.h

        src/logger/logger.h
        src/logger/logger.cpp

        src/datacache/datacache.h
        src/datasender/datasender.h
        src/datasender/datasender.cpp
        src/datareader/datareader.h
        src/datareader/datareader.cpp
        src/utils/utils.h
        src/utils/utils.cpp
)

target_link_libraries(${PROJECT_NAME} Threads::Threads)