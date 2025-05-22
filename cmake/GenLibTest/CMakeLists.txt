cmake_minimum_required(VERSION  3.2)

project(GenLibTest)

set(LIB_VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DUSE_LIB)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_compile_options("$<$<C_COMPILE_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILE_ID:MSVC>:/utf-8>")
    add_definitions(-DOS_WIN)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DOS_LINUX)
endif ()

set(LIB_INCLUDE test.h)
set(LIB_SRC test.h test.cpp macros.h)

configure_file(GenLibTestConfig.g.in
    ${CMAKE_CURRENT_BINARY_DIR}/GenLibTestConfig.h
)

add_library(${PROJECT_NAME} STATIC ${LIB_SRC})

install(FILE ${LIB_INCLUDE}
    DESTINATION include/test-${LIB_VERSION}
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/${PROJECT_NAME}
)