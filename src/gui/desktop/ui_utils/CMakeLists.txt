
set(UTILS_SOURCES
    completer_factory.cpp
    config_dialog_manager.cpp
    iconfig_dialog_manager.cpp
)

set(UTILS_HEADERS
    completer_factory.hpp
    config_dialog_manager.hpp
    icompleter_factory.hpp
    iconfig_dialog_manager.hpp
)

add_library(gui_ui_utils OBJECT ${UTILS_SOURCES} ${UTILS_HEADERS})
target_link_libraries(gui_ui_utils
                        PRIVATE
                            core
                            database
                            Qt::Widgets
)

target_include_directories(gui_ui_utils
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
                                ${CMAKE_CURRENT_SOURCE_DIR}
)
