
project(system)

include(GenerateExportHeader)

find_package(Qt5Core REQUIRED)

include_directories(${CMAKE_BINARY_DIR}/exports/)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS})

if (WIN32)
    set(SYSTEM_SOURCES implementation/system_win32.cpp)
else()
    set(SYSTEM_SOURCES implementation/system_unix.cpp)
endif(WIN32)

set(SYSTEM_HEADERS system.hpp)

add_library(system SHARED ${SYSTEM_SOURCES} ${SYSTEM_HEADERS})
generate_export_header(system EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/system_export.h)

qt5_use_modules(system Core)
