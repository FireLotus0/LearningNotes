cmake_minimum_required(VERSION 3.12)

project(CollectLower VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)
find_package(ProtocolCodecEngine REQUIRED)
link_directories(${LIB_ROOT_PATH}/ext)
find_package(Bugreport REQUIRED)
find_package(QLogCollectorServer REQUIRED)


string(CONCAT ACCESS_KEY_STR "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJBdXRoZW50aWNhdGlvbiIsImlzcyI6Ind3dy5l"
    "YW50dGVjaC5jb20iLCJuYW1lIjoiQ29sbGVjdExvd2VyIn0.cVIV9EkS4qipnnw_oUHBOyMKA4hhzvASak7QejsvIws"
)
add_compile_definitions(ACCESS_KEY="${ACCESS_KEY_STR}")
#此处宏定义用于检查程序和查找应用版本号
add_compile_definitions(APP_VERSION_DEF="APP_VERSION:v${PROJECT_VERSION}")
add_compile_definitions(ACCESS_KEY_DEF="ACCESS_KEY:${ACCESS_KEY_STR}")

#添加宏定义
add_compile_definitions(CUR_PROJECT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
add_compile_definitions(APP_VERSION="v${PROJECT_VERSION}")

#查找usb
include(${CMAKE_SOURCE_DIR}/src/usb/usb.cmake)
message(STATUS "${CMAKE_SOURCE_DIR}/src/usb/usb.cmake/usb/usb.cmake")
include_directories(${CMAKE_SOURCE_DIR}/src/usb)

add_executable(${PROJECT_NAME}
    main.cpp

    utils/threadmanage.h
    utils/threadmanage.cpp
    utils/transfermanager.h
    utils/transfermanager.cpp
    utils/configadc.h
    utils/configadc.cpp

    ${USB_SRC}
    ${CODE_ENGINE_SRC}
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Network
    bugreport
    protocol_codec_engine
    qlogcollectorserver
    ${USB_LIB}

)


include(${CMAKE_SOURCE_DIR}/modules/publish_release.cmake)
PublishRelease(${PROJECT_NAME} 0)