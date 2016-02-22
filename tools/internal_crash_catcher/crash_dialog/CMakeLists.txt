
find_package(Qt5Widgets REQUIRED)

include_directories(SYSTEM ${Qt5Widgets_INCLUDE_DIRS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(HEADERS
    ui/crash_dialog.hpp
    debugger_factory.hpp
    idebugger.hpp
)

set(SRCS
    ui/crash_dialog.cpp
    idebugger.cpp
    main.cpp
)

set(UI_FILES
    ui/crash_dialog.ui
)

set(FILES_TO_MOC
    ui/crash_dialog.hpp
)


if(UNIX)

    set(HEADERS ${HEADERS}
            unix/gdb_wrapper.hpp
        )

    set(SRCS ${SRCS}
            unix/debugger_factory.cpp
            unix/gdb_wrapper.cpp
        )

else()

endif()


qt5_wrap_cpp(WRAPPED_CPP ${FILES_TO_MOC})
qt5_wrap_ui(WRAPPED_UI ${UI_FILES})

add_executable(crash_dialog ${SRCS} ${HEADERS} ${WRAPPED_CPP} ${WRAPPED_UI})

target_link_libraries(crash_dialog
                        Qt5::Widgets
                      )
