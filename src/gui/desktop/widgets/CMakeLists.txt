
find_package(Qt6 REQUIRED COMPONENTS QuickWidgets Widgets)

set(SRCS
    collection_dir_scan_dialog.cpp
    media_preview.cpp
    tasks_view_widget.cpp
    project_creator/project_creator_dialog.cpp
    series_detection/series_detection.cpp
    qtext_choosefile.cpp
)

set(HEADERS
    collection_dir_scan_dialog.hpp
    media_preview.hpp
    tasks_view_widget.hpp
    project_creator/project_creator_dialog.hpp
    series_detection/series_detection.hpp
    qtext_choosefile.hpp
)

#set of file to be moced
set(TO_MOC
    collection_dir_scan_dialog.hpp
    media_preview.hpp
    project_creator/project_creator_dialog.hpp
    series_detection/series_detection.hpp
    qtext_choosefile.hpp
)

source_group(files .*)
source_group(filters_editor .*\/filters_editor\/.* )
source_group(project_creator .*\/project_creator\/.* )
source_group(tag_editor .*\/tag_editor\/.* )
source_group(generated_files \(moc.*\)|\(ui_.*\)|\(qrc_.*\) )

qt6_wrap_cpp(MOCED ${TO_MOC})

add_library(gui_widgets OBJECT ${SRCS} ${MOCED} ${HEADERS})
set_target_properties(gui_widgets PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(gui_widgets
                        PRIVATE
                            core
                            database
                            face_recognition
                            photos_crawler
                            project_utils
                            quick_views
                            system
                            Qt::Widgets
                            Qt::QuickWidgets
)

target_include_directories(gui_widgets
                            PRIVATE
                                ${CMAKE_SOURCE_DIR}/src/gui/
                                ${CMAKE_SOURCE_DIR}/src/gui/desktop
)
