# Copyright (C) The libssh2 project and its contributors.
# SPDX-License-Identifier: BSD-3-Clause

option(LIBSSH2_USE_PKGCONFIG "Enable pkg-config to detect libssh2 dependencies. Default: OFF" "OFF")

include(CMakeFindDependencyMacro)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

set(_lib "")
if("WinCNG" STREQUAL "OpenSSL")
  find_dependency(OpenSSL)
elseif("WinCNG" STREQUAL "wolfSSL")
  find_dependency(WolfSSL)
  set(_lib libssh2::wolfssl)
elseif("WinCNG" STREQUAL "Libgcrypt")
  find_dependency(Libgcrypt)
  set(_lib libssh2::libgcrypt)
elseif("WinCNG" STREQUAL "mbedTLS")
  find_dependency(MbedTLS)
  set(_lib libssh2::mbedcrypto)
endif()

if()
  find_dependency(ZLIB)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/libssh2-targets.cmake")

# Alias for either shared or static library
if(NOT TARGET libssh2::libssh2)
  add_library(libssh2::libssh2 ALIAS libssh2::libssh2_shared)
endif()

# Compatibility alias
if(NOT TARGET Libssh2::libssh2)
  add_library(Libssh2::libssh2 ALIAS libssh2::libssh2_shared)
endif()

cmake_policy(GET CMP0099 _has_CMP0099)  # https://cmake.org/cmake/help/latest/policy/CMP0099.html
if(NOT _has_CMP0099 AND _lib)
  message(STATUS "libssh2: CMP0099 not detected, resorting to workaround.")
  # CMake before CMP0099 (CMake 3.17 2020-03-20) did not endorse the concept of libdirs and lib names.
  # It expected libs to have an absolute filename. As a workaround, manually apply dependency libdirs
  # to the libssh2 target, for CMake consumers without this policy set.
  get_target_property(_libdirs "${_lib}" INTERFACE_LINK_DIRECTORIES)
  if(_libdirs)
    foreach(_target IN ITEMS libssh2::libssh2_shared libssh2::libssh2_static)
      if(TARGET "${_target}")
        target_link_directories("${_target}" INTERFACE ${_libdirs})
      endif()
    endforeach()
  endif()
endif()
