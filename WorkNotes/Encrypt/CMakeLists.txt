cmake_minimum_required(VERSION 3.5)

project(usb_wrap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# 系统平台定义
if (WIN32)
    set(OS_NAME OS_WINDOWS)
elseif (UNIX)
    set(OS_NAME OS_UNIX)
endif ()

if (MSVC)
    # 设置utf-8编码
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(USB_NAME libusb-1.0)
elseif (Unix)
    set(USB_NAME usb-1.0)
endif ()

find_package(Qt5 COMPONENTS Core)

# 查找libusb
find_library(USB_LIBRARY ${USB_NAME})
if (NOT USB_LIBRARY)
    message(FATAL_ERROR "USB Library Not Found!")
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
    utils/datahandler/datahandler.h
    utils/datahandler/datahandler.cpp
    utils/logger/logger.h
    utils/json/json.h

    usbmanager/libusb.h
    usbmanager/usbmanager.h
    usbmanager/usbmanager.cpp

    utils/encrypt/encryptor.h
    utils/encrypt/encryptor.cpp
    utils/encrypt/qtaes/qaesencryption.h
    utils/encrypt/qtaes/qaesencryption.cpp
    utils/encrypt/qtaes/aesni/aesni-enc-cbc.h
    utils/encrypt/qtaes/aesni/aesni-enc-ecb.h
    utils/encrypt/qtaes/aesni/aesni-key-exp.h
    utils/encrypt/qtaes/aesni/aesni-key-init.h

)
add_executable(test ${SRC})
target_link_libraries(test
    ${USB_LIBRARY}
    Qt5::Core
    Threads::Threads
)
