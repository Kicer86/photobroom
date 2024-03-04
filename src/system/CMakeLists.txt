
include(GenerateExportHeader)

find_package(Qt6 REQUIRED COMPONENTS Core)

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

if(WIN32)
    configure_file(paths_win.hpp.cmake ${CMAKE_BINARY_DIR}/paths.hpp @ONLY)
else()
    configure_file(paths_unix.hpp.cmake ${CMAKE_BINARY_DIR}/paths.hpp @ONLY)
endif()


add_library(system)

target_sources(system
    PUBLIC
        FILE_SET CXX_MODULES FILES
            ${SYSTEM_SOURCES}
            system.cxx
            implementation/filesystem.cpp
            implementation/system.cpp
)

target_include_directories(system
                            PUBLIC
                                ${CMAKE_CURRENT_BINARY_DIR}
                                ${CMAKE_BINARY_DIR}
                            PRIVATE
                                ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(system PRIVATE Qt::Core)


if(BUILD_SHARED_LIBS)
    install(TARGETS system RUNTIME DESTINATION ${PATH_LIBS}
                           LIBRARY DESTINATION ${PATH_LIBS})
endif()


if(BUILD_TESTING)
    include(system_tests.cmake)
endif()
