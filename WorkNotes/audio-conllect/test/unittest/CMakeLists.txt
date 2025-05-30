cmake_minimum_required(VERSION 3.12)

project(unit-test)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Test REQUIRED)
find_package(QLogCollectorServer REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp

    tester/unittest.h
    tester/unittest.cpp
    )

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Test

    qlogcollectorserver
    )