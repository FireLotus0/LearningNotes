cmake_minimum_required(VERSION 3.2)

project(EncryptorTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

option(GEN_STATIC_LIB "生成静态库")
if(GEN_STATIC_LIB AND WIN32)
endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_definitions(-DOS_WIN)
    set(BUILD_SHARED_LIBS ON)
    set(CMAKE_INSTALL_PREFIX D:/thirdparty/packages/qt5.15.2/EncryptorTool)
    set(USB_NAME "libusb-1.0")
    set(USB_INCLUDE_FILE src/utils/usbmanager/libusb.h)
    set(QT_PATH_HINTS )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions(-DOS_LINUX)
    set(BUILD_SHARED_LIBS ON)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /projects/EncryptTool)
    set(CMAKE_INSTALL_PREFIX /cross-compiler/EncryptorTool)
    set(USB_NAME "usb-1.0")
    set(USB_INCLUDE_FILE )
    set(QT_PATH_HINTS /cross-compiler/5.15.2/gcc_64)
endif ()

add_definitions(-DDEV_TEST)
find_package(Qt5  COMPONENTS Core Network REQUIRED NO_DEFAULT_PATH)
find_package(ProtocolCodecEngine REQUIRED)


find_library(LIB_USB ${USB_NAME})
if (NOT LIB_USB)
    message(FATAL_ERROR "Can`t find usb lib!")
else ()
    message(STATUS "libusb path:${LIB_USB}")
endif ()


set(LIB_SRC
    include/encryptor.h
    include/encryptor.cpp
    src/utils/usbmanager/iohelper.h
    src/utils/usbmanager/usbmanager.h
    src/utils/usbmanager/usbmanager.cpp
    src/utils/usbmanager/usbdevice.h
    src/utils/usbmanager/usbdevice.cpp
    src/utils/usbmanager/usblistener.h
    src/utils/usbmanager/usblistener.cpp
    src/utils/usbmanager/iohelper.h
    src/utils/usbmanager/iohelper.cpp
    src/utils/usbmanager/usbinfo.h
    src/utils/usbmanager/usbinfo.cpp
    src/utils/usbmanager/iomanager.h
    src/utils/usbmanager/iomanager.cpp

    src/utils/aes/encry_aes.h
    src/utils/aes/encry_aes.cpp

    src/utils/keymanager/keymanager.h
    src/utils/keymanager/keymanager.cpp

    src/utils/hostinfo/hostinfo.h
    src/utils/hostinfo/hostinfo.cpp

    src/utils/logger/logger.h
    src/utils/logger/utils.h
    src/utils/logger/utils.cpp

    src/bean/commomtype.h
    src/qtaes/qaesencryption.h
    src/qtaes/qaesencryption.cpp
    src/qtaes/aesni/aesni-enc-cbc.h
    src/qtaes/aesni/aesni-enc-ecb.h
    src/qtaes/aesni/aesni-key-exp.h
    src/qtaes/aesni/aesni-key-init.h

    ${USB_INCLUDE_FILE}
)

if(GEN_STATIC_LIB AND WIN32)
    add_library(${PROJECT_NAME} STATIC ${LIB_SRC})
else ()
    add_library(${PROJECT_NAME} SHARED ${LIB_SRC})
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${LIB_USB}
    protocol_codec_engine
    Qt5::Core
    Qt5::Network
)
target_compile_definitions(${PROJECT_NAME} PRIVATE LOG_LEVEL_VAL=0)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "include/encryptor.h")
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    PUBLIC_HEADER DESTINATION include
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION lib
    LIBRARY DESTINATION bin
)
install(EXPORT ${PROJECT_NAME}-targets
    NAMESPACE ${PROJECT_NAME}::
    FILE ${PROJECT_NAME}Config.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)


find_package(EncryptorTool REQUIRED CONFIG)
add_executable(test src/test/main.cpp ) #${LIB_SRC}
target_link_libraries(test
#    ${LIB_USB}
    EncryptorTool::EncryptorTool
    protocol_codec_engine
    Qt5::Network
    Qt5::Core
)
target_compile_definitions(test PRIVATE LOG_LEVEL_VAL=0 ) #USE_LIB
#
add_subdirectory(test_codec)