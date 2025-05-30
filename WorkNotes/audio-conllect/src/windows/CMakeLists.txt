cmake_minimum_required(VERSION 3.12)

project(acquisition_HMI  VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Network REQUIRED)
find_package(QLogCollectorServer REQUIRED)
find_package(QTaskMachines REQUIRED)
find_package(Bugreport REQUIRED)
find_package(ProtocolCodecEngine REQUIRED)

#此处添加翻译脚本
#find_package(Qt5 COMPONENTS LinguistTools)
#qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} app_zh.ts app_en.ts)
#message("application translation files: ${QM_FILES}")
#configure_file(app_translate.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

#添加宏定义
add_compile_definitions(CUR_PROJECT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
add_compile_definitions(APP_VERSION="v${PROJECT_VERSION}")

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
  #配置资源和应用信息配置文件
  set(RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc.in ${RC_FILE} @ONLY)
  #生成安装程序脚本
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/res/install.nsi.in ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}-install.nsi @ONLY)
endif()

#查找usb
include(${CMAKE_SOURCE_DIR}/src/usb/usb.cmake)
message(STATUS "${CMAKE_SOURCE_DIR}/src/usb/usb.cmake/usb/usb.cmake")
include_directories(${CMAKE_SOURCE_DIR}/src/usb)

add_executable(${PROJECT_NAME}
    main.cpp

    app.h
    app.cpp
    app.ui
    app.qrc

    dialog/connecttoserverdlg.cpp
    dialog/connecttoserverdlg.h

    ${USB_SRC}

    ${RC_FILE}
    ${CODE_ENGINE_SRC}

    utils/appsettings.h
    utils/appsettings.cpp

    ${SINGLE_INSTANCE_SRC}
    ${VIEW_DEBUG_SRC}

    #翻译资源文件
    #${QM_FILES}
    #${CMAKE_CURRENT_BINARY_DIR}/app_translate.qrc
    )

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network

    qlogcollectorserver
    qtask_machines
    bugreport
    #添加协议编解码工具
    protocol_codec_engine
    ${USB_LIB}
)

if (WIN32 AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  target_link_libraries(${PROJECT_NAME} Qt5::UiTools)
endif()

include(${CMAKE_SOURCE_DIR}/modules/publish_release.cmake)
PublishRelease(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/res/logo.ico)