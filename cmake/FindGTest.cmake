


# - Try to find GTest
# Once done this will define
#  GTEST_FOUND - System has GTest
#  GTEST_INCLUDE_DIRS - The GTest include directories
#  GTEST_LIBRARY - GTest library
#  GTEST_MAIN_LIBRARY - GTest main library

find_path(GTEST_INCLUDE_DIR gtest/gtest.h
          HINTS ${GTEST_DIR}/include)
                    
find_library(GTEST_LIBRARY      libgtest.a)
find_library(GTEST_MAIN_LIBRARY libgtest_main.a)

set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GTEST_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GTest DEFAULT_MSG
                                  GTEST_INCLUDE_DIR )

mark_as_advanced(GTEST_INCLUDE_DIR)
