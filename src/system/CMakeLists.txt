
project(system)

include(GenerateExportHeader)


if (WIN32)
    set(SYSTEM_SOURCES implementation/system_win32.cpp)
else()
    set(SYSTEM_SOURCES implementation/system_unix.cpp)
endif(WIN32)
    
set(SYSTEM_HEADERS system.hpp)

add_library(system SHARED ${SYSTEM_SOURCES} ${SYSTEM_HEADERS})
