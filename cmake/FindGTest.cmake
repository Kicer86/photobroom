
# - Try to find GTest
# Once done this will define
#  GTEST_FOUND - System has GTest
#  GTEST_INCLUDE_DIRS - The GTest include directories
#  GTEST_SOURCES - Extra sources to be added to build

find_path(GTEST_INCLUDE_DIR gtest/gtest.h
          HINTS ${GTEST_DIR}/include)
          
find_path(GTEST_INTERNALS src/gtest-all.cc
          HINTS ${GTEST_INCLUDE_DIR}/.. 
                ${GTEST_DIR}
                /usr/src/gtest                      #debian
         )

find_file(GTEST_SOURCE  src/gtest-all.cc
          HINTS ${GTEST_INTERNALS})
          
set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR} ${GTEST_INTERNALS})
set(GTEST_SOURCES      ${GTEST_SOURCE} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GTEST_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GTest  DEFAULT_MSG
                                  GTEST_SOURCE GTEST_INCLUDE_DIR GTEST_INTERNALS)

mark_as_advanced(GTEST_SOURCE GTEST_INCLUDE_DIR GTEST_INTERNALS)
