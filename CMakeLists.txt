
cmake_minimum_required(VERSION 3.8)                           # due to CMAKE_CXX_STANDARD variables and c++17 support
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0063 NEW)

project(PhotoBroom VERSION 1.3.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_private;${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

#check if git modules are setup
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/cmake/FindGTest.cmake OR
   NOT EXISTS ${PROJECT_SOURCE_DIR}/external/dlib)
    message(FATAL_ERROR "Git submodules were not updated. See docs/build.txt for instructions.")
endif()

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
include(${PROJECT_SOURCE_DIR}/cmake_private/functions.cmake)
include(CheckCXXCompilerFlag)

# enable/disable concepts
check_cxx_compiler_flag("-fconcepts" CONCEPTS_SUPPORTED)

if (CONCEPTS_SUPPORTED)
    add_compile_options("-fconcepts")
    add_definitions(-DCONCEPTS_SUPPORTED)
endif()

# workaround for a bug https://developercommunity.visualstudio.com/content/problem/74173/cannot-use-some-functionalities-of-qt-framework-be.html
# also see: https://bugreports.qt.io/browse/QTBUG-61667
#
# + CMake missing implementations:
# https://stackoverflow.com/questions/44960715/how-to-enable-stdc17-in-vs2017-with-cmake
if(MSVC_VERSION)
    if(MSVC_VERSION GREATER_EQUAL 1900)
        add_definitions(-D__cpp_variable_templates=201304)

        include(CheckCXXCompilerFlag)
        CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
        if (_cpp_latest_flag_supported)
            add_compile_options("/std:c++latest")
            message("Enabling c++latest for Visual Studio")
        else()
            message(FATAL_ERROR "Enabling c++latest for Visual Studio is not possible")
        endif()
    else()
        message(FATAL_ERROR "Visual Studio 2017 is required")
    endif()
endif()

#options:
option(BUILD_TESTS "Build tests. Set to ON to build tests.")
option(BUILD_SHARED_LIBS "Global flag to cause add_library to create shared libraries if on.")
option(BUILD_UPDATER "Enable 'updater' module" WIN32)
option(DEVELOPER_BUILD "Enable developer build. Enable it to make debugging easier.")

#options description:
add_feature_info("Build tests" BUILD_TESTS "Enables build of tests. Feature controled by BUILD_TESTS variable.")
add_feature_info("Enable developer build" DEVELOPER_BUILD "Enable it to make debugging easier.")
add_feature_info("Enable 'updater' module" BUILD_UPDATER "Build module responsible for online version check.")

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
        set(PATH_ICONS    "share/photo_broom/icons")
        set(PATH_SCRIPTS  "share/photo_broom/scripts")
elseif(WIN32)
        set(PATH_BIN      ".")
        set(PATH_LIBS     ".")
        set(PATH_PLUGINS  "plugins")
        set(PATH_TR       "tr")
        set(PATH_DOCS     "docs")
        set(PATH_ICONS    "icons")
        set(PATH_SCRIPTS  "scripts")
endif(UNIX OR CYGWIN)

# RPATH
# http://www.cmake.org/Wiki/CMake_RPATH_handling
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PATH_LIBS}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#subdirs
add_subdirectory(external)
add_subdirectory(images)
add_subdirectory(src)
add_subdirectory(tr)
add_subdirectory(tools)

#system dependent stuff
if(UNIX OR CYGWIN)
    include(${PROJECT_SOURCE_DIR}/cmake_private/system/linux.cmake)
elseif(WIN32)
    include(${PROJECT_SOURCE_DIR}/cmake_private/system/win32.cmake)
endif(UNIX OR CYGWIN)

#documentation
find_program(DOXYGEN doxygen)
add_feature_info("Documentation generation" DOXYGEN "When doxygen program is found, extra build target is enabled.")

if(DOXYGEN)

    add_custom_target(documentation
                      COMMAND ${DOXYGEN} ${CMAKE_SOURCE_DIR}/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )

endif(DOXYGEN)

#icons
set(SVG_ICONS images/photo_broom.svg)

#unit tests
if(BUILD_TESTS)

    #disable compilation warnings for googletest
    foreach(gtest_target gtest gtest-main gmock gmock-main)
        if(TARGET gtest)
            disableWarnings(gtest)
        endif()
    endforeach()

    add_custom_target(RunUnitTests ALL
                      COMMAND TSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/sanitizer-thread-suppressions.txt ctest
                      DEPENDS core_tests
                              database_tests
                              database_integration_tests
                              gui_tests
                              photos_crawler_tests
                     )
endif()

#installation steps
install(FILES ${SVG_ICONS} DESTINATION ${PATH_ICONS}/svg)

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
