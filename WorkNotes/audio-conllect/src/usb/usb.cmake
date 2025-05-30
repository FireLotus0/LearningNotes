# 查找libusb
if (WIN32)
    set(USB_NAME libusb-1.0)
else ()
    set(USB_NAME usb-1.0)
endif ()
find_library(USB_LIB ${USB_NAME})
if(NOT USB_LIB)
    message(FATAL_ERROR "libusb not found!")
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(USB_SRC
    ${CMAKE_CURRENT_LIST_DIR}/iohelper.h
    ${CMAKE_CURRENT_LIST_DIR}/iohelper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/iomanager.h
    ${CMAKE_CURRENT_LIST_DIR}/iomanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libusb.h
    ${CMAKE_CURRENT_LIST_DIR}/usbdevice.h
    ${CMAKE_CURRENT_LIST_DIR}/usbdevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/usbinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/usbinfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/usblistener.h
    ${CMAKE_CURRENT_LIST_DIR}/usblistener.cpp
    ${CMAKE_CURRENT_LIST_DIR}/usbmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/usbmanager.cpp
)