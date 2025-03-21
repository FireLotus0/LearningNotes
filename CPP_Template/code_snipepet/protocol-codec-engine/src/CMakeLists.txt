cmake_minimum_required(VERSION 3.12)

project(protocol_codec_engine VERSION 0.0.7)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

if (WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else ()
    set(CMAKE_CXX_FLAGS -Wno-attributes)
endif()

set(SRC_HEADERS
    ../include/protocolcodecengine.h
    ../include/protocolcodecinterface.h

    ../include/utils/protocoltemplatedecoder.h
    ../include/utils/protocolflagdata.h

    ../include/flagdata/dataheader.h
    ../include/flagdata/datasize.h
    ../include/flagdata/datacontent.h
    ../include/flagdata/dataverify.h
    ../include/flagdata/dataend.h

    ../include/decoder/protocoldecoder.h

    ../include/encoder/protocolencoder.h
)

set(SRC_SOURCE
    protocolcodecengine.cpp
    protocolcodecinterface.cpp

    utils/protocoltemplatedecoder.cpp
    utils/protocolflagdata.cpp

    flagdata/dataheader.cpp
    flagdata/datasize.cpp
    flagdata/datacontent.cpp
    flagdata/dataverify.cpp
    flagdata/dataend.cpp

    decoder/protocoldecoder.cpp

    encoder/protocolencoder.cpp
)

set(CMAKE_DEBUG_POSTFIX "d")

add_library(${PROJECT_NAME}
    ${SRC_HEADERS}
    ${SRC_SOURCE}
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT ProtocolCodecEngineTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/ProtocolCodecEngineConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ProtocolCodecEngineConfig.cmake"
    INSTALL_DESTINATION lib/cmake/ProtocolCodecEngine
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ProtocolCodecEngineConfig.cmake"
    DESTINATION lib/cmake/ProtocolCodecEngine
)

install(EXPORT ProtocolCodecEngineTargets
    FILE ProtocolCodecEngineTargets.cmake
    DESTINATION lib/cmake/ProtocolCodecEngine
)