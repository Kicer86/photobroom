
project(broom)

cmake_minimum_required(VERSION 2.8.10)   #.10 required for visual studio 2012
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(BUILD_TESTS FALSE CACHE BOOL "Build tests. Set to TRUE to build tests")
if(BUILD_TESTS)
    enable_testing()
endif(BUILD_TESTS)    

if(UNIX OR CYGWIN)
    add_definitions(-DOS_UNIX)
elseif(WIN32)
    add_definitions(-DOS_WIN)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif(UNIX OR CYGWIN)

add_subdirectory(src/core)
add_subdirectory(src/analyzer)
add_subdirectory(src/database)
add_subdirectory(src/gui)
add_subdirectory(src/tests)

find_package(OpenLibrary REQUIRED utils)

include_directories(${CMAKE_BINARY_DIR}/exports ${OPENLIBRARY_INCLUDES})

set(BROOM_SOURCES
    src/configuration/configurationfactory.cpp
    src/configuration/entrydata.cpp
    src/main.cpp
   )


set(BROOM_HEADERS
    src/configuration/configurationfactory.hpp
    src/configuration/entrydata.hpp
    src/configuration/iconfiguration.hpp
	src/configuration/ientry.hpp
   )

source_group(configuration REGULAR_EXPRESSION .*configuration.* )

add_executable(broom ${BROOM_SOURCES} ${BROOM_HEADERS})
turnOnCpp11(broom)
turnOnAllWarnings(${BROOM_SOURCES})

target_link_libraries(broom analyzer database gui)

message("Configuration: ")
message("BUILD_TESTS...........${BUILD_TESTS}")
