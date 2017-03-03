
cmake_minimum_required(VERSION 3.1)                           # due to CMAKE_CXX_STANDARD variables
cmake_policy(SET CMP0048 NEW)

if (NOT "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" VERSION_LESS "3.3")
    cmake_policy(SET CMP0063 NEW)
endif()

project(PhotoBroom VERSION 1.2.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

#options for CPack
set(CPACK_PACKAGE_VENDOR "Michal Walenciak")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Photo Broom - tool for managing your photos")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/docs/gpl-3.0.txt)

#include modules
include(FeatureSummary)
include(GenerateExportHeader)
include(${PROJECT_SOURCE_DIR}/cmake/functions.cmake)
include(CheckCXXCompilerFlag)

# enable/disable concepts
check_cxx_compiler_flag("-fconcepts" CONCEPTS_SUPPORTED)

if (CONCEPTS_SUPPORTED)
    add_compile_options("-fconcepts")
    add_definitions(-DCONCEPTS_SUPPORTED)
endif()

#options:
option(BUILD_TESTS "Build tests. Set to ON to build tests.")
option(BUILD_SHARED_LIBS "Global flag to cause add_library to create shared libraries if on.")
option(DEVELOPER_BUILD "Enable developer build. Enable it to make debugging easier.")

#options description:
add_feature_info("Build tests" BUILD_TESTS "Enables build of tests. Feature controled by BUILD_TESTS variable.")
add_feature_info("Enable developer build" DEVELOPER_BUILD "Enable it to make debugging easier.")

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
if(UNIX OR CYGWIN)
        set(PATH_BIN      "bin")
        set(PATH_LIBS     "lib/photo_broom")
        set(PATH_PLUGINS  "lib/photo_broom/plugins")
        set(PATH_TR       "share/photo_broom/tr")
        set(PATH_DOCS     "share/photo_broom/docs")
elseif(WIN32)
        set(PATH_BIN      ".")
        set(PATH_LIBS     ".")
        set(PATH_PLUGINS  "plugins")
        set(PATH_TR       "tr")
        set(PATH_DOCS     "docs")
endif(UNIX OR CYGWIN)

# RPATH
# http://www.cmake.org/Wiki/CMake_RPATH_handling
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PATH_LIBS}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#subdirs
add_subdirectory(images)
add_subdirectory(src)
add_subdirectory(tr)
add_subdirectory(tools)

#system dependent stuff
if(UNIX OR CYGWIN)
    include(${PROJECT_SOURCE_DIR}/cmake/system/linux.cmake)
elseif(WIN32)
    include(${PROJECT_SOURCE_DIR}/cmake/system/win32.cmake)
endif(UNIX OR CYGWIN)

#documentation
find_program(DOXYGEN doxygen)
add_feature_info("Documentation generation" DOXYGEN "When doxygen program is found, extra build target is enabled.")

if(DOXYGEN)

    add_custom_target(documentation
                      COMMAND ${DOXYGEN} ${CMAKE_SOURCE_DIR}/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )

endif(DOXYGEN)

#unit tests
if(BUILD_TESTS)
    add_custom_target(RunUnitTests ALL
                      COMMAND ctest
                      DEPENDS core_tests
                              database_tests
                              gui_tests
                              photos_crawler_tests
                     )
endif()

#package
include(CPack)

#info
message("")
message("*************************************************************")
message("Configuration summary:")
message("")
message("Extra targets:")
if(DOXYGEN)
    message("documentation          - generates html in build dir")
endif()
message("generate_code_coverage - generates code coverage in build dir")
message("package                - generates broom installer")
if(TARGET deploy)
    message("deploy                 - generates list of external libraries used by project and adds them to instalation (and package)")
endif()
if(BUILD_TESTS)
    message("test                   - runs tests (ctest command can be also used)")
endif()
message("")

feature_summary(WHAT ALL)
