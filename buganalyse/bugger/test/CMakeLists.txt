project(Test)

include_directories(${CMAKE_SOURCE_DIR}/src/utils/)


set(SRC main.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/macros.h
    ${CMAKE_SOURCE_DIR}/src/utils/utils.h
    ${CMAKE_SOURCE_DIR}/src/utils/utils.cpp

    ${CMAKE_SOURCE_DIR}/src/utils/task/taskentity.h
    ${CMAKE_SOURCE_DIR}/src/utils/task/taskentity.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/task/taskexecutor.h
    ${CMAKE_SOURCE_DIR}/src/utils/task/taskexecutor.cpp

    ${CMAKE_SOURCE_DIR}/src/utils/session/session.h
    ${CMAKE_SOURCE_DIR}/src/utils/session/session.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/session/channelsession.h
    ${CMAKE_SOURCE_DIR}/src/utils/session/channelsession.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/session/scpsession.h
    ${CMAKE_SOURCE_DIR}/src/utils/session/scpsession.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    libssh2::libssh2_shared
    ws2_32
)