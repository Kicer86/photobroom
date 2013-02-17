
project(broom)

cmake_minimum_required(VERSION 2.8)


set(BROOM_SOURCES
    src/configruation/configurationfactory.cpp
    src/configuration/entrydata.cpp
    src/main.cpp
   )

add_executable(broom ${BROOM_SOURCES})

set_target_properties(broom PROPERTIES COMPILE_FLAGS --std=c++11)
