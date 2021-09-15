
find_package(Qt6 REQUIRED COMPONENTS Quick QuickWidgets)

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

qt6_wrap_cpp(MOCED ${TO_MOC})
qt6_wrap_ui(WRAPPED_UI ${UI_FILES})

add_library(gui_ui OBJECT ${SRC} ${MOCED} ${WRAPPED_UI} ${HEADERS})
set_target_properties(gui_ui PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gui_ui
                        PRIVATE
                            core
                            database
                            face_recognition
                            photos_crawler
                            project_utils
                            system
                            quick_views

                            Qt::Widgets
                            Qt::Quick
                            Qt::QuickWidgets
)

target_include_directories(gui_ui
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
                                ${CMAKE_CURRENT_BINARY_DIR}
)
