
add_subdirectory(win32/kdbgwin)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Widgets_INCLUDE_DIRS}
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


if(DEVELOPER_BUILD)

    add_definitions(-DDEVELOPER_BUILD)
    add_custom_command(OUTPUT crash_dialog_config.hpp
                       COMMAND config_generator
                       ARGS -o crash_dialog_config.hpp
                            --entry test_app_path=\\"$<TARGET_FILE:test_app>\\")

    add_custom_target(crash_dialog_config
                      DEPENDS crash_dialog_config.hpp)

endif()


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
        win32/debugger.hpp
    )

    set(SRCS ${SRCS}
        win32/debugger_factory.cpp
        win32/debugger_gcc.cpp
    )

    set(FILES_TO_MOC ${FILES_TO_MOC}
    )

endif()


qt5_wrap_cpp(WRAPPED_CPP ${FILES_TO_MOC})
qt5_wrap_ui(WRAPPED_UI ${UI_FILES})

add_executable(crash_dialog ${SRCS} ${HEADERS} ${WRAPPED_CPP} ${WRAPPED_UI})
target_link_libraries(crash_dialog
                        Qt5::Core
                        Qt5::Widgets
)

install(TARGETS crash_dialog RUNTIME DESTINATION ${PATH_BIN})


if(DEVELOPER_BUILD)
    add_dependencies(crash_dialog crash_dialog_config)
endif()
