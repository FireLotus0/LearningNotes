cmake_minimum_required(VERSION 3.2)

project(usb_test)

include(${CMAKE_SOURCE_DIR}/src/usb/usb.cmake)
message(STATUS "${CMAKE_SOURCE_DIR}/src/usb/usb.cmake/usb/usb.cmake")

include_directories(${CMAKE_SOURCE_DIR}/src/usb)

find_package(Qt5 COMPONENTS Core REQUIRED)

add_executable(${PROJECT_NAME} main.cpp ${USB_SRC})
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    ${USB_LIB}
)