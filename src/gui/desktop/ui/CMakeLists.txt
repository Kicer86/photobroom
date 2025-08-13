
find_package(Qt6 REQUIRED COMPONENTS Widgets Quick QuickWidgets Qml)

set(SRC
    mainwindow.cpp
    photos_grouping_dialog.cpp
)

set(HEADERS
    mainwindow.hpp
    photos_grouping_dialog.hpp
)

set(TO_MOC
    mainwindow.hpp
    photos_grouping_dialog.hpp
)

set(UI_FILES
    photos_grouping_dialog.ui
)

qt6_wrap_cpp(MOCED ${TO_MOC})
qt6_wrap_ui(WRAPPED_UI ${UI_FILES})

add_library(gui_ui OBJECT ${SRC} ${MOCED} ${WRAPPED_UI} ${HEADERS})
target_link_libraries(gui_ui
                        PUBLIC
                            Qt::Qml
                        PRIVATE
                            core
                            database
                            gui_models
                            photos_crawler
                            project_utils
                            system

                            Qt::Widgets
                            Qt::Quick
                            Qt::QuickWidgets
)

target_include_directories(gui_ui
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
                                ${CMAKE_CURRENT_BINARY_DIR}
)
