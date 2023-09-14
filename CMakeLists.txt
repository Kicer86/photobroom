
cmake_minimum_required(VERSION 3.18)
project(PhotoBroom VERSION 1.7.0 LANGUAGES CXX)

if(POLICY CMP0146)
    cmake_policy(SET CMP0146 OLD)
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake_modules;
)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

#check if git modules are setup
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/cmake_modules/FindEasyExif.cmake)
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

if(MSVC_VERSION)
    #enable auto_ptr. Disabled by VS for c++17 but required by exiv 0.27
    add_definitions(-D_HAS_AUTO_PTR_ETC=1)
    add_compile_options(/wd4251)
elseif(APPLE)
    #enable auto_ptr. Disabled by clang on MacOS for c++17 but required by exiv 0.27
    add_definitions(-D_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
endif()

#objdump
if(ENABLE_OBJDUMPING)
    add_custom_target(DumpObjs ALL)
endif()

#options:
option(BUILD_LEARNING_TESTS "Build learning tests" OFF)
option(RUN_TESTS_AFTER_BUILD "Run unit tests after build.")
option(BUILD_UPDATER "Enable 'updater' module" ${WIN32})
option(STATIC_PLUGINS "Build plugins as static" OFF)

#options description:
add_feature_info("Run unit test after build" RUN_TESTS_AFTER_BUILD "Runs unit tests after build. Feature controled by RUN_TESTS_AFTER_BUILD variable.")
add_feature_info("Enable 'updater' module" BUILD_UPDATER "Build module responsible for online version check.")
add_feature_info("Static plugins" STATIC_PLUGINS "Build all plugins as static modules.")
add_feature_info("Build id" PHOTO_BROOM_BUILD_ID "Build id attached to installer version")

#tests
if(BUILD_TESTING)

    enable_testing()

    if(${RUN_TESTS_AFTER_BUILD})
        set(run_ut ALL)
    endif()

    add_custom_target(RunUnitTests ${run_ut}
                      COMMAND TSAN_OPTIONS=suppressions=${CMAKE_SOURCE_DIR}/sanitizer-thread-suppressions.txt ctest -L UnitTest --output-on-failure)
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

#system dependent stuff
if(UNIX OR CYGWIN)
    include(${PROJECT_SOURCE_DIR}/cmake/system/linux.cmake)
elseif(WIN32)
    include(${PROJECT_SOURCE_DIR}/cmake/system/win32.cmake)
endif(UNIX OR CYGWIN)

#subdirs
add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(tr)

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
