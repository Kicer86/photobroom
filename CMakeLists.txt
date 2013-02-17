
project(broom)

cmake_minimum_required(VERSION 2.8)

find_package(OpenLibrary REQUIRED utils)

include_directories(${OPENLIBRARY_INCLUDES})

set(BROOM_SOURCES
    src/configuration/configurationfactory.cpp
    src/configuration/entrydata.cpp
    src/main.cpp
   )

add_executable(broom ${BROOM_SOURCES})
set_target_properties(broom PROPERTIES COMPILE_FLAGS --std=c++11)

add_subdirectory(src/tests)
