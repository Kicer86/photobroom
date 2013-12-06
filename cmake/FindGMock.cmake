
# useful: http://www.rad.upenn.edu/sbia/software/basis/apidoc/latest/FindGMock_8cmake_source.html

# - Try to find GMock
# Once done this will define
#  GMOCK_FOUND        - System has GMock
#  GMOCK_INCLUDE_DIRS - The GMock include directories
#  GMOCK_LIBRARY      - GMock library
#  GMOCK_MAIN_LIBRARY - GMock main library

find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
          HINTS ${GMOCK_DIR}/include)
          
find_library(GMOCK_LIBRARY      libgmock.a)
find_library(GMOCK_MAIN_LIBRARY libgmock_main.a)

set(GMOCK_INCLUDE_DIRS ${GMOCK_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GMOCK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GTest DEFAULT_MSG
                                  GMOCK_INCLUDE_DIR)

mark_as_advanced(GMOCK_INCLUDE_DIR)
