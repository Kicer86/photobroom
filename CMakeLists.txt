
cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0063 NEW)
cmake_policy(SET CMP0075 NEW)

if (POLICY CMP0100)
    cmake_policy(SET CMP0100 NEW)
endif()

project(PhotoBroom VERSION 1.4.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake;${CMAKE_SOURCE_DIR}/cmake_modules)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(BUILD_SHARED_LIBS ON)       # static linking may cause problems with std::any_cast

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(NO_OUTPUT_DIRS TARGETS)
endif()

#check if git modules are setup
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/cmake_modules/FindGTest.cmake)
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
include(CheckCXXCompilerFlag)
include(CTest)
include(${PROJECT_SOURCE_DIR}/cmake/functions.cmake)
include(${PROJECT_SOURCE_DIR}/cmake_modules/DeveloperTools.cmake)

# enable/disable concepts
check_cxx_compiler_flag("-fconcepts" CONCEPTS_SUPPORTED)

if(CONCEPTS_SUPPORTED)
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
        if(MSVC_VERSION LESS 1916)
            add_definitions(-D__cpp_variable_templates=201304)
        endif()
    else()
        message(FATAL_ERROR "Visual Studio 2017 is required")
    endif()
endif()

if(MSVC_VERSION)
    #enable auto_ptr. Disabled by VS for c++17 but required by exiv 0.27
    add_definitions(-D_HAS_AUTO_PTR_ETC=1)
    add_compile_options(/wd4251)
endif()

#options:
option(RUN_TESTS_AFTER_BUILD "Run unit tests after build.")
option(BUILD_UPDATER "Enable 'updater' module" ${WIN32})
option(STATIC_PLUGINS "Build plugins as static" OFF)

#options description:
add_feature_info("Run unit test after build" RUN_TESTS_AFTER_BUILD "Runs unit tests after build. Feature controled by RUN_TESTS_AFTER_BUILD variable.")
add_feature_info("Enable 'updater' module" BUILD_UPDATER "Build module responsible for online version check.")
add_feature_info("Static plugins" STATIC_PLUGINS "Build all plugins as static modules.")

#tests
if(BUILD_TESTING)

    # setup paths for finders
    set(GMOCK_DIR "${PROJECT_SOURCE_DIR}/external/googletest/googlemock")
    set(GTEST_DIR "${PROJECT_SOURCE_DIR}/external/googletest/googletest")
    set(GMOCK_FORCE_SOURCES TRUE)
    set(GTEST_FORCE_SOURCES TRUE)

    enable_testing()

    if(${RUN_TESTS_AFTER_BUILD})
        set(run_ut ALL)
    endif()

    add_custom_target(RunUnitTests ${run_ut}
                      COMMAND TSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/sanitizer-thread-suppressions.txt ctest -L UnitTest)
endif(BUILD_TESTING)

#system related definitions
if(UNIX OR CYGWIN)
    add_definitions(-DOS_UNIX)
elseif(WIN32)
    add_definitions(-DOS_WIN)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
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
        set(PATH_DATA     "share/photo_broom/")
elseif(WIN32)
        set(PATH_BIN      ".")
        set(PATH_LIBS     ".")
        set(PATH_PLUGINS  "plugins")
        set(PATH_TR       "tr")
        set(PATH_DOCS     "docs")
        set(PATH_ICONS    "icons")
        set(PATH_SCRIPTS  "scripts")
        set(PATH_DATA     ".")
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
add_subdirectory(external)

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

#icons
set(SVG_ICONS images/photo_broom.svg)

#ci/cd
configure_file(${CMAKE_SOURCE_DIR}/ci/appveyor.yml.in ${CMAKE_SOURCE_DIR}/appveyor.yml @ONLY)

#unit tests
if(BUILD_TESTING)

    #disable compilation warnings for googletest
    foreach(gtest_target gtest gtest-main gmock gmock-main)
        if(TARGET ${gtest_target})
            disableWarnings(${gtest_target})
        endif()
    endforeach()

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
    message("deploy                 - generates list of external libraries used by project and adds them to instalation (and package). Run manually before package on Windows")
endif()
if(BUILD_TESTING)
    message("test                   - runs tests (ctest command can also be used)")
    message("run_unit_tests_code_coverage - runs tests and collects code coverage")
endif()
message("")

feature_summary(WHAT ALL)
