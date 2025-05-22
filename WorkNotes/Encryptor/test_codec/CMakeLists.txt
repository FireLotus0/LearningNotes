cmake_minimum_required(VERSION 3.2)

project(test_codec)
set(CMAKE_AUTOMOC ON)
if(Unix)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /projects/EncryptTool/testCodec)
endif ()

add_executable(${PROJECT_NAME}
    main.cpp
    encry_aes.h
    encry_aes.cpp

    qtaes/qaesencryption.h
    qtaes/qaesencryption.cpp
    qtaes/aesni/aesni-enc-cbc.h
    qtaes/aesni/aesni-enc-ecb.h
    qtaes/aesni/aesni-key-exp.h
    qtaes/aesni/aesni-key-init.h
)

find_package(EncryptorTool REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME}
    protocol_codec_engine
    Qt5::Core
    Qt5::Network
    EncryptorTool::EncryptorTool
)