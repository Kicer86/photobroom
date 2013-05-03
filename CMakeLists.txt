
project(broom)

cmake_minimum_required(VERSION 2.8.10)   #.10 required for visual studio 2012

add_subdirectory(src/analyzer)
add_subdirectory(src/tests)

find_package(OpenLibrary REQUIRED utils)

include_directories(${CMAKE_BINARY_DIR} ${OPENLIBRARY_INCLUDES})

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

target_link_libraries(broom analyzer)
