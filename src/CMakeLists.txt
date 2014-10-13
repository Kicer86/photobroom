
project(broom)

cmake_policy(SET CMP0028 NEW)

add_subdirectory(core)
add_subdirectory(configuration)
add_subdirectory(analyzer)
add_subdirectory(database)
add_subdirectory(database_tools)
add_subdirectory(gui)
add_subdirectory(project_utils)
add_subdirectory(system)

find_package(OpenLibrary REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories(${CMAKE_BINARY_DIR}/exports ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDES} ${Qt5Core_INCLUDE_DIRS})

set(BROOM_SOURCES main.cpp)

add_executable(broom ${BROOM_SOURCES})
target_link_libraries(broom core database configuration gui project_utils system Qt5::Core)
turnOnCpp11(broom)

install(TARGETS broom RUNTIME DESTINATION bin)
