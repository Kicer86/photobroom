
include_directories(
                    $<TARGET_PROPERTY:core,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:photos_crawler,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:project_utils,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:system,INTERFACE_INCLUDE_DIRECTORIES>
)

set(SRC
    configuration_dialog.cpp
    faces_dialog.cpp
    mainwindow.cpp
    photos_grouping_dialog.cpp
)

set(HEADERS
    configuration_dialog.hpp
    faces_dialog.hpp
    mainwindow.hpp
    photos_grouping_dialog.hpp
)

set(TO_MOC
    configuration_dialog.hpp
    faces_dialog.hpp
    mainwindow.hpp
    photos_grouping_dialog.hpp
)

set(UI_FILES
    configuration_dialog.ui
    faces_dialog.ui
    mainwindow.ui
    photos_grouping_dialog.ui
)

source_group(files REGULAR_EXPRESSION .*gui.* )

qt5_wrap_cpp(MOCED ${TO_MOC})
qt5_wrap_ui(WRAPPED_UI ${UI_FILES})

add_library(gui_ui OBJECT ${SRC} ${MOCED} ${WRAPPED_UI} ${HEADERS})
set_target_properties(gui_ui PROPERTIES POSITION_INDEPENDENT_CODE ON)
