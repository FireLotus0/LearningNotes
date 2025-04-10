project(Test)

include_directories(${CMAKE_SOURCE_DIR}/src/utils/task/)


set(SRC main.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/task/macros.h
    ${CMAKE_SOURCE_DIR}/src/utils/task/session.h
    ${CMAKE_SOURCE_DIR}/src/utils/task/session.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/task/utils.h
    ${CMAKE_SOURCE_DIR}/src/utils/task/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/task/taskentity.h
    ${CMAKE_SOURCE_DIR}/src/utils/task/taskentity.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/task/taskexecutor.h
    ${CMAKE_SOURCE_DIR}/src/utils/task/taskexecutor.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/task/channelsession.h
    ${CMAKE_SOURCE_DIR}/src/utils/task/channelsession.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/task/scpsession.h
    ${CMAKE_SOURCE_DIR}/src/utils/task/scpsession.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    libssh2::libssh2_shared
    ws2_32
)