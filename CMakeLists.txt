
cmake_policy(SET CMP0048 NEW)

project(broom VERSION 0.90 LANGUAGES CXX)

cmake_minimum_required(VERSION 3.0)   # due to VERSION argument for project()
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#options for CPack
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/docs/gpl-3.0.txt)

#include modules
include(CPack)
include(FeatureSummary)
include(GenerateExportHeader)
include(${PROJECT_SOURCE_DIR}/cmake/functions.cmake)

#options:
option(BUILD_SHARED_LIBS "Build all libraries as shared" TRUE)
option(BUILD_TESTS "Build tests. Set to ON to build tests")
option(AUTODOWNLOAD_EASYEXIF "Automatically download EasyExif package if not found")

#options description:
add_feature_info("Build tests" BUILD_TESTS "Enables build of tests. Feature controled by BUILD_TESTS variable.")
add_feature_info("Build shared libraries" BUILD_SHARED_LIBS "Builds all libraries as shared. Feature controlled by BUILD_SHARED_LIBS variable.")
add_feature_info("Automatically download EasyExif" AUTODOWNLOAD_EASYEXIF "When set to ON, EasyExif library will be automatically downloaded when not found.")

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

#3rd party packages auto download
if(AUTODOWNLOAD_EASYEXIF)
    include(${PROJECT_SOURCE_DIR}/cmake/3rd_party/easyexif.cmake)
       
    #command for 3rd_party dir creation
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/3rd_party/src/dir
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/3rd_party/src/
                       COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/3rd_party/src/dir)
    
    addEasyExif()
endif(AUTODOWNLOAD_EASYEXIF)

#base paths
if(UNIX OR CYGWIN)
	set(PATH_LIBS     "lib/broom")
	set(PATH_PLUGINS  "lib/broom/plugins")
	set(PATH_TR       "share/broom/tr")
elseif(WIN32)
	set(PATH_LIBS     "")
	set(PATH_PLUGINS  "plugins")
	set(PATH_TR       "tr")
endif(UNIX OR CYGWIN)

# RPATH
# http://www.cmake.org/Wiki/CMake_RPATH_handling
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PATH_LIBS}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#subdirs
add_subdirectory(src)
add_subdirectory(tr)

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
message("Configuration summary:")
message("")
message("Extra targets:")
if(DOXYGEN)
    message("documentation          - generates html in build dir")
endif()
message("generate_code_coverage - generates code coverage in build dir")
message("package                - generates broom installer")
message("")

feature_summary(WHAT ALL)
