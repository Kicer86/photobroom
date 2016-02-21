
find_package(Qt5Widgets REQUIRED)

include_directories(SYSTEM ${Qt5Widgets_INCLUDE_DIRS})

set(HEADERS
        gdb_wrapper.hpp
   )

set(SRCS
        main.cpp
        gdb_wrapper.cpp
    )

add_executable(crash_dialog ${SRCS} ${HEADERS})

target_link_libraries(crash_dialog
                        Qt5::Widgets
                      )
