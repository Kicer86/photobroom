
project(broom)

#download packages required on windows
if(WIN32)
    include(${CMAKE_SOURCE_DIR}/cmake/3rd_party/nettle.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/3rd_party/easyexif.cmake)
    
    addEasyExif()
    addNettle()
endif()


add_subdirectory(core)
add_subdirectory(configuration)
add_subdirectory(analyzer)
add_subdirectory(database)
add_subdirectory(gui)
add_subdirectory(system)

find_package(OpenLibrary REQUIRED utils)
find_package(Boost REQUIRED)

include_directories(${CMAKE_BINARY_DIR}/exports)
include_directories(SYSTEM ${OPENLIBRARY_INCLUDES} ${Boost_INCLUDE_DIRS})

set(BROOM_SOURCES main.cpp)

add_executable(broom ${BROOM_SOURCES})
target_link_libraries(broom gui configuration)
turnOnCpp11(broom)

install(TARGETS broom RUNTIME DESTINATION bin)
