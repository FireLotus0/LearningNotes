set(CODE_ENGINE_SRC
    ${CMAKE_CURRENT_LIST_DIR}/customcodeengine.h
    ${CMAKE_CURRENT_LIST_DIR}/customcodeengine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bean/hostdata.h
    ${CMAKE_CURRENT_LIST_DIR}/bean/fpgacontroldata.h
    ${CMAKE_CURRENT_LIST_DIR}/bean/sensordata.h
)

if (NOT MSVC)
list(APPEND CODE_ENGINE_SRC
    ${CMAKE_CURRENT_LIST_DIR}/bean/datareader.h
    ${CMAKE_CURRENT_LIST_DIR}/bean/datareader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bean/fpgadatareader.h
    ${CMAKE_CURRENT_LIST_DIR}/bean/fpgadatareader.cpp)
endif ()