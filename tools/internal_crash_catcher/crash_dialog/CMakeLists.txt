
find_package(Qt5Widgets REQUIRED)

include_directories(SYSTEM ${Qt5Widgets_INCLUDE_DIRS})

set(HEADERS
   )

set(SRCS
        main.cpp
    )
    
if(UNIX)
    
    set(HEADERS ${HEADERS}
            unix/gdb_wrapper.hpp
        )
            
    set(SRCS ${SRCS}
            unix/gdb_wrapper.cpp
        )

else()

endif()


add_executable(crash_dialog ${SRCS} ${HEADERS})

target_link_libraries(crash_dialog
                        Qt5::Widgets
                      )
