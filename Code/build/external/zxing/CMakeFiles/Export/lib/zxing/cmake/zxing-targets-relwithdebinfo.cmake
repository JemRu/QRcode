#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "zxing::zxing" for configuration "RelWithDebInfo"
set_property(TARGET zxing::zxing APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(zxing::zxing PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/zxing.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS zxing::zxing )
list(APPEND _IMPORT_CHECK_FILES_FOR_zxing::zxing "${_IMPORT_PREFIX}/bin/zxing.exe" )

# Import target "zxing::libzxing" for configuration "RelWithDebInfo"
set_property(TARGET zxing::libzxing APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(zxing::libzxing PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libzxing.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS zxing::libzxing )
list(APPEND _IMPORT_CHECK_FILES_FOR_zxing::libzxing "${_IMPORT_PREFIX}/lib/libzxing.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
