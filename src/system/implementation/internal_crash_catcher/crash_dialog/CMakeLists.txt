
find_package(Qt5Widgets REQUIRED)

include_directories(SYSTEM ${Qt5Widgets_INCLUDE_DIRS})


set(SRCS
        main.cpp
    )

add_executable(crash_dialog ${SRCS})

target_link_libraries(crash_dialog
                        Qt5::Widgets
                      )
