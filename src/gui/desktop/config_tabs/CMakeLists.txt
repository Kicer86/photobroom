
set(CONFIG_TABS_SOURCES
    main_tab.cpp
    tools_tab.cpp
)

set(CONFIG_TABS_HEADERS
    a_config_tab.hpp
    main_tab.hpp
    tools_tab.hpp
)


set(CONFIG_TABS_FILES_TO_MOC
    main_tab.hpp
    tools_tab.hpp
)

set(CONFIG_TABS_UI_FILES
    main_tab.ui
    tools_tab.ui
)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt6_wrap_cpp(CONFIG_TABS_WRAPPED_CPP ${CONFIG_TABS_FILES_TO_MOC})
qt6_wrap_ui(CONFIG_TABS_WRAPPED_UI ${CONFIG_TABS_UI_FILES})

add_library(gui_config_tabs OBJECT ${CONFIG_TABS_SOURCES} ${CONFIG_TABS_WRAPPED_CPP} ${CONFIG_TABS_HEADERS} ${CONFIG_TABS_WRAPPED_UI})
target_link_libraries(gui_config_tabs
                        PRIVATE
                            core
                            Qt::Widgets
)

target_include_directories(gui_config_tabs
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
                                ${CMAKE_CURRENT_BINARY_DIR}
)
