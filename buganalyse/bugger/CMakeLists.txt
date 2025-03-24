cmake_minimum_required(VERSION 3.2)
project(Bugger)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network)
find_package(libssh2 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})
set(SRC
    src/main.cpp
    src/app/app.h
    src/app/app.cpp
    src/app.qrc

    src/widgets/custombtngroup.h
    src/widgets/custombtngroup.cpp

    src/utils/frame/windowframelesshelper.h
    src/utils/frame/windowframelesshelper.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    libssh2::libssh2_shared
    ws2_32
)