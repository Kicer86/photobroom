
# useful: http://www.rad.upenn.edu/sbia/software/basis/apidoc/latest/FindGMock_8cmake_source.html

# - Try to find GMock
# Once done this will define
#  GMOCK_FOUND - System has GMock
#  GMOCK_INCLUDE_DIRS - The GMock include directories
#  GMOCK_SOURCES - Extra sources to be added to build


find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
          HINTS ${GMOCK_DIR}/include)
          
find_path(GMOCK_INTERNALS src/gmock-all.cc
          HINTS ${GMOCK_INCLUDE_DIR}/.. 
                ${GMOCK_DIR}
                /usr/src/gmock                 #debian
         )

find_file(GMOCK_SOURCE  src/gmock-all.cc
          HINTS ${GMOCK_INTERNALS})

set(GMOCK_INCLUDE_DIRS ${GMOCK_INCLUDE_DIR} )
set(GMOCK_SOURCES      ${GMOCK_SOURCE} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GMOCK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GTest  DEFAULT_MSG
                                  GMOCK_SOURCE GMOCK_INCLUDE_DIR GMOCK_INTERNALS)

mark_as_advanced(GMOCK_SOURCE GMOCK_INCLUDE_DIR GMOCK_INTERNALS)
