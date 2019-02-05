
include(GenerateExportHeader)

find_package(Qt5Core 5.4 REQUIRED)

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SYSTEM_SOURCES)

# system related stuff
if (WIN32)

    set(SYSTEM_SOURCES
            ${SYSTEM_SOURCES}
            implementation/system_win32.cpp
    )

elseif(UNIX)

    set(SYSTEM_SOURCES
            ${SYSTEM_SOURCES}
            implementation/system_unix.cpp
    )

endif(WIN32)


# common stuff
set(SYSTEM_SOURCES ${SYSTEM_SOURCES}
                   implementation/filesystem.cpp
                   implementation/system.cpp
)

set(SYSTEM_HEADERS
        system.hpp
        filesystem.hpp
)

if(WIN32)

    if(DEVELOPER_BUILD)
        #Add installation path as a base path.
        set(BASE_PATH "${CMAKE_INSTALL_PREFIX}/")
        string(REPLACE "\\" "/" BASE_PATH ${BASE_PATH})
    else()
        unset(BASE_PATH)
    endif()

    configure_file(paths_win.hpp.cmake ${CMAKE_BINARY_DIR}/paths.hpp @ONLY)

else()

    configure_file(paths_unix.hpp.cmake ${CMAKE_BINARY_DIR}/paths.hpp @ONLY)

endif()

add_library(system ${SYSTEM_SOURCES} ${SYSTEM_HEADERS})
target_include_directories(system PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(system PRIVATE
                        Qt5::Core
                     )

hideSymbols(system)
generate_export_header(system)

if(BUILD_SHARED_LIBS)
    install(TARGETS system RUNTIME DESTINATION ${PATH_LIBS}
                           LIBRARY DESTINATION ${PATH_LIBS})
endif()


if(BUILD_TESTS)
    include(system_tests.cmake)
endif()
