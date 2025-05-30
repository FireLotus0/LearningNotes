cmake_minimum_required(VERSION 3.12)

project(acoustic_data_acquisition)

#启用cmake查找路径调试
#set(CMAKE_FIND_DEBUG_MODE TRUE)

if (WIN32)
  #编译使用utf8文件
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
  if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    # 为Release添加调试信息
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
  endif ()
endif()


add_compile_definitions(ROOT_PROJECT_PATH="${CMAKE_SOURCE_DIR}")

############################# 3rdparty defines #############################

# dev-tools
if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  include(${CMAKE_SOURCE_DIR}/cpm/CPM_0.40.2.cmake)
  CPMAddPackage(
      NAME Dev-Tools
      GIT_REPOSITORY git@192.168.1.159:qt-dev/dev-tools.git
      GIT_TAG v1.0.5
      DOWNLOAD_ONLY YES
      SOURCE_DIR "${CMAKE_SOURCE_DIR}/dev-tools")
endif()

############################# modules defines #############################

# 添加modules头文件路径
include_directories(${CMAKE_SOURCE_DIR}/modules)

# 添加单例应用程序源文件
include(${CMAKE_SOURCE_DIR}/modules/singleinstance/src.cmake)
include(${CMAKE_SOURCE_DIR}/modules/codecengine/src.cmake)

# 界面调试
if (WIN32 AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  find_package(Qt5 COMPONENTS UiTools)
  file(GLOB VIEW_DEBUG_SRC
      ${CMAKE_SOURCE_DIR}/modules/viewdebug/*.cpp
  )
endif ()

############################# projects defines #############################

if(MSVC)
  add_subdirectory(src/windows)
else()
  add_subdirectory(src/linux)
#  add_subdirectory(test/usbtest)
endif()


