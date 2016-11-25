
include_directories(
                    $<TARGET_PROPERTY:configuration,INTERFACE_INCLUDE_DIRECTORIES>
)


set(CONFIG_TABS_SOURCES
    look_tab.cpp
    main_tab.cpp
    )

set(CONFIG_TABS_HEADERS
    look_tab.hpp
    main_tab.hpp
    )


set(CONFIG_TABS_FILES_TO_MOC
    look_tab.hpp
    main_tab.hpp
    )

set(CONFIG_TABS_UI_FILES
    look_tab.ui
    main_tab.ui
    )

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(CONFIG_TABS_WRAPPED_CPP ${CONFIG_TABS_FILES_TO_MOC})
qt5_wrap_ui(CONFIG_TABS_WRAPPED_UI ${CONFIG_TABS_UI_FILES})

add_library(gui_config_tabs OBJECT ${CONFIG_TABS_SOURCES} ${CONFIG_TABS_WRAPPED_CPP} ${CONFIG_TABS_HEADERS} ${CONFIG_TABS_WRAPPED_UI})
set_target_properties(gui_config_tabs PROPERTIES POSITION_INDEPENDENT_CODE ON)
