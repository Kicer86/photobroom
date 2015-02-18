
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

set(SYSTEM_SOURCES ${SYSTEM_SOURCES} 
                   implementation/filesystem.cpp
                   implementation/system.cpp
)
                   
set(SYSTEM_HEADERS system.hpp filesystem.hpp)

if(WIN32)
    configure_file(paths_win.hpp.cmake ${CMAKE_BINARY_DIR}/paths.hpp @ONLY)
else()
    configure_file(paths_unix.hpp.cmake ${CMAKE_BINARY_DIR}/paths.hpp @ONLY)
endif()

add_library(system SHARED ${SYSTEM_SOURCES} ${SYSTEM_HEADERS})
target_link_libraries(system PRIVATE Qt5::Core)
target_include_directories(system PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

hideSymbols(system)
generate_export_header(system EXPORT_FILE_NAME)

install(TARGETS system RUNTIME DESTINATION ${PATH_LIBS}
                       LIBRARY DESTINATION ${PATH_LIBS})
