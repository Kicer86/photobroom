
project(system)

include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports/)

if (WIN32)
    set(SYSTEM_SOURCES implementation/system_win32.cpp)
else()
    set(SYSTEM_SOURCES implementation/system_unix.cpp)
endif(WIN32)
    
set(SYSTEM_HEADERS system.hpp)

add_library(system SHARED ${SYSTEM_SOURCES} ${SYSTEM_HEADERS})
generate_export_header(system EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/system_export.h)
