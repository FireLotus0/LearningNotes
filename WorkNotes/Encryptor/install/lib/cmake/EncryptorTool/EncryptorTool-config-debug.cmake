#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "EncryptorTool::EncryptorTool" for configuration "Debug"
set_property(TARGET EncryptorTool::EncryptorTool APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(EncryptorTool::EncryptorTool PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/EncryptorTool.lib"
  )

list(APPEND _cmake_import_check_targets EncryptorTool::EncryptorTool )
list(APPEND _cmake_import_check_files_for_EncryptorTool::EncryptorTool "${_IMPORT_PREFIX}/lib/EncryptorTool.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
