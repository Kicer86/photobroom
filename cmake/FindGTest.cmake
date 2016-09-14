


# - Try to find GTest
# Once done this will define
#  GTEST_FOUND        - System has GTest
#  GTEST_INCLUDE_DIRS - The GTest include directories
#  GTEST_LIBRARY      - GTest library
#  GTEST_MAIN_LIBRARY - GTest main library

find_path(GTEST_INCLUDE_DIR gtest/gtest.h
          HINTS ${GTEST_DIR}/include
                ${GMOCK_DIR}/gtest/include)

find_library(GTEST_LIBRARY      libgtest.a)
find_library(GTEST_MAIN_LIBRARY libgtest_main.a)

set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR} )

if(GTEST_INCLUDE_DIR AND NOT GTEST_LIBRARY)

    if(NOT TARGET gtest)

        message("Trying to find GTest sources and register extra targets")
        find_file(GTEST_BASE_SOURCE src/gtest-all.cc
                  HINTS /usr/src/gtest
                        ${GTEST_DIR}
                        ${GMOCK_DIR}/gtest
                  )

        find_file(GTEST_MAIN_SOURCE src/gtest_main.cc
                  HINTS /usr/src/gtest
                        ${GTEST_DIR}
                        ${GMOCK_DIR}/gtest
                  )

        if(NOT GTEST_BASE_SOURCE OR NOT GTEST_MAIN_SOURCE)
            message(FATAL_ERROR "Could not find base for GTest sources. Set GTEST_DIR to proper value")
        endif(NOT GTEST_BASE_SOURCE OR NOT GTEST_MAIN_SOURCE)

        add_library(gtest STATIC ${GTEST_BASE_SOURCE})
        add_library(gtest-main STATIC ${GTEST_MAIN_SOURCE})

        get_filename_component(gtest_base_dir ${GTEST_BASE_SOURCE} PATH)
        target_include_directories(gtest      
                                    PRIVATE 
                                        ${gtest_base_dir}/..
                                        ${gtest_base_dir}/../include 
        )

        target_include_directories(gtest-main 
                                    PRIVATE 
                                        ${gtest_base_dir}/..
                                        ${gtest_base_dir}/../include 
        )
                        
    endif(NOT TARGET gtest)

    set(GTEST_LIBRARY gtest)
    set(GTEST_MAIN_LIBRARY gtest-main)

endif(GTEST_INCLUDE_DIR AND NOT GTEST_LIBRARY)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GTEST_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(GTest DEFAULT_MSG
                                  GTEST_INCLUDE_DIR GTEST_BASE_SOURCE GTEST_MAIN_SOURCE)

mark_as_advanced(GTEST_INCLUDE_DIR GTEST_BASE_SOURCE GTEST_MAIN_SOURCE)
