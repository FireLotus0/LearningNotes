cmake_minimum_required(VERSION 3.2)
project(Bugger)

set(CMAKE_CXX_STANDARD 17 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network Concurrent)
find_package(libssh2 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})
set(SRC
    src/main.cpp
    src/app/app.h
    src/app/app.cpp
    src/app.qrc

    src/widgets/custombtngroup.h
    src/widgets/custombtngroup.cpp

    src/dialog/info/infodlg.h
    src/dialog/info/infodlg.cpp

    src/utils/frame/windowframelesshelper.h
    src/utils/frame/windowframelesshelper.cpp
    src/utils/ssh/sessionmanager.h
    src/utils/ssh/sessionmanager.cpp
    src/utils/ssh/session.h
    src/utils/ssh/session.cpp
    src/utils/ssh/taskexecutor.h
    src/utils/ssh/taskexecutor.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    Qt5::Concurrent
    libssh2::libssh2_shared
    ws2_32
)