
project(broom CXX)

cmake_minimum_required(VERSION 2.8.10)   #.10 required for visual studio 2012
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(FeatureSummary)
include(GenerateExportHeader)
include(${PROJECT_SOURCE_DIR}/cmake/functions.cmake)

#options:
option(BUILD_SHARED_LIBS "Build all libraries as shared" TRUE)
option(BUILD_TESTS "Build tests. Set to ON to build tests")

#options description:
add_feature_info("Build tests" BUILD_TESTS "Enables build of tests. Feature controled by BUILD_TESTS variable.")
add_feature_info("Build shared libraries" BUILD_SHARED_LIBS "Builds all libraries as shared. Feature controlled by BUILD_SHARED_LIBS variable.")

#tests
if(BUILD_TESTS)
    enable_testing()
endif(BUILD_TESTS)

#system related definitions
if(UNIX OR CYGWIN)
    add_definitions(-DOS_UNIX)
elseif(WIN32)
    add_definitions(-DOS_WIN)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif(UNIX OR CYGWIN)

#base paths
set(PATH_LIBS     "lib/broom")
set(PATH_PLUGINS  "lib/broom/plugins")

#subdirs
add_subdirectory(src)

#documentation
find_program(DOXYGEN doxygen)
add_feature_info("Documentation generation" DOXYGEN "When doxygen program is found, extra build target is enabled.")

if(DOXYGEN)

    add_custom_target(documentation
                      COMMAND ${DOXYGEN} ${CMAKE_SOURCE_DIR}/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )

endif(DOXYGEN)

#info
message("")
message("*************************************************************")
message("Extra targets:")
if(DOXYGEN)
    message("documentation          - generates html in build dir")
endif()
message("generate_code_coverage - generates code coverage in build dir")
message("")

feature_summary(WHAT ALL)
