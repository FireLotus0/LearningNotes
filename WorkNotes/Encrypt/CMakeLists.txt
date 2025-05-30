cmake_minimum_required(VERSION 3.5)

project(usb_wrap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# 系统平台定义
if (WIN32)
    set(OS_NAME OS_WINDOWS)
else()
    set(OS_NAME OS_UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /projects/EncryptTool/)
endif ()

if (MSVC)
    # 设置utf-8编码
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(USB_NAME "libusb-1.0")
else()
    set(USB_NAME "usb-1.0")
endif ()

find_package(Qt5 COMPONENTS Core Network)
add_definitions(-DWIN32_LEAN_AND_MEAN)
add_definitions(-D_WINSOCKAPI_)
# 查找libusb
find_library(USB_LIBRARY ${USB_NAME})
if (NOT USB_LIBRARY)
    message(FATAL_ERROR "USB Library Not Found! ${USB_NAME}")
endif ()
# 查找线程库
find_package(Threads REQUIRED)
# 系统平台宏定义
add_definitions(-D${OS_NAME})

set(SRC
    main.cpp
    utils/singleinstance/singleinstance.h

    utils/bytearray/bytearray.h
    utils/bytearray/bytearray.cpp
    utils/codec/framecodec.h
    utils/codec/framecodec.cpp

    utils/codec/crc.h
    utils/codec/crc.cpp
    encryptdog/encryptdog.h
    encryptdog/encryptdog.cpp
    utils/logger/logger.h
    utils/json/json.h

    utils/usbmanager/libusb.h
    utils/usbmanager/usbmanager.h
    utils/usbmanager/usbmanager.cpp

    utils/hostinfo/hostinfo.h
    utils/hostinfo/hostinfo.cpp

    utils/encrypt/encryptor.h
    utils/encrypt/encryptor.cpp
    utils/encrypt/qtaes/qaesencryption.h
    utils/encrypt/qtaes/qaesencryption.cpp
    utils/encrypt/qtaes/aesni/aesni-enc-cbc.h
    utils/encrypt/qtaes/aesni/aesni-enc-ecb.h
    utils/encrypt/qtaes/aesni/aesni-key-exp.h
    utils/encrypt/qtaes/aesni/aesni-key-init.h

    http/httplib.h
    http/httpmanager.h
    http/httpmanager.cpp

    enctool/enctool.h
    enctool/enctool.cpp
)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME}
    ${USB_LIBRARY}
    Qt5::Core
    Qt5::Network
    Threads::Threads
)
