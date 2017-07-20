
# useful: http://www.rad.upenn.edu/sbia/software/basis/apidoc/latest/FindGMock_8cmake_source.html

# - Try to find GMock
# Once done this will define
#  GMOCK_FOUND        - System has GMock
#  GMOCK_INCLUDE_DIRS - The GMock include directories
#  GMOCK_LIBRARY      - GMock library
#  GMOCK_MAIN_LIBRARY - GMock main library

find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
          HINTS ${GMOCK_DIR}/include)

find_library(GMOCK_LIBRARY      gmock)
find_library(GMOCK_MAIN_LIBRARY gmock_main)

set(GMOCK_INCLUDE_DIRS ${GMOCK_INCLUDE_DIR})

option(GMOCK_FORCE_SOURCES "Prefer googlemock sources over existing libraries" ON)

if(GMOCK_INCLUDE_DIR AND (NOT GMOCK_LIBRARY OR GMOCK_FORCE_SOURCES))

    if(NOT TARGET gmock)

        message("Trying to find GMock sources and register extra targets")
        find_file(GMOCK_BASE_SOURCE gmock-all.cc
                  HINTS /usr/src/gmock
                        /usr/src/gmock/src
                        ${GMOCK_DIR}
                        ${GMOCK_DIR}/src
        )

        find_file(GMOCK_MAIN_SOURCE gmock_main.cc
                  HINTS /usr/src/gmock
                        /usr/src/gmock/src
                        ${GMOCK_DIR}
                        ${GMOCK_DIR}/src
        )

        if(NOT GMOCK_BASE_SOURCE OR NOT GMOCK_MAIN_SOURCE)
            message(FATAL_ERROR "Could not find base for GMock sources. Set GMOCK_DIR to proper value")
        endif(NOT GMOCK_BASE_SOURCE OR NOT GMOCK_MAIN_SOURCE)

        find_package(GTest REQUIRED)

        add_library(gmock STATIC ${GMOCK_BASE_SOURCE})
        add_library(gmock-main STATIC ${GMOCK_MAIN_SOURCE})

        get_filename_component(gmock_base_dir ${GMOCK_BASE_SOURCE} PATH)
        target_include_directories(gmock
                                    PRIVATE
                                        # master branch
                                        ${GTEST_INCLUDE_DIRS}
                                        # v 1.7.0
                                        ${gmock_base_dir}/..
                                        ${gmock_base_dir}/../include
                                        ${gmock_base_dir}/../gtest/include
        )

        target_include_directories(gmock-main
                                    PRIVATE
                                        # master branch
                                        ${GTEST_INCLUDE_DIRS}
                                        # v 1.7.0
                                        ${gmock_base_dir}/..
                                        ${gmock_base_dir}/../include
                                        ${gmock_base_dir}/../gtest/include
        )

        #there should be a CMakeLists.txt for GMock however it introduces a GTest targets also which may conflict with targets from FindGTest

    endif(NOT TARGET gmock)

    set(GMOCK_LIBRARY gmock CACHE STRING "gmock target" FORCE)
    set(GMOCK_MAIN_LIBRARY gmock-main CACHE STRING "gmock-main target" FORCE)

endif(GMOCK_INCLUDE_DIR AND (NOT GMOCK_LIBRARY OR GMOCK_FORCE_SOURCES))

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GMOCK_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GMock DEFAULT_MSG
                                  GMOCK_INCLUDE_DIR GMOCK_LIBRARY GMOCK_MAIN_LIBRARY)

mark_as_advanced(GMOCK_INCLUDE_DIR GMOCK_BASE_SOURCE GMOCK_MAIN_SOURCE)
