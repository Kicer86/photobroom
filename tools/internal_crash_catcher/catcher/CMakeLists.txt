
find_package(Qt6 REQUIRED COMPONENTS Core)

include_directories(..)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRCS catcher.hpp)

set(EXTRA_LIBRARIES)

if(WIN32)

    find_package(DbgHelp REQUIRED)

    set(EXTRA_LIBRARIES ${DBGHELP_LIBRARIES})
    include_directories(${DBGHELP_INCLUDE_DIRS})

    list(APPEND SRCS catcher_win32.cpp)

elseif(UNIX AND NOT APPLE)

    list(APPEND SRCS catcher_posix.cpp)

else()

    list(APPEND SRCS catcher_null.cpp)
    message(WARNING "Crash catcher won't be installed - unsupported platform")

endif()


add_library(catcher STATIC ${SRCS})
target_include_directories(catcher PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(catcher
                        PRIVATE
                        ${EXTRA_LIBRARIES}
                        Qt::Core
)


generate_export_header(catcher)
hideSymbols(catcher)
