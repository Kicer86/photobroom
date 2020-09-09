
set(UTILS_SOURCES
    completer_factory.cpp
    config_dialog_manager.cpp
    editor_factory.cpp
    iconfig_dialog_manager.cpp
    icons_loader.cpp
)

set(UTILS_HEADERS
    completer_factory.hpp
    config_dialog_manager.hpp
    editor_factory.hpp
    icompleter_factory.hpp
    iconfig_dialog_manager.hpp
    icons_loader.hpp
    ieditor_factory.hpp
)


set(UTILS_FILES_TO_MOC
    editor_factory.hpp
)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(UTILS_WRAPPED_CPP ${UTILS_FILES_TO_MOC})

add_library(gui_ui_utils OBJECT ${UTILS_SOURCES} ${UTILS_WRAPPED_CPP} ${UTILS_HEADERS})
set_target_properties(gui_ui_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gui_ui_utils
                        PRIVATE
                            core
                            database
                            KWidgetsAddons
                            Qt5::Widgets
)

target_include_directories(gui_ui_utils
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
                                ${CMAKE_CURRENT_SOURCE_DIR}
)
