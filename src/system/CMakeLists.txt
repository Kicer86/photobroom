
include(GenerateExportHeader)

find_package(Qt5Core 5.4 REQUIRED)
find_package(KF5Crash)

include_directories(${CMAKE_BINARY_DIR}/exports/ ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)


# crash handling
set(CRASH_HANDLER_FOUND FALSE)
set(CRASH_HANDLER_SOURCES)
set(CRASH_HANDLER_LIBRARIES)

if(KF5Crash_FOUND)

    include_directories(SYSTEM $<TARGET_PROPERTY:KF5::Crash,INTERFACE_INCLUDE_DIRECTORIES>)

    set(CRASH_HANDLER_SOURCES implementation/crash_catcher_kde5.cpp)
    set(CRASH_HANDLER_LIBRARIES KF5::Crash)
    set(CRASH_HANDLER_FOUND TRUE)

endif(KF5Crash_FOUND)

if(NOT CRASH_HANDLER_FOUND)
    set(CRASH_HANDLER_SOURCES implementation/crash_catcher_null.cpp)
endif()


set(SYSTEM_SOURCES ${CRASH_HANDLER_SOURCES})


# system related stuff
if (WIN32)

    include_directories(SYSTEM ${DBGHELP_INCLUDE_DIRS})

    set(SYSTEM_SOURCES
            ${SYSTEM_SOURCES}
            implementation/system_win32.cpp
    )

elseif(UNIX)

    include_directories(SYSTEM ${LIBBACKTRACE_INCLUDE_DIRS})

    set(SYSTEM_SOURCES
            ${SYSTEM_SOURCES}
            implementation/system_unix.cpp
    )

endif(WIN32)


# common stuff
set(SYSTEM_SOURCES ${SYSTEM_SOURCES}
                   implementation/crash_catcher.cpp
                   implementation/filesystem.cpp
                   implementation/system.cpp
)

set(SYSTEM_HEADERS
        system.hpp
        filesystem.hpp
        crash_catcher.hpp
)

if(WIN32)

    if(DEVELOPER_BUILD)
        #Add installation path as a base path.
        set(BASE_PATH "${CMAKE_INSTALL_PREFIX}/")
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
                        ${CRASH_HANDLER_LIBRARIES}
                     )

hideSymbols(system)
generate_export_header(system EXPORT_FILE_NAME)

if(BUILD_SHARED_LIBS)
    install(TARGETS system RUNTIME DESTINATION ${PATH_LIBS}
                           LIBRARY DESTINATION ${PATH_LIBS})
endif()
