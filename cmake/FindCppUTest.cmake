
#rules for finding CppUTest
#based on http://www.vtk.org/Wiki/CMake:How_To_Find_Libraries

find_path(CPPUTEST_INCLUDE_DIR CppUTest/TestHarness.h)

set(CMAKE_LIBRARY_ARCHITECTURE "x86_64-linux-gnu")  #TODO: this is the only (?) way to find cpputest on debian
find_library(CPPUTEST_LIBRARY NAMES libCppUTest.a)
find_library(CPPUTEST_EXT_LIBRARY NAMES libCppUTestExt.a)

set(CPPUTEST_LIBRARIES ${CPPUTEST_LIBRARY} )
set(CPPUTEST_EXT_LIBRARIES ${CPPUTEST_EXT_LIBRARY} )
set(CPPUTEST_INCLUDE_DIRS ${CPPUTEST_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CppUTest DEFAULT_MSG CPPUTEST_LIBRARY CPPUTEST_INCLUDE_DIR)

mark_as_advanced(CPPUTEST_INCLUDE_DIR CPPUTEST_LIBRARY)
