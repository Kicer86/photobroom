
if(WIN32)
    add_subdirectory(win32/kdbgwin)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(HEADERS
    ui/crash_dialog.hpp
    command_line_parser.hpp
    debugger_factory.hpp
    idebugger.hpp
    idebugger_factory.hpp
)

set(SRCS
    ui/crash_dialog.cpp
    command_line_parser.cpp
    idebugger.cpp
    idebugger_factory.cpp
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

    set(FILES_TO_MOC ${FILES_TO_MOC}
            unix/gdb_wrapper.hpp
    )

else()

    set(HEADERS ${HEADERS}
        win32/kdbgwin_wrapper.hpp
    )

    set(SRCS ${SRCS}
        win32/debugger_factory.cpp
        win32/kdbgwin_wrapper.cpp
    )

    set(FILES_TO_MOC ${FILES_TO_MOC}
        win32/kdbgwin_wrapper.hpp
    )

endif()


qt6_wrap_cpp(WRAPPED_CPP ${FILES_TO_MOC})
qt6_wrap_ui(WRAPPED_UI ${UI_FILES})

add_executable(crash_dialog ${SRCS} ${HEADERS} ${WRAPPED_CPP} ${WRAPPED_UI})
target_link_libraries(crash_dialog
                        system
                        Qt::Core
                        Qt::Widgets
)

install(TARGETS crash_dialog RUNTIME DESTINATION ${PATH_BIN})
