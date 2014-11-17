
include(GenerateExportHeader)

find_package(Qt5Core REQUIRED)

include_directories(${CMAKE_BINARY_DIR}/exports/ ${CMAKE_BINARY_DIR})
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (WIN32)
    set(SYSTEM_SOURCES implementation/system_win32.cpp)
else()
    set(SYSTEM_SOURCES implementation/system_unix.cpp)
endif(WIN32)

set(SYSTEM_SOURCES ${SYSTEM_SOURCES} implementation/filesystem.cpp)
set(SYSTEM_HEADERS system.hpp filesystem.hpp)

if(WIN32)
    configure_file(paths_win.hpp.cmake ${CMAKE_BINARY_DIR}/paths.hpp @ONLY)
else()
    configure_file(paths_unix.hpp.cmake ${CMAKE_BINARY_DIR}/paths.hpp @ONLY)
endif()

add_library(system ${SYSTEM_SOURCES} ${SYSTEM_HEADERS})
generate_export_header(system EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/system_export.h)

target_link_libraries(system PRIVATE Qt5::Core)
turnOnCpp11(system)
hideSymbols(system)

install(TARGETS system DESTINATION ${PATH_LIBS})
