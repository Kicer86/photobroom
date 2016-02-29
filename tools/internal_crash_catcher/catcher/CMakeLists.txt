
find_package(Qt5Core REQUIRED)

include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRCS
    catcher.hpp
    crash_dialog_launcher.hpp
)

set(EXTRA_LIBRARIES)

if(WIN32)

    find_package(DbgHelp REQUIRED)

    set(EXTRA_LIBRARIES ${DBGHELP_LIBRARIES})
    include_directories(${DBGHELP_INCLUDE_DIRS})

    list(APPEND SRCS catcher_win32.cpp
                     crash_dialog_launcher_win32.cpp
                     crash_dialog_launcher_win32.hpp)

elseif(UNIX)

    list(APPEND SRCS catcher_posix.cpp)

else()

    message(FATAL_ERROR "Unsupported platform")

endif()


add_library(catcher STATIC ${SRCS})
target_include_directories(catcher PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(catcher
                        PRIVATE
                        ${EXTRA_LIBRARIES}
                        Qt5::Core
)


generate_export_header(catcher)
hideSymbols(catcher)
